#pragma once
/*
 *      Copyright (C) 2005-2017 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <stdint.h>
#include <map>

#include "cores/DSP/Typedefs/DSPTypedefs.h"


namespace DSP
{
//! @todo rework this class with uint64_t!
class CDSPIDFactory
{
  typedef std::map<std::string, uint32_t> UniqueStringIDMap_t;
  typedef std::map<uint32_t, std::string> UniqueUint32IDMap_t;
  typedef std::map<uint16_t, int32_t>     InstanceIDMap_t;
public:
  static int32_t GetUniqueDSPNodeID(const std::string &Name, const DSPCategory_t &Category);
  static uint16_t GetInstanceDSPNodeID(const uint32_t UniqueID);

private:
  static UniqueStringIDMap_t  m_UniqueStringIDMap;
  static UniqueUint32IDMap_t  m_UniqueUint32IDMap;
  static InstanceIDMap_t      m_InstanceIDMap;
};
}
