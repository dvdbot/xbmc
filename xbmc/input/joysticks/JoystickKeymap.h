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
#pragma once

#include "JoystickTypes.h"
#include "input/Action.h"

#include <set>
#include <string>

//class TiXmlElement;
//class TiXmlNode;

namespace KODI
{
namespace JOYSTICK
{
  class CJoystickKeymap
  {
  public:
    void MapAction(int windowId, const std::string& controllerId, const std::string& feature, KODI::JOYSTICK::ANALOG_STICK_DIRECTION dir, unsigned int holdtimeMs, std::set<std::string> hotkeys, CAction action);

    void Clear();

    //unsigned int GetHoldTimeMs(int window, const CKey &key, bool fallback = true) const;

  private:
    struct KeymapEntry
    {
      CAction action;
      std::set<std::string> hotkeys;
    };

    struct OverloadedButton
    {
      using HoldTimeMS = unsigned int;
      std::map<HoldTimeMS, KeymapEntry> actions;
    };

    struct KeymapFeature
    {
      KeymapFeature(std::string feature,
        KODI::JOYSTICK::ANALOG_STICK_DIRECTION dir = KODI::JOYSTICK::ANALOG_STICK_DIRECTION::UNKNOWN) :
        feature(std::move(feature)),
        dir(dir)
      {
      }

      bool operator<(const KeymapFeature &other) const
      {
        if (feature < other.feature) return true;
        if (feature > other.feature) return false;

        return dir < other.dir;
      }

      std::string feature;
      KODI::JOYSTICK::ANALOG_STICK_DIRECTION dir = KODI::JOYSTICK::ANALOG_STICK_DIRECTION::UNKNOWN;
    };

    struct JoystickKeymap
    {
      std::string controllerId;
      std::map<KeymapFeature, OverloadedButton> keymap;
    };

    using WindowID = int;
    using WindowKeymap = std::vector<JoystickKeymap>;
    using JoystickWindowMap = std::map<WindowID, WindowKeymap>;

    JoystickWindowMap m_joystickKeymap;
  };
}
}
