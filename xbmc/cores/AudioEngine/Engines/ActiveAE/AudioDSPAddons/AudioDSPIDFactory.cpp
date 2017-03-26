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

#include "cores/AudioEngine//Engines/ActiveAE/AudioDSPAddons/AudioDSPIDFactory.h"

using namespace std;
using namespace DSP;


namespace ActiveAE
{
DSPErrorCode_t CAudioDSPIDFactory::RegisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
{
  if (AddonName.empty() || NodeName.empty())
  {
    return DSP_ERR_INVALID_INPUT;
  }

  string modeAddonNameStr = AddonName;
  modeAddonNameStr += "::";
  modeAddonNameStr += NodeName;

  UniqueStringIDMap_t::iterator iter = m_UniqueStringIDMap.find(modeAddonNameStr);
  if (iter != m_UniqueStringIDMap.end())
  {
    return DSP_ERR_NODE_ID_ALREADY_REGISTERED;
  }

  std::hash<std::string> hash;
  NodeID_t uniqueID;
  uniqueID.UniqueAddonID = (uint32_t)hash(AddonName);
  uniqueID.ModeID = (uint16_t)hash(NodeName);
  if (!CustomName.empty())
  {
    uniqueID.ModeInstanceID = 0x0;
  }
  else
  {
    modeAddonNameStr += "::";
    modeAddonNameStr += CustomName;
    uniqueID.ModeInstanceID = (uint16_t)hash(CustomName);
  }

  m_UniqueStringIDMap[modeAddonNameStr] = uniqueID;
  m_UniqueUint32IDMap[uniqueID] = modeAddonNameStr;

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPIDFactory::DeregisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
{
  if (AddonName.empty() || NodeName.empty())
  {
    return DSP_ERR_INVALID_INPUT;
  }

  string nodeAddonNameStr = GenerateNodeString(AddonName, NodeName, CustomName);
  UniqueStringIDMap_t::iterator iterStr = m_UniqueStringIDMap.find(nodeAddonNameStr);
  if (iterStr == m_UniqueStringIDMap.end())
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }

  UniqueUint32IDMap_t::iterator iterUint32 = m_UniqueUint32IDMap.find(iterStr->second);
  if (iterUint32 == m_UniqueUint32IDMap.end())
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }
  m_UniqueStringIDMap.erase(iterStr);
  m_UniqueUint32IDMap.erase(iterUint32);

  return DSP_ERR_NO_ERR;
}

uint64_t CAudioDSPIDFactory::GetNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
{
  if (AddonName.empty() || NodeName.empty())
  {
    return 0x0;
  }

  string nodeAddonNameStr = GenerateNodeString(AddonName, NodeName, CustomName);
  UniqueStringIDMap_t::iterator iter = m_UniqueStringIDMap.find(nodeAddonNameStr);
  if (iter == m_UniqueStringIDMap.end())
  {
    return 0x0;
  }

  return iter->second;
}
}
