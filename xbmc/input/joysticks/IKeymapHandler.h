/*
 *      Copyright (C) 2015-2017 Team Kodi
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

namespace KODI
{
namespace JOYSTICK
{
  /*!
   * \ingroup joystick
   * \brief Interface for handling keymap keys
   *
   * Keys can be mapped to analog actions (e.g. "AnalogSeekForward") or digital
   * actions (e.g. "Up").
   */
  class IKeymapHandler
  {
  public:
    virtual ~IKeymapHandler() = default;

    /*!
     * \brief Get the type of action mapped to the specified key
     *
     * \return The input type of the action
     */
    virtual INPUT_TYPE GetInputType() const = 0;

    /*!
     * \brief Get the action ID mapped to the specified key
     *
     * \return The action ID
     */
    virtual unsigned int GetActionID() const = 0;

    /*!
     * \brief A key mapped to a digital action has been pressed or released
     *
     * \param bPressed   true if the key's button/axis is activated, false if deactivated
     * \param holdTimeMs The held time in ms for pressed buttons, or 0 for released
     */
    virtual void OnDigitalKey(bool bPressed, unsigned int holdTimeMs) = 0;

    /*!
     * \brief Callback for keys mapped to analog actions
     *
     * \param magnitude  The amount of the analog action
     * \param motionTimeMs  The time since the magnitude was 0.0
     */
    virtual void OnAnalogKey(float magnitude, unsigned int motionTimeMs) = 0;
  };
}
}
