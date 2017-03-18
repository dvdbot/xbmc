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
#include <vector>

#include "DSPChain/Nodes/Interfaces/IDSPChainNode.h"

namespace DSP
{
class IDSPChainModel
{
public:
  typedef struct DSPNodeInfo_t
  {
    const uint64_t    ID;
    const std::string Name;
    const bool        Active;

    DSPNodeInfo_t &operator=(DSPNodeInfo_t &NodeInfo)
    {
      return *this;
    }

    DSPNodeInfo_t() :
      ID(0),
      Name(""),
      Active(false)
    {
    }

    DSPNodeInfo_t(uint64_t ID, std::string Name, bool Active) :
      ID(ID),
      Name(Name),
      Active(Active)
    {
    }
  }DSPNodeInfo_t;

  typedef std::vector<DSPNodeInfo_t> DSPNodeInfoVector_t;


  virtual DSPErrorCode_t AddNode(const DSPNodeInfo_t &Node) = 0;
  virtual DSPErrorCode_t RemoveNode(uint64_t ID) = 0;
  
  virtual DSPNodeInfo_t  GetNodeInfo(uint64_t ID) = 0;
  virtual DSPErrorCode_t GetNodeInfos(DSPNodeInfoVector_t &NodeInfos) = 0;
  virtual DSPErrorCode_t GetActiveNodes(DSPNodeInfoVector_t &ActiveNodeInfos) = 0;
  
  virtual DSPErrorCode_t EnableNode(uint64_t ID) = 0;
  virtual DSPErrorCode_t SetNodePosition(uint64_t ID, uint32_t Position) = 0;
  virtual DSPErrorCode_t DisableNode(uint64_t ID) = 0;
};
}
