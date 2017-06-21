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
#include "input/Action.h"
#include "input/ActionTranslator.h"
#include "input/IActionHandler.h"
#include "input/IKeymap.h"

#include <algorithm>
#include <assert.h>

using namespace KODI;
using namespace JOYSTICK;

#define DIGITAL_ANALOG_THRESHOLD  0.5f

#define HOLD_TIMEOUT_MS     500
#define REPEAT_TIMEOUT_MS   50

CKeyHandler::CKeyHandler(const std::string &keyName, IActionHandler *actionHandler, const IKeymap *keymap, IKeymapHandler *keymapHandler) :
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
  m_lastActionMs = 0;
  m_lastHoldTimeMs = 0;
}

bool CKeyHandler::OnDigitalMotion(bool bPressed, unsigned int holdTimeMs)
{
  return OnAnalogMotion(bPressed ? 1.0f : 0.0f, holdTimeMs);
}

bool CKeyHandler::OnAnalogMotion(float magnitude, unsigned int motionTimeMs)
{
  const auto &actions = m_keymap->GetActions(m_keyName);
  if (!actions.empty())
  {
    // Get action properties
    const KeymapAction& finalAction = *actions.rbegin();

    // Actions are sorted by holdtime, so the final holdtime is the maximum one
    const bool bHasDelay = (finalAction.holdTimeMs > 0);

    // Calculate press state
    const bool bPressed = (magnitude >= DIGITAL_ANALOG_THRESHOLD);
    const bool bJustPressed = bPressed && !m_bHeld;
    const bool bJustReleased = !bPressed && m_bHeld;

    // Update hold start time
    if (bJustPressed)
      m_holdStartTimeMs = motionTimeMs;

    // Calculate holdtime
    unsigned int holdTimeMs = 0;
    if (m_bHeld)
      holdTimeMs = motionTimeMs - m_holdStartTimeMs;

    if (!bHasDelay)
    {
      SendAction(finalAction, magnitude, holdTimeMs);
    }
    else
    {
      // If holdtime has exceeded the last action, execute it now
      if (holdTimeMs >= finalAction.holdTimeMs)
      {
        SendAction(finalAction, magnitude, holdTimeMs - finalAction.holdTimeMs);
      }
      else if (bJustReleased)
      {
        // Find the maximum holdtime less than the current holdtime
        auto it = actions.rend();
        for ( ; it != actions.rbegin(); ++it)
        {
          // Analog actions can't be overridden with a greater holdtime
          if (CActionTranslator::IsAnalog(it->actionId))
            continue;

          if (it->holdTimeMs < m_lastHoldTimeMs)
            break;
        }

        if (it != actions.rbegin())
          SendAction(finalAction, magnitude, holdTimeMs);
      }
    }

    m_bHeld = bPressed;
    m_lastHoldTimeMs = holdTimeMs;

    return true;
  }

  return false;
}

bool CKeyHandler::SendAction(const KeymapAction& action, float magnitude, unsigned int holdTimeMs)
{
  bool bSendAction = false;

  const auto &hotkeys = action.hotkeys;
  bool bHotkeysPressed = std::find_if(hotkeys.begin(), hotkeys.end(),
    [this](const std::string &hotkey)
    {
      return m_keymapHandler->IsPressed(hotkey);
    }) != hotkeys.end();

  if (bHotkeysPressed)
  {
    if (CActionTranslator::IsAnalog(action.actionId))
    {
      // Always send analog actions
      bSendAction = true;
    }
    else
    {
      if (holdTimeMs == 0)
      {
        // Dispatch action if button was pressed this frame
        bSendAction = true;
      }
      else
      {
        // Check criteria for sending a repeat action
        bool bSendRepeat = true;

        // Don't send a repeat action if the last key has changed
        if (m_keymapHandler->GetLastPressed() != m_keyName)
          bSendRepeat = false;

        // Ensure initial timeout has elapsed
        if (holdTimeMs < HOLD_TIMEOUT_MS)
          bSendRepeat = false;

        // Ensure repeat timeout has elapsed
        if (holdTimeMs < m_lastActionMs + REPEAT_TIMEOUT_MS)
          bSendRepeat = false;

        bSendAction = bSendRepeat;
      }
    }
  }

  if (bSendAction)
  {
    const CAction guiAction(action.actionId, magnitude, 0.0f, action.actionString, holdTimeMs);
    m_actionHandler->SendAction(guiAction);
    m_lastActionMs = holdTimeMs;
    return true;
  }

  return false;
}
