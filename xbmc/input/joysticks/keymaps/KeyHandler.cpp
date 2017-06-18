/*
 *      Copyright (C) 2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "KeyHandler.h"
#include "input/joysticks/interfaces/IKeymapHandler.h"
#include "input/joysticks/JoystickUtils.h"
#include "input/Action.h"
#include "input/ActionTranslator.h"
#include "input/IActionHandler.h"
#include "input/IKeymap.h"
#include "input/IKeymapEnvironment.h"

#include <algorithm>
#include <assert.h>

using namespace KODI;
using namespace JOYSTICK;

#define DIGITAL_ANALOG_THRESHOLD  0.5f

#define HOLD_TIMEOUT_MS     500
#define REPEAT_TIMEOUT_MS   50

CKeyHandler::CKeyHandler(const std::string &keyName, ACTIONS::IActionHandler *actionHandler, const IKeymap *keymap, IKeymapHandler *keymapHandler) :
  m_keyName(keyName),
  m_actionHandler(actionHandler),
  m_keymap(keymap),
  m_keymapHandler(keymapHandler)
{
  assert(m_actionHandler != nullptr);
  assert(m_keymap != nullptr);
  assert(m_keymapHandler != nullptr);

  Reset();
}

void CKeyHandler::Reset()
{
  m_bHeld = false;
  m_holdStartTimeMs = 0;
  m_lastHoldTimeMs = 0;
  m_bActionSent = false;
  m_lastActionMs = 0;
}

bool CKeyHandler::OnDigitalMotion(bool bPressed, unsigned int holdTimeMs)
{
  return OnAnalogMotion(bPressed ? 1.0f : 0.0f, holdTimeMs);
}

bool CKeyHandler::OnAnalogMotion(float magnitude, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate press state
  const bool bPressed = (magnitude >= DIGITAL_ANALOG_THRESHOLD);
  const bool bJustPressed = bPressed && !m_bHeld;

  if (bJustPressed)
  {
    // Reset key if just pressed
    Reset();

    // Record hold start time if just pressed
    m_holdStartTimeMs = motionTimeMs;
  }

  // Calculate holdtime relative to when magnitude crossed the threshold
  unsigned int holdTimeMs = 0;
  if (bPressed)
    holdTimeMs = motionTimeMs - m_holdStartTimeMs;

  // Get actions for the key
  const auto &actions = m_keymap->GetActions(m_keyName);
  if (!actions.empty())
  {
    bHandled = SendActions(actions, magnitude, holdTimeMs);
  }

  m_bHeld = bPressed;
  m_lastHoldTimeMs = holdTimeMs;

  return bHandled;
}

bool CKeyHandler::SendActions(const KeymapActions& actions, float magnitude, unsigned int holdTimeMs)
{
  bool bHandled = false;

  // Calculate press state
  const bool bPressed = (magnitude >= DIGITAL_ANALOG_THRESHOLD);
  const bool bJustPressed = bPressed && !m_bHeld;
  const bool bJustReleased = !bPressed && m_bHeld;

  // Actions are sorted by holdtime, so the final action is the one with the
  // greatest holdtime
  const KeymapAction& finalAction = *actions.rbegin();
  const unsigned int maxHoldTimeMs = finalAction.holdTimeMs;

  const bool bHasDelay = (maxHoldTimeMs != 0);
  if (!bHasDelay)
  {
    bHandled = SendAction(finalAction, magnitude, holdTimeMs);
  }
  else
  {
    // If holdtime has exceeded the last action, execute it now
    if (holdTimeMs >= finalAction.holdTimeMs)
    {
      // Force holdtime to zero for the initial press
      bHandled = SendAction(finalAction, magnitude, m_bActionSent ? holdTimeMs - finalAction.holdTimeMs : 0);
    }
    else
    {
      bool bSendReleaseAction = true;

      // Don't send an action on release if one has already been sent
      if (m_bActionSent)
        bSendReleaseAction = false;

      // Don't send a release action if button is still pressed
      else if (bPressed)
        bSendReleaseAction = false;

      if (bSendReleaseAction)
      {
        // Analog actions can't be overridden with a greater holdtime
        std::vector<const KeymapAction*> digitalActions;
        for (const auto& action : actions)
        {
          if (!CActionTranslator::IsAnalog(action.actionId))
            digitalActions.push_back(&action);
        }

        // Find the maximum holdtime less than the current holdtime
        for (auto it = digitalActions.begin(); it != digitalActions.end(); )
        {
          const KeymapAction &action = **it;

          unsigned int thisHoldTime = (*it)->holdTimeMs;

          ++it;
          if (it == digitalActions.end())
            break;

          unsigned int nextHoldTime = (*it)->holdTimeMs;

          // If button was just released, holdtime will be 0, so use the prior
          // holdtime instead
          unsigned int actionHoldtime = (bJustReleased ? m_lastHoldTimeMs : holdTimeMs);

          if (thisHoldTime <= actionHoldtime && actionHoldtime < nextHoldTime)
          {
            bHandled = SendAction(action, 1.0f, 0);
            break;
          }
        }
      }
    }
  }

  return bHandled;
}

bool CKeyHandler::SendAction(const KeymapAction& action, float magnitude, unsigned int holdTimeMs)
{
  // Calculate press state
  const bool bPressed = (magnitude >= DIGITAL_ANALOG_THRESHOLD);

  bool bSendAction = false;

  if (HotkeysPressed(action.hotkeys))
  {
    if (CActionTranslator::IsAnalog(action.actionId))
    {
      // Don't send action for 0.0 magnitude
      bSendAction = (magnitude > 0.0f);
    }
    else
    {
      // Don't send action for releases
      if (bPressed)
      {
        // Dispatch action if button was pressed this frame
        if (holdTimeMs == 0)
          bSendAction = true;
        else
          bSendAction = SendRepeatAction(holdTimeMs);
      }
    }
  }

  if (bSendAction)
  {
    const CAction guiAction(action.actionId, magnitude, 0.0f, action.actionString, holdTimeMs);
    m_actionHandler->SendAction(guiAction);
    m_keymapHandler->OnPress(m_keyName);
    m_bActionSent = true;
    m_lastActionMs = holdTimeMs;
    return true;
  }

  return false;
}

bool CKeyHandler::HotkeysPressed(const std::set<std::string> &hotkeys) const
{
  // No hotkeys to press
  if (hotkeys.empty())
    return true;

  // Look for unpressed hotkey
  auto it = std::find_if(hotkeys.begin(), hotkeys.end(),
    [this](const std::string &hotkey)
    {
      return !m_keymapHandler->IsPressed(CJoystickUtils::MakeKeyName(hotkey));
    });

  // Return true if all hotkeys are pressed
  return it == hotkeys.end();
}

bool CKeyHandler::SendRepeatAction(unsigned int holdTimeMs)
{
  bool bSendRepeat = true;

  // Don't send a repeat action if the last key has changed
  if (m_keymapHandler->GetLastPressed() != m_keyName)
    bSendRepeat = false;

  // Ensure initial timeout has elapsed
  else if (holdTimeMs < HOLD_TIMEOUT_MS)
    bSendRepeat = false;

  // Ensure repeat timeout has elapsed
  else if (holdTimeMs < m_lastActionMs + REPEAT_TIMEOUT_MS)
    bSendRepeat = false;

  return bSendRepeat;
}
