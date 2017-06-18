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

#include "KeymapHandler.h"
#include "KeyHandler.h"
#include "input/joysticks/interfaces/IKeyHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/joysticks/JoystickUtils.h"
#include "input/IKeymap.h"
#include "input/IKeymapEnvironment.h"

#include <algorithm>
#include <assert.h>
#include <utility>

using namespace KODI;
using namespace JOYSTICK;

CKeymapHandler::CKeymapHandler(IActionHandler *handler, const IKeymap *keymap) :
  m_handler(handler),
  m_keymap(keymap)
{
  assert(handler != nullptr);
  assert(keymap != nullptr);
}

std::string CKeymapHandler::ControllerID() const
{
  return m_keymap->ControllerID();
}

bool CKeymapHandler::AcceptsInput()
{
  return m_keymap->Environment()->AcceptsKeymapInput();
}

bool CKeymapHandler::OnButtonPress(const FeatureName& feature, bool bPressed)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature);

  IKeyHandler *handler = GetKeyHandler(keyName);
  bool bHandled = handler->OnDigitalMotion(bPressed, 0);

  if (bHandled)
    m_lastPressed = keyName;

  return true;
}

void CKeymapHandler::OnButtonHold(const FeatureName& feature, unsigned int holdTimeMs)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature);

  IKeyHandler *handler = GetKeyHandler(keyName);
  handler->OnDigitalMotion(true, holdTimeMs);
}

bool CKeymapHandler::OnButtonMotion(const FeatureName& feature, float magnitude, unsigned int motionTimeMs)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature);

  IKeyHandler *handler = GetKeyHandler(keyName);
  bool bHandled = handler->OnAnalogMotion(magnitude, motionTimeMs);

  return true;
}

bool CKeymapHandler::OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the direction of the stick's position
  const ANALOG_STICK_DIRECTION analogStickDir = CJoystickTranslator::VectorToAnalogStickDirection(x, y);

  // Calculate the magnitude projected onto that direction
  const float magnitude = std::max(std::abs(x), std::abs(y));

  // Deactivate directions in which the stick is not pointing first
  for (auto dir : CJoystickUtils::GetDirections())
  {
    if (dir != analogStickDir)
      DeactivateDirection(feature, dir);
  }

  // Now activate direction the analog stick is pointing
  if (analogStickDir != ANALOG_STICK_DIRECTION::UNKNOWN)
    bHandled = ActivateDirection(feature, magnitude, analogStickDir, motionTimeMs);

  return bHandled;
}

bool CKeymapHandler::OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z)
{
  return false; //! @todo implement
}

bool CKeymapHandler::ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature, dir);

  IKeyHandler *handler = GetKeyHandler(keyName);
  handler->OnAnalogMotion(magnitude, motionTimeMs);

  return true;
}

void CKeymapHandler::DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature, dir);

  IKeyHandler *handler = GetKeyHandler(keyName);
  handler->OnAnalogMotion(0.0f, 0);
}

IKeyHandler *CKeymapHandler::GetKeyHandler(const std::string &keyName)
{
  auto it = m_keyHandlers.find(keyName);
  if (it == m_keyHandlers.end())
  {
    m_keyHandlers.insert(std::make_pair(keyName, new CKeyHandler(keyName, m_handler, m_keymap, this)));
    it = m_keyHandlers.find(keyName);
  }

  return it->second.get();
}
