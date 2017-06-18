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

#include "KeymapHandling.h"
#include "input/joysticks/IActionHandler.h"
#include "input/joysticks/IKeymapHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/ActionIDs.h"
#include "Application.h"

#include <algorithm>
#include <assert.h>
#include <utility>

using namespace KODI;
using namespace JOYSTICK;

#define HOLD_TIMEOUT_MS     500
#define REPEAT_TIMEOUT_MS   50


// --- CKeymapUtils ------------------------------------------------------------

const std::vector<ANALOG_STICK_DIRECTION>& CKeymapUtils::GetDirections(void)
{
  static std::vector<ANALOG_STICK_DIRECTION> directions;
  if (directions.empty())
  {
    directions.push_back(ANALOG_STICK_DIRECTION::UP);
    directions.push_back(ANALOG_STICK_DIRECTION::DOWN);
    directions.push_back(ANALOG_STICK_DIRECTION::RIGHT);
    directions.push_back(ANALOG_STICK_DIRECTION::LEFT);
  }
  return directions;
}

// --- CHotkey ---------------------------------------------------------------

void CHotkey::OnButtonPress(bool bPressed)
{
  m_bPressed = bPressed;
}

void CHotkey::OnButtonMotion(float magnitude)
{
  m_bPressed = (magnitude >= ANALOG_DIGITAL_THRESHOLD);
}

// --- CKeymapButton -----------------------------------------------------------

CKeymapButton::CKeymapButton(CAction action, IActionHandler *actionHandler) :
  m_action(std::move(action)),
  m_actionHandler(actionHandler)
{
}

std::unique_ptr<IKeymapHandler> CKeymapButton::CreateButton(std::string actionString, IActionHandler *actionHandler)
{
  std::unique_ptr<IKeymapHandler> button;

  CAction action(std::move(actionString));
  if (action.IsAnalog())
    button.reset(new CAnalogButton(std::move(action), actionHandler));
  else
    button.reset(new CDigitalButton(std::move(action), actionHandler));

  return button;
}

unsigned int CKeymapButton::GetActionID() const
{
  return std::max(m_action.GetID(), 0);
}

// --- CAnalogButton ---------------------------------------------------------

CAnalogButton::CAnalogButton(CAction action, IActionHandler *actionHandler) :
  CKeymapButton(std::move(action), actionHandler)
{
}

void CAnalogButton::OnDigitalKey(bool bPressed, unsigned int holdTimeMs)
{
  return OnAnalogKey(bPressed ? 1.0f : 0.0f, holdTimeMs);
}

void CAnalogButton::OnAnalogKey(float magnitude, unsigned int motionTimeMs)
{
  m_actionHandler->SendAnalogAction(m_action, magnitude);
}

// --- CDigitalButton ----------------------------------------------------------

CDigitalButton::CDigitalButton(CAction action, IActionHandler *actionHandler) :
  CKeymapButton(std::move(action), actionHandler)
{
}

void CDigitalButton::OnDigitalKey(bool bPressed, unsigned int holdTimeMs)
{
  if (!m_bPressed)
  {
    m_bPressed = true;

    // Only dispatch action if button was pressed this frame
    if (holdTimeMs == 0)
    {
      if (m_actionHandler->SendDigitalAction(m_action))
        m_lastDigitalActionMs = holdTimeMs;
    }
  }
  else if (holdTimeMs > HOLD_TIMEOUT_MS)
  {
    if (holdTimeMs > m_lastDigitalActionMs + REPEAT_TIMEOUT_MS)
    {
      m_actionHandler->SendDigitalAction(m_action);
      m_lastDigitalActionMs = holdTimeMs;
    }
  }
}

void CDigitalButton::OnAnalogKey(float magnitude, unsigned int motionTimeMs)
{

}

// --- COverloadedButton -------------------------------------------------------

void COverloadedButton::RegisterAction(unsigned int holdtimeMs, std::shared_ptr<const CHotkey> hotkey, std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> handler)
{
  KeymapEntry entry{ std::move(hotkey), std::move(handler) };
  m_actions[holdtimeMs] = std::move(entry);
}

void COverloadedButton::Clear()
{
  m_actions.clear();
}

INPUT_TYPE COverloadedButton::GetInputType() const
{
  INPUT_TYPE inputType = INPUT_TYPE::UNKNOWN;

  const unsigned int holdtimeMs = 0;
  auto it = m_actions.find(holdtimeMs);
  if (it != m_actions.end())
    inputType = it->second.handler->GetInputType();

  return inputType;
}

unsigned int COverloadedButton::GetActionID() const
{
  const unsigned int holdtimeMs = 0;

  auto it = m_actions.find(holdtimeMs);
  if (it != m_actions.end())
    return it->second.handler->GetActionID();

  return ACTION_NONE;
}

void COverloadedButton::OnDigitalKey(bool bPressed, unsigned int holdTimeMs)
{
  //! @todo
}

void COverloadedButton::OnAnalogKey(float magnitude, unsigned int motionTimeMs)
{
  //! @todo
}

// --- CJoystickKeymap -------------------------------------------------------

CJoystickKeymap::KeymapFeature::KeymapFeature(std::string feature, ANALOG_STICK_DIRECTION dir /* = ANALOG_STICK_DIRECTION::UNKNOWN */) :
  feature(std::move(feature)),
  dir(dir)
{
}

CJoystickKeymap::CJoystickKeymap(std::string controllerId) :
  m_controllerId(std::move(controllerId))
{
}

void CJoystickKeymap::AddHandler(const FeatureName& feature, ANALOG_STICK_DIRECTION dir, unsigned int holdtimeMs, const std::string &hotkey, std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> handler)
{
  // Find/create hotkey object
  std::shared_ptr<CHotkey>& hotkeyPtr = m_hotkeys[hotkey];
  if (!hotkeyPtr)
    hotkeyPtr.reset(new CHotkey);

  // Find overloaded button
  KeymapFeature featureKey{ feature, dir };
  COverloadedButton &button = GetButton(featureKey);

  // Register action
  button.RegisterAction(holdtimeMs, hotkeyPtr, std::move(handler));
}

void CJoystickKeymap::Clear()
{
  m_keymap.clear();
  m_hotkeys.clear();
}

bool CJoystickKeymap::HasFeature(const FeatureName& feature) const
{
  KeymapFeature featureKey{ feature };
  if (HasButton(featureKey))
    return true;

  for (auto dir : CKeymapUtils::GetDirections())
  {
    featureKey.dir = dir;
    if (HasButton(featureKey))
      return true;
  }

  return false;
}

INPUT_TYPE CJoystickKeymap::GetInputType(const FeatureName& feature) const
{
  const COverloadedButton &button = GetButton(KeymapFeature{ feature });
  return button.GetInputType();
}

bool CJoystickKeymap::OnButtonPress(const FeatureName& feature, bool bPressed)
{
  bool bHandled = false;

  KeymapFeature featureKey(feature);
  auto it = m_keymap.find(featureKey);
  if (it != m_keymap.end())
  {
    COverloadedButton &button = it->second;
    button.OnDigitalKey(bPressed, 0);
    bHandled = true;
  }

  return bHandled;
}

void CJoystickKeymap::OnButtonHold(const FeatureName& feature, unsigned int holdTimeMs)
{
  auto it = m_keymap.find(KeymapFeature{ feature });
  if (it != m_keymap.end())
  {
    COverloadedButton &button = it->second;
    button.OnDigitalKey(true, holdTimeMs);
  }
}

bool CJoystickKeymap::OnButtonMotion(const FeatureName& feature, float magnitude, unsigned int motionTimeMs)
{
  bool bHandled = false;

  auto it = m_keymap.find(KeymapFeature{ feature });
  if (it != m_keymap.end())
  {
    COverloadedButton &button = it->second;
    button.OnAnalogKey(magnitude, motionTimeMs);
    bHandled = true;
  }

  return bHandled;
}

bool CJoystickKeymap::OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the direction of the stick's position
  const ANALOG_STICK_DIRECTION analogStickDir = CJoystickTranslator::VectorToAnalogStickDirection(x, y);

  // Calculate the magnitude projected onto that direction
  const float magnitude = std::max(std::abs(x), std::abs(y));

  // Deactivate directions in which the stick is not pointing first
  for (auto dir : CKeymapUtils::GetDirections())
  {
    if (dir != analogStickDir)
      DeactivateDirection(feature, dir);
  }

  // Now activate direction the analog stick is pointing
  if (analogStickDir != ANALOG_STICK_DIRECTION::UNKNOWN)
    bHandled = ActivateDirection(feature, magnitude, analogStickDir, motionTimeMs);

  return bHandled;
}

bool CJoystickKeymap::OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z)
{
  return false; //! @todo implement
}

bool CJoystickKeymap::ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs)
{
  bool bHandled = false;

  auto it = m_keymap.find(KeymapFeature{ feature, dir });
  if (it != m_keymap.end())
  {
    COverloadedButton &button = it->second;
    button.OnAnalogKey(magnitude, motionTimeMs);
    bHandled = true;
  }

  return bHandled;
}

void CJoystickKeymap::DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir)
{
  auto it = m_keymap.find(KeymapFeature{ feature, dir });
  if (it != m_keymap.end())
  {
    COverloadedButton &button = it->second;
    button.OnAnalogKey(0.0f, 0);
  }
}

bool CJoystickKeymap::HasButton(const KeymapFeature& feature) const
{
  bool bFound = false;

  auto it = m_keymap.find(feature);
  if (it != m_keymap.end())
    bFound = true;

  return bFound;
}

COverloadedButton &CJoystickKeymap::GetButton(const KeymapFeature& feature)
{
  return m_keymap[feature];
}

const COverloadedButton &CJoystickKeymap::GetButton(const KeymapFeature& feature) const
{
  auto it = m_keymap.find(feature);
  if (it != m_keymap.end())
    return it->second;

  static COverloadedButton empty;
  return empty;
}

// --- CKeymapHandling ---------------------------------------------------------
/*
CKeymapHandling::CKeymapHandling(IKeymapHandler* handler, IKeymapTranslator* keymap) :
  m_handler(handler),
  m_keymap(keymap)
{
  assert(m_handler != nullptr);
  assert(m_keymap != nullptr);
}

std::string CKeymapHandling::ControllerID(void) const
{
  return m_keymap->ControllerID();
}

bool CKeymapHandling::HasFeature(const FeatureName& feature) const
{
  if (m_keymap->GetKeyID(feature) != 0)
    return true;

  // Try analog stick directions
  if (m_keymap->GetKeyID(feature, ANALOG_STICK_DIRECTION::UP) != 0 ||
      m_keymap->GetKeyID(feature, ANALOG_STICK_DIRECTION::DOWN) != 0 ||
      m_keymap->GetKeyID(feature, ANALOG_STICK_DIRECTION::RIGHT) != 0 ||
      m_keymap->GetKeyID(feature, ANALOG_STICK_DIRECTION::LEFT) != 0)
    return true;

  return false;
}

bool CKeymapHandling::AcceptsInput()
{
  //return IsAppFocused();
  return g_application.IsAppFocused();
}

INPUT_TYPE CKeymapHandling::GetInputType(const FeatureName& feature) const
{
  return m_handler->GetInputType(m_keymap->GetKeyID(feature), GetWindowID(), GetFallthrough());
}

bool CKeymapHandling::OnButtonPress(const FeatureName& feature, bool bPressed)
{
  if (bPressed && m_easterEgg && m_easterEgg->OnButtonPress(feature))
    return true;

  const unsigned int keyId = m_keymap->GetKeyID(feature);
  const int windowId = GetWindowID();
  const bool bFallthrough = GetFallthrough();

  if (m_handler->GetInputType(keyId, windowId, bFallthrough) == INPUT_TYPE::DIGITAL)
  {
    m_handler->OnDigitalKey(keyId, windowId, bFallthrough, bPressed);
    return true;
  }

  return false;
}

void CKeymapHandling::OnButtonHold(const FeatureName& feature, unsigned int holdTimeMs)
{
  const unsigned int keyId = m_keymap->GetKeyID(feature);
  const int windowId = GetWindowID();
  const bool bFallthrough = GetFallthrough();

  m_handler->OnDigitalKey(keyId, windowId, bFallthrough, true, holdTimeMs);
}

bool CKeymapHandling::OnButtonMotion(const FeatureName& feature, float magnitude)
{
  const unsigned int keyId = m_keymap->GetKeyID(feature);
  const int windowId = GetWindowID();
  const bool bFallthrough = GetFallthrough();

  if (m_handler->GetInputType(keyId, windowId, bFallthrough) == INPUT_TYPE::ANALOG)
  {
    m_handler->OnAnalogKey(keyId, windowId, bFallthrough, magnitude);
    return true;
  }

  return false;
}

bool CKeymapHandling::OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the direction of the stick's position
  const ANALOG_STICK_DIRECTION analogStickDir = CJoystickTranslator::VectorToAnalogStickDirection(x, y);

  // Calculate the magnitude projected onto that direction
  const float magnitude = std::max(std::abs(x), std::abs(y));

  // Deactivate directions in which the stick is not pointing first
  for (ANALOG_STICK_DIRECTION dir : GetDirections())
  {
    if (dir != analogStickDir)
      DeactivateDirection(feature, dir);
  }

  // Now activate direction the analog stick is pointing
  if (magnitude != 0.0f)
    bHandled = ActivateDirection(feature, magnitude, analogStickDir, motionTimeMs);

  return bHandled;
}

bool CKeymapHandling::OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z)
{
  return false; //! @todo implement
}

unsigned int CKeymapHandling::GetActionID(const FeatureName& feature)
{
  return m_handler->GetActionID(m_keymap->GetKeyID(feature), GetWindowID(), GetFallthrough());
}

int CKeymapHandling::GetWindowID() const
{
  return g_windowManager.GetActiveWindowID();
}

bool CKeymapHandling::ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the button key ID and input type for the analog stick's direction
  const unsigned int  keyId = m_keymap->GetKeyID(feature, dir);
  const int           windowId = GetWindowID();
  const bool          bFallthrough = GetFallthrough();
  const INPUT_TYPE    inputType = m_handler->GetInputType(keyId, windowId, bFallthrough);

  if (inputType == INPUT_TYPE::DIGITAL)
  {
    unsigned int holdTimeMs = 0;

    const bool bIsPressed = (magnitude >= ANALOG_DIGITAL_THRESHOLD);
    if (bIsPressed)
    {
      const bool bIsHeld = (m_holdStartTimes.find(keyId) != m_holdStartTimes.end());
      if (bIsHeld)
        holdTimeMs = motionTimeMs - m_holdStartTimes[keyId];
      else
        m_holdStartTimes[keyId] = motionTimeMs;
    }
    else
    {
      m_holdStartTimes.erase(keyId);
    }

    m_handler->OnDigitalKey(keyId, windowId, bFallthrough, bIsPressed, holdTimeMs);
    bHandled = true;
  }
  else if (inputType == INPUT_TYPE::ANALOG)
  {
    m_handler->OnAnalogKey(keyId, windowId, bFallthrough, magnitude);
    bHandled = true;
  }

  if (bHandled)
    m_currentDirections[feature] = dir;

  return bHandled;
}

void CKeymapHandling::DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir)
{
  if (m_currentDirections[feature] == dir)
  {
    // Calculate the button key ID and input type for this direction
    const unsigned int  keyId = m_keymap->GetKeyID(feature, dir);
    const int           windowId = GetWindowID();
    const bool          bFallthrough = GetFallthrough();
    const INPUT_TYPE    inputType = m_handler->GetInputType(keyId, windowId, bFallthrough);

    if (inputType == INPUT_TYPE::DIGITAL)
    {
      m_handler->OnDigitalKey(keyId, windowId, bFallthrough, false);
    }
    else if (inputType == INPUT_TYPE::ANALOG)
    {
      m_handler->OnAnalogKey(keyId, windowId, bFallthrough, 0.0f);
    }

    m_holdStartTimes.erase(keyId);
    m_currentDirections[feature] = ANALOG_STICK_DIRECTION::UNKNOWN;
  }
}

const std::vector<ANALOG_STICK_DIRECTION>& CKeymapHandling::GetDirections(void)
{
  static std::vector<ANALOG_STICK_DIRECTION> directions;
  if (directions.empty())
  {
    directions.push_back(ANALOG_STICK_DIRECTION::UP);
    directions.push_back(ANALOG_STICK_DIRECTION::DOWN);
    directions.push_back(ANALOG_STICK_DIRECTION::RIGHT);
    directions.push_back(ANALOG_STICK_DIRECTION::LEFT);
  }
  return directions;
}
*/
