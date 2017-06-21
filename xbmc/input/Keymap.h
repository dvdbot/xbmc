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

#include "IKeymap.h"

class IKeymapEnvironment;

class CKeymap : public IKeymap
{
public:
  CKeymap(const IWindowKeymap *keymap, const IKeymapEnvironment *environment);

  // implementation of IKeymap
  virtual std::string ControllerID() const;
  virtual const IKeymapEnvironment *Environment() const override { return m_environment; }
  const KODI::JOYSTICK::KeymapActions &GetActions(const std::string& keyName) const;

private:
  // Construction parameters
  const IWindowKeymap *const m_keymap;
  const IKeymapEnvironment *const m_environment;
};
