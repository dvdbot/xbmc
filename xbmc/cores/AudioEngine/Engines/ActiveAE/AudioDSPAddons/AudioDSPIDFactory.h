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


#include "cores/DSP/Factory/Interfaces/IDSPIDFactory.h"


namespace ActiveAE
{
class CAudioDSPIDFactory : public DSP::IDSPIDFactory
{
  typedef std::map<std::string, uint64_t> UniqueStringIDMap_t;
  typedef std::map<uint64_t, std::string> UniqueUint32IDMap_t;

public:
  // invalid ID == 0x0! 
  virtual DSPErrorCode_t RegisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");
  virtual DSPErrorCode_t DeregisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");
  virtual uint64_t GetNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");

private:
  inline std::string GenerateNodeString(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
  {
    std::string nodeAddonNameStr = AddonName;
    nodeAddonNameStr += "::";
    nodeAddonNameStr += NodeName;

    if (!CustomName.empty())
    {
      nodeAddonNameStr += "::";
      nodeAddonNameStr += CustomName;
    }

    return nodeAddonNameStr;
  }

  UniqueStringIDMap_t  m_UniqueStringIDMap;
  UniqueUint32IDMap_t  m_UniqueUint32IDMap;
};
}
