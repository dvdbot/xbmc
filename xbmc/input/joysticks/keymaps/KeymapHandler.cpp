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
#include "games/controllers/Controller.h"
#include "games/GameServices.h"
#include "input/joysticks/interfaces/IKeyHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/joysticks/JoystickUtils.h"
#include "input/IKeymap.h"
#include "input/IKeymapEnvironment.h"
#include "ServiceBroker.h"

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <utility>

using namespace KODI;
using namespace JOYSTICK;

CKeymapHandler::CKeymapHandler(IActionHandler *handler, const IKeymap *keymap) :
  m_handler(handler),
  m_keymap(keymap)
{
  assert(m_handler != nullptr);
  assert(m_keymap != nullptr);
}

bool CKeymapHandler::IsPressed(const std::string& keyName) const
{
  auto it = m_keyHandlers.find(keyName);
  if (it != m_keyHandlers.end())
    return it->second->IsPressed();

  return false;
}

std::string CKeymapHandler::ControllerID() const
{
  return m_keymap->ControllerID();
}

bool CKeymapHandler::HasAction(const FeatureName& feature) const
{
  bool bHasAction = false;

  auto controller = CServiceBroker::GetGameServices().GetController(ControllerID());
  if (controller)
  {
    switch (controller->GetFeatureType(feature))
    {
    case FEATURE_TYPE::SCALAR:
    {
      bHasAction = !m_keymap->GetActions(CJoystickUtils::MakeKeyName(feature)).empty();
      break;
    }
    case FEATURE_TYPE::ANALOG_STICK:
    {
      for (auto dir : CJoystickUtils::GetDirections())
      {
        if (!m_keymap->GetActions(CJoystickUtils::MakeKeyName(feature, dir)).empty())
        {
          bHasAction = true;
          break;
        }
      }
      break;
    }
    default:
      break;
    }
  }

  return bHasAction;
}

bool CKeymapHandler::OnButtonPress(const FeatureName& feature, bool bPressed)
{
  const std::string keyName = CJoystickUtils::MakeKeyName(feature);

  IKeyHandler *handler = GetKeyHandler(keyName);
  return handler->OnDigitalMotion(bPressed, 0);
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
  return handler->OnAnalogMotion(magnitude, motionTimeMs);
}

bool CKeymapHandler::OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs)
{
  bool bHandled = false;

  // Calculate the direction of the stick's position
  const ANALOG_STICK_DIRECTION analogStickDir = CJoystickTranslator::VectorToAnalogStickDirection(x, y);

  // Calculate the magnitude projected onto that direction
  const float magnitude = std::max(std::fabs(x), std::fabs(y));

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
  return handler->OnAnalogMotion(magnitude, motionTimeMs);
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
    m_keyHandlers.insert(std::make_pair(keyName, std::unique_ptr<IKeyHandler>(new CKeyHandler(keyName, m_handler, m_keymap, this))));
    it = m_keyHandlers.find(keyName);
  }

  return it->second.get();
}
