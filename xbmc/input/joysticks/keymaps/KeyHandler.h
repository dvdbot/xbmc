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

#include "input/joysticks/interfaces/IKeyHandler.h"
#include "input/joysticks/JoystickTypes.h"

#include <map>
#include <string>

class CAction;
class IKeymap;

namespace KODI
{
namespace ACTIONS
{
  class IActionHandler;
}

namespace JOYSTICK
{
  class IKeymapHandler;

  /*!
   * \ingroup joystick
   * \brief
   */
  class CKeyHandler : public IKeyHandler
  {
  public:
    CKeyHandler(const std::string &keyName, ACTIONS::IActionHandler *actionHandler, const IKeymap *keymap, IKeymapHandler *keymapHandler);

    virtual ~CKeyHandler() = default;

    // implementation of IKeyHandler
    virtual bool IsPressed() const override { return m_bHeld; }
    virtual bool OnDigitalMotion(bool bPressed, unsigned int holdTimeMs) override;
    virtual bool OnAnalogMotion(float magnitude, unsigned int motionTimeMs) override;

  private:
    void Reset();

    bool SendActions(const KeymapActions& actions, float magnitude, unsigned int holdTimeMs);
    bool SendAction(const KeymapAction& action, float magnitude, unsigned int holdTimeMs);

    // Check criteria for sending a repeat action
    bool SendRepeatAction(unsigned int holdTimeMs);

    bool HotkeysPressed(const std::set<std::string> &hotkeys) const;

    // Construction parameters
    const std::string m_keyName;
    ACTIONS::IActionHandler *const m_actionHandler;
    const IKeymap *const m_keymap;
    IKeymapHandler *const m_keymapHandler;

    // State variables
    bool m_bHeld;
    unsigned int m_holdStartTimeMs;
    unsigned int m_lastHoldTimeMs;
    bool m_bActionSent;
    unsigned int m_lastActionMs;
  };
}
}