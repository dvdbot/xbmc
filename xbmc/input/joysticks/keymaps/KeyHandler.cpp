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
#include <utility>

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
}

bool CKeyHandler::OnDigitalMotion(bool bPressed, unsigned int holdTimeMs)
{
  auto &actions = m_keymap->GetActions(m_keyName);
  if (!actions.empty())
  {
    const KeymapAction &keymapAction = *actions.begin();

    const unsigned int actionId = keymapAction.actionId;
    const std::string &actionString = keymapAction.actionString;
    const unsigned int requriedHoldTimeMs = keymapAction.holdTimeMs;

    if (CActionTranslator::IsAnalog(actionId))
    {
      if (bPressed)
      {
        CAction action(actionId, 1.0f, 0.0f, actionString);
        SendAnalogAction(action);
      }
    }
    else
    {
      if (bPressed)
      {
        if (holdTimeMs >= requriedHoldTimeMs)
        {
          if (!m_bPressed)
            holdTimeMs = requriedHoldTimeMs;

          CAction action(actionId, actionString);
          action.SetHoldTime(holdTimeMs - requriedHoldTimeMs);
          SendDigitalAction(action);
        }
      }
      else
      {
        m_bPressed = false;
      }
    }

    return true;
  }

  return false;
}

bool CKeyHandler::OnAnalogMotion(float magnitude, unsigned int motionTimeMs)
{
  auto &actions = m_keymap->GetActions(m_keyName);
  if (!actions.empty())
  {
    const KeymapAction &keymapAction = *actions.begin();

    const unsigned int actionId = keymapAction.actionId;
    const std::string &actionString = keymapAction.actionString;
    const unsigned int requriedHoldTimeMs = keymapAction.holdTimeMs;

    if (CActionTranslator::IsAnalog(actionId))
    {
      if (magnitude > 0.0f)
      {
        CAction action(actionId, magnitude, 0.0f, actionString);
        m_actionHandler->SendAction(action);
      }
    }
    else
    {
      const bool bIsPressed = (magnitude >= DIGITAL_ANALOG_THRESHOLD);
      if (bIsPressed)
      {
        if (!m_bHeld)
        {
          m_holdStartTime = motionTimeMs;
          m_bHeld = true;
        }

        unsigned int holdTimeMs = motionTimeMs - m_holdStartTime;

        if (holdTimeMs >= requriedHoldTimeMs)
        {
          if (!m_bPressed)
            holdTimeMs = requriedHoldTimeMs;

          CAction action(actionId, actionString);
          action.SetHoldTime(holdTimeMs - requriedHoldTimeMs);
          SendDigitalAction(action);
        }
      }
      else
      {
        m_holdStartTime = 0;
        m_bPressed = false;
        m_bHeld = false;
      }
    }

    return true;
  }

  return false;
}

void CKeyHandler::SendDigitalAction(const CAction& action)
{
  const unsigned int holdTimeMs = action.GetHoldTime();

  if (!m_bPressed)
  {
    // Only dispatch action if button was pressed this frame
    if (holdTimeMs == 0)
    {
      if (m_actionHandler->SendAction(action))
        m_lastDigitalActionMs = holdTimeMs;
    }
  }
  else if (m_keymapHandler->GetLastPressed() == m_keyName && holdTimeMs > HOLD_TIMEOUT_MS)
  {
    if (holdTimeMs > m_lastDigitalActionMs + REPEAT_TIMEOUT_MS)
    {
      m_actionHandler->SendAction(action);
      m_lastDigitalActionMs = holdTimeMs;
    }
  }

  m_bPressed = true;
}

void CKeyHandler::SendAnalogAction(const CAction& action)
{
  m_actionHandler->SendAction(action);
}
