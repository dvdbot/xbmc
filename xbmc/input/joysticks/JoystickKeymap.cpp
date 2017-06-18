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

#include "JoystickKeymap.h"

#include <algorithm>

using namespace KODI;
using namespace JOYSTICK;

void CJoystickKeymap::MapAction(int windowId, const std::string& controllerId, const std::string& feature, KODI::JOYSTICK::ANALOG_STICK_DIRECTION dir, unsigned int holdtimeMs, std::set<std::string> hotkeys, CAction action)
{
  WindowKeymap& windowKeymap = m_joystickKeymap[windowId];

  auto FindControllerID = [&controllerId](const JoystickKeymap& entry)
  {
    return entry.controllerId == controllerId;
  };

  // Look up existing keymap for the controller profile
  auto itJoystick = std::find_if(windowKeymap.begin(), windowKeymap.end(), FindControllerID);

  if (itJoystick == windowKeymap.end())
  {
    windowKeymap.emplace_back(JoystickKeymap{ controllerId });
    itJoystick = std::find_if(windowKeymap.begin(), windowKeymap.end(), FindControllerID);
  }

  JoystickKeymap &keymap = *itJoystick;
  OverloadedButton& button = keymap.keymap[KeymapFeature(feature, dir)];

  button.actions.insert(std::make_pair(holdtimeMs, KeymapEntry{ std::move(action), std::move(hotkeys) }));
}

void CJoystickKeymap::Clear()
{
  m_joystickKeymap.clear();
}
