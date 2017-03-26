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

using namespace std;


namespace DSP
{
uint64_t CDSPIDFactory::GetNodeID(const std::string &AddonName, const std::string &ModeName, const std::string &CustomName)
{
  if (AddonName.empty() || ModeName.empty())
  {
    return 0x0;
  }

  string modeAddonNameStr = AddonName;
  modeAddonNameStr += "::";
  modeAddonNameStr += ModeName;

  if (!CustomName.empty())
  {
    modeAddonNameStr += "::";
    modeAddonNameStr += CustomName;
  }

  UniqueStringIDMap_t::iterator iter = m_UniqueStringIDMap.find(modeAddonNameStr);
  if (iter == m_UniqueStringIDMap.end())
  {
    std::hash<std::string> hash;
    NodeID_t uniqueID;
    uniqueID.UniqueAddonID = (uint32_t)hash(AddonName);
    uniqueID.ModeID = (uint16_t)hash(ModeName);
    if (!CustomName.empty())
    {
      uniqueID.ModeInstanceID = 0x0;
    }
    else
    {
      uniqueID.ModeInstanceID = (uint16_t)hash(CustomName);
    }

    m_UniqueStringIDMap[modeAddonNameStr] = uniqueID;
    m_UniqueUint32IDMap[uniqueID] = modeAddonNameStr;
  }

  return m_UniqueStringIDMap[modeAddonNameStr];
}
}
