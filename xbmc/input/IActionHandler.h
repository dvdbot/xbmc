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

class CAction;

/*!
 * \ingroup joystick
 * \brief Interface for handling Kodi actions
 */
class IActionHandler
{
public:
  virtual ~IActionHandler() = default;

  /*!
   * \brief Send a digital or analog action
   * \param action The action to send
   * \return True if the action was handled, false otherwise
   */
  virtual bool SendAction(const CAction& action) = 0;
};
