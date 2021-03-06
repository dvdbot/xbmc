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

#include <map>
#include <string>

class CAction;
class IActionHandler;
class IKeymap;

namespace KODI
{
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
    CKeyHandler(const std::string &keyName, IActionHandler *actionHandler, const IKeymap *keymap, IKeymapHandler *keymapHandler);

    virtual ~CKeyHandler() = default;

    // implementation of IKeyHandler
    virtual bool OnDigitalMotion(bool bPressed, unsigned int holdTimeMs) override;
    virtual bool OnAnalogMotion(float magnitude, unsigned int motionTimeMs) override;

  private:
    void SendDigitalAction(const CAction& action);
    void SendAnalogAction(const CAction& action);

    // Construction parameters
    const std::string m_keyName;
    IActionHandler *const m_actionHandler;
    const IKeymap *const m_keymap;
    IKeymapHandler *const m_keymapHandler;

    // Digital variables
    bool m_bPressed = false;
    unsigned int m_lastDigitalActionMs = 0;

    // Analog variables
    bool m_bHeld = false;
    unsigned int m_holdStartTime = 0;
  };
}
}
