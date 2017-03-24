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

#include "cores/DSP/Models/Interfaces/IDSPChainModel.h"

namespace DSP
{
class CDSPChainModel : public IDSPChainModel
{
  typedef struct NodeInfo_t
  {
    NodeID_t    ID;
    std::string ModeName;
    bool        Active;

    NodeInfo_t &operator=(DSPNodeInfo_t &NodeInfo)
    {
      return *this;
    }

    NodeInfo_t() :
      ModeName(""),
      Active(false)
    {
    }

    NodeInfo_t(NodeID_t ID, std::string Name, bool Active) :
      ID(ID),
      ModeName(Name),
      Active(Active)
    {
    }
  }NodeInfo_t;

  typedef std::vector<NodeInfo_t> NodeInfoVector_t;

public:
  virtual DSPErrorCode_t AddNode(const DSPNodeInfo_t &Node)
  {
    if (Node.Name.empty())
    {
      return DSP_ERR_INVALID_NODE_ID;
    }

    if (GetNode(Node.ID) >= 0)
    {
      return DSP_ERR_NODE_ALREADY_AVAILABLE;
    }

    NodeID_t id(Node.ID);
    m_Nodes.push_back(NodeInfo_t(id, Node.Name, Node.Active));
    if (Node.Active)
    {
      m_ActiveNodes.push_back(Node.ID);
    }

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t RemoveNode(uint64_t ID)
  {
    int32_t idx = GetNode(ID);
    if (idx < 0)
    {
      return DSP_ERR_NODE_NOT_FOUND;
    }

    NodeInfoVector_t::iterator iter = m_Nodes.begin() + idx;
    m_Nodes.erase(iter);

    return DSP_ERR_NO_ERR;
  }

  virtual DSPNodeInfo_t GetNodeInfo(uint64_t ID)
  {
    int32_t idx = GetNode(ID);
    if (ID < 0 || idx < 0)
    {
      return DSPNodeInfo_t();
    }

    NodeInfo_t &node = m_Nodes.at(idx);
    return DSPNodeInfo_t(node.ID, node.ModeName, node.Active);
  }

  virtual DSPErrorCode_t GetNodeInfos(DSPNodeInfoVector_t &NodeInfos)
  {
    NodeInfos.clear();
    for (NodeInfoVector_t::iterator iter = m_Nodes.begin(); iter != m_Nodes.end(); ++iter)
    {
      NodeInfos.push_back(DSPNodeInfo_t(iter->ID, iter->ModeName, iter->Active));
    }

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t GetActiveNodes(DSPNodeInfoVector_t &ActiveNodeInfos)
  {
    ActiveNodeInfos.clear();
    for (std::vector<uint64_t>::iterator iter = m_ActiveNodes.begin(); iter != m_ActiveNodes.end(); ++iter)
    {
      int32_t idx = GetNode(*iter);
      if (idx < 0)
      {
        return DSP_ERR_FATAL_ERROR;
      }
      
      NodeInfo_t &node = m_Nodes.at(idx);
      ActiveNodeInfos.push_back(DSPNodeInfo_t(node.ID, node.ModeName, node.Active));
    }

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t EnableNode(uint64_t ID)
  {
    int32_t idx = GetNode(ID);
    if (idx < 0)
    {
      return DSP_ERR_NODE_NOT_FOUND;
    }

    if (!m_Nodes.at(idx).Active)
    {
      m_Nodes.at(idx).Active = true;
      m_ActiveNodes.push_back(ID);
    }

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t SetNodePosition(uint64_t ID, uint32_t Position)
  {
    int32_t idx = GetNode(ID);
    if (idx < 0)
    {
      return DSP_ERR_NODE_NOT_FOUND;
    }

    RemoveActiveNode(ID);
    m_ActiveNodes.insert(m_ActiveNodes.begin() + Position, ID);

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t DisableNode(uint64_t ID)
  {
    int32_t idx = GetNode(ID);
    if (idx < 0)
    {
      return DSP_ERR_NODE_NOT_FOUND;
    }

    m_Nodes.at(idx).Active = false;
    RemoveActiveNode(ID);

    return DSP_ERR_NO_ERR;
  }

private:
  int32_t GetNode(uint64_t ID)
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

  void RemoveActiveNode(uint64_t ID)
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
};
}
