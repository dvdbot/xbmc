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

#include "cores/DSP/Utils/DSPIDFactory.h"

namespace DSP
{
CDSPIDFactory::UniqueStringIDMap_t  CDSPIDFactory::m_UniqueStringIDMap;
CDSPIDFactory::UniqueUint32IDMap_t  CDSPIDFactory::m_UniqueUint32IDMap;
CDSPIDFactory::InstanceIDMap_t      CDSPIDFactory::m_InstanceIDMap;

int32_t DSP::CDSPIDFactory::GetUniqueDSPNodeID(const std::string & Name, const DSPCategory_t & Category)
{
  UniqueStringIDMap_t::iterator iter = m_UniqueStringIDMap.find(Name);
  if (iter == m_UniqueStringIDMap.end())
  {
    std::hash<std::string> hash;
    uint32_t uniqueID = (uint32_t)hash(Name);

    m_UniqueStringIDMap[Name]     = uniqueID;
    m_UniqueUint32IDMap[uniqueID] = Name;
    m_InstanceIDMap[uniqueID]     = 0;
  }

  return m_UniqueStringIDMap[Name];
}

uint16_t DSP::CDSPIDFactory::GetInstanceDSPNodeID(const uint32_t UniqueID)
{
  UniqueUint32IDMap_t::iterator iter = m_UniqueUint32IDMap.find(UniqueID);
  if (iter == m_UniqueUint32IDMap.end())
  {
    return 0;
  }

  return (m_InstanceIDMap[UniqueID]++) + 1;
}
}
