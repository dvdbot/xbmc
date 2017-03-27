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

#include "cores/DSP/Models/Interfaces/IDSPNodeModel.h"
#include "cores/DSP/Factory/Interfaces/IDSPNodeFactory.h"
#include "cores/DSP/Factory/Interfaces/IDSPIDFactory.h"

#include <vector>
#include <string>


namespace DSP
{
class CDSPNodeModel : public IDSPNodeModel, public DSP::IDSPNodeFactory, public DSP::IDSPIDFactory
{
  // node model interface
  typedef struct NodeInfo_t
  {
    std::string AddonName;
    std::string ModeName;
    std::string InstanceModeName;
    NodeID_t    ID;
    bool        Active;
    const DSP::IDSPNodeCreator *NodeCreator;

    NodeInfo_t &operator=(CDSPNodeInfo &NodeInfo)
    {
      return *this;
    }

    NodeInfo_t() :
      ID(0),
      Active(false),
      NodeCreator(nullptr),
      AddonName(""),
      ModeName(""),
      InstanceModeName("")
    {
    }

    NodeInfo_t(NodeID_t ID, bool Active, DSP::IDSPNodeCreator *Creator, std::string AddonName, std::string ModeName, std::string InstanceModeName = "") :
      ID(ID),
      Active(Active),
      NodeCreator(nullptr),
      AddonName(AddonName),
      ModeName(ModeName),
      InstanceModeName(InstanceModeName)
    {
    }
  }NodeInfo_t;

  class CNodeStringInfo : public NodeID_t
  {
  public:
    CNodeStringInfo(const std::string &AddonName, const std::string &ModeName, const std::string &InstanceModeName)
    {
      std::hash<std::string> hashObj;

      NodeID_t::UniqueAddonID = (uint32_t)hashObj(AddonName);
      NodeID_t::ModeID = (uint16_t)hashObj(ModeName);
      NodeID_t::ModeInstanceID = (uint16_t)hashObj(InstanceModeName);
    }
  };

  typedef std::vector<NodeInfo_t> NodeInfoVector_t;

  // node factory interface
  typedef std::vector<IDSPNodeCreator*> DSPNodeCreators_t;
  
  // node ID factory interface
  typedef std::map<std::string, uint64_t> UniqueStringIDMap_t;
  typedef std::map<uint64_t, std::string> UniqueUint32IDMap_t;

public:
  virtual ~CDSPNodeModel();

  // model interface
  virtual DSPErrorCode_t RegisterNode(CDSPNodeInfoQuery &Node, IDSPNodeCreator* (*NodeCreatorCB)()) override;
  virtual DSPErrorCode_t DeregisterNode(uint64_t ID) override;
  virtual CDSPNodeInfo GetNodeInfo(CDSPNodeInfoQuery &Node) override;
  virtual DSPErrorCode_t GetNodeInfos(DSPNodeInfoVector_t &NodeInfos) override;
  virtual DSPErrorCode_t GetActiveNodes(DSPNodeInfoVector_t &ActiveNodeInfos) override;
  virtual DSPErrorCode_t EnableNode(uint64_t ID, uint32_t Position = 0) override;
  virtual DSPErrorCode_t DisableNode(uint64_t ID) override;

  // factory interface
  virtual IDSPNode* InstantiateNode(uint64_t ID);
  virtual DSPErrorCode_t DestroyNode(IDSPNode *&Node);

  // node ID factory interface
  // invalid ID == 0x0! 
  virtual DSPErrorCode_t RegisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");
  virtual DSPErrorCode_t DeregisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");
  virtual uint64_t GetNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName = "");
  
private:
  // node model interface
  inline std::string GenerateNodeString(const std::vector<std::string> &NameVectorString)
  {
  }

  inline int32_t GetNode(uint64_t ID)
  {
    for (int32_t ii = 0; ii < (int32_t)m_Nodes.size(); ii++)
    {
      if (ID == m_Nodes.at(ii).ID)
      {
        return ii;
      }
    }

    return -1;
  }

  inline int32_t GetNode(const std::string &AddonName, const std::string &ModeName, const std::string &InstanceModeName)
  {
    for (int32_t ii = 0; ii < (int32_t)m_Nodes.size(); ii++)
    {
      if (AddonName == m_Nodes.at(ii).AddonName && ModeName == m_Nodes.at(ii).ModeName && InstanceModeName == m_Nodes.at(ii).InstanceModeName)
      {
        return ii;
      }
    }

    return -1;
  }

  inline void RemoveActiveNode(uint64_t ID)
  {
    for (std::vector<uint64_t>::iterator iter = m_ActiveNodes.begin(); iter != m_ActiveNodes.end(); ++iter)
    {
      if (ID == *iter)
      {
        m_ActiveNodes.erase(iter);
        return;
      }
    }
  }

  NodeInfoVector_t m_Nodes;
  std::vector<uint64_t> m_ActiveNodes;

  // node factory interface
  inline DSPNodeCreators_t::iterator getCreator(uint64_t ID)
  {
    for (DSPNodeCreators_t::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
    {
      if ((*iter)->GetID() == ID)
      {
        return iter;
      }
    }

    return m_DSPNodeCreators.end();
  }

  inline DSPNodeCreators_t::iterator getCreator(IDSPNodeCreator *Creator)
  {
    for (DSPNodeCreators_t::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
    {
      if (*iter == Creator)
      {
        return iter;
      }
    }

    return m_DSPNodeCreators.end();
  }

  std::vector<IDSPNodeCreator*> m_DSPNodeCreators;

  // node ID factory interface
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
