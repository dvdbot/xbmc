#include "cores/DSP/Models/DSPNodeModel.h"

using namespace DSP;
using namespace std;


CDSPNodeModel::~CDSPNodeModel()
{
  for (NodeInfoVector_t::iterator iter = m_Nodes.begin(); iter != m_Nodes.end(); ++iter)
  {
    if (iter->NodeCreator)
    {
      delete iter->NodeCreator;
      iter->NodeCreator = nullptr;
    }
  }

  m_Nodes.clear();
}

DSPErrorCode_t CDSPNodeModel::RegisterNode(CDSPNodeInfoQuery &Node, IDSPNodeCreator* (*NodeCreatorFactory)())
{
  if (Node.NameVector.size() < 2)
  {
    return DSP_ERR_INVALID_NODE_ID;
  }

  string addonName = Node.NameVector.at(0);
  string modeName = Node.NameVector.at(1);
  string instanceModeName = "";
  if (Node.NameVector.size() > 2)
  {
    instanceModeName = Node.NameVector.at(2);
  }


  if (GetNodeData(addonName, modeName, instanceModeName) != m_Nodes.end())
  {
    return DSP_ERR_NODE_ALREADY_REGISTERED;
  }

  CNodeStringInfo id(addonName, modeName, instanceModeName);
  if (id == 0x0)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  IDSPNodeCreator *creator = NodeCreatorFactory();
  if (!creator)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  m_Nodes.push_back(NodeInfo_t(id, false, creator, addonName, modeName, instanceModeName));

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::DeregisterNode(uint64_t ID)
{
  NodeInfoVector_t::iterator iter = GetNodeData(ID);
  if (iter != m_Nodes.end())
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }

  if (iter->NodeCreator)
  {
    delete iter->NodeCreator;
    iter->NodeCreator = nullptr;
  }
  m_Nodes.erase(iter);

  return DSP_ERR_NO_ERR;
}

IDSPNodeModel::CDSPNodeInfo CDSPNodeModel::GetNodeInfo(CDSPNodeInfoQuery &Node)
{
  string nodeStr = GenerateNodeString(Node.NameVector);
  if (Node.NameVector.size() < 2 || Node.NameVector.size() > 3)
  {
    return CDSPNodeInfo();
  }

  string addonName = Node.NameVector.at(0);
  string modeName = Node.NameVector.at(1);
  string instanceModeName = Node.NameVector.size() == 3 ? instanceModeName = Node.NameVector.at(2) : "";
  NodeID_t id = 0x0;
  bool isActive = false;

  for (int ii = 0; ii < m_Nodes.size(); ii++)
  {
    if (addonName == m_Nodes.at(ii).AddonName && modeName == m_Nodes.at(ii).ModeName && instanceModeName == m_Nodes.at(ii).InstanceModeName)
    {
      nodeStr = GenerateNodeString(addonName, modeName, instanceModeName);
      id = m_Nodes.at(ii).ID;
      isActive = m_Nodes.at(ii).Active;
    }
  }

  if (nodeStr.empty() || id == 0x0)
  {
    return CDSPNodeInfo();
  }

  return CDSPNodeInfo(nodeStr, id, isActive);
}

DSPErrorCode_t CDSPNodeModel::GetNodeInfos(DSPNodeInfoVector_t &NodeInfos)
{
  NodeInfos.clear();
  string nodeStr;
  for (NodeInfoVector_t::iterator iter = m_Nodes.begin(); iter != m_Nodes.end(); ++iter)
  {
    nodeStr = GenerateNodeString(iter->AddonName, iter->ModeName, iter->InstanceModeName);
    NodeInfos.push_back(CDSPNodeInfo(nodeStr, iter->ID, iter->Active));
  }

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::GetActiveNodes(DSPNodeInfoVector_t &ActiveNodeInfos)
{
  ActiveNodeInfos.clear();
  string nodeStr;
  for (std::vector<uint64_t>::iterator iter = m_ActiveNodes.begin(); iter != m_ActiveNodes.end(); ++iter)
  {
    NodeInfoVector_t::iterator nodeIter = GetNodeData(*iter);
    if (nodeIter == m_Nodes.end())
    {
      return DSP_ERR_FATAL_ERROR;
    }

    nodeStr = nodeIter->AddonName;
    nodeStr += "::";
    nodeStr += nodeIter->ModeName;
    if (!nodeIter->InstanceModeName.empty())
    {
      nodeStr += "::";
      nodeStr += nodeIter->InstanceModeName;
    }

    NodeInfo_t &node = m_Nodes.at(nodeIter->ID);
    ActiveNodeInfos.push_back(CDSPNodeInfo(nodeStr, node.ID, node.Active));
  }

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::EnableNode(uint64_t ID, uint32_t Position)
{
  NodeInfoVector_t::iterator nodeIter = GetNodeData(ID);
  if (nodeIter == m_Nodes.end())
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }

  if (!nodeIter->Active)
  {
    nodeIter->Active = true;
    m_ActiveNodes.push_back(ID);
  }
  RemoveActiveNode(ID);
  m_ActiveNodes.insert(m_ActiveNodes.begin() + Position, ID);

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::DisableNode(uint64_t ID)
{
  NodeInfoVector_t::iterator nodeIter = GetNodeData(ID);
  if (nodeIter == m_Nodes.end())
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }

  nodeIter->Active = false;
  RemoveActiveNode(ID);

  return DSP_ERR_NO_ERR;
}


// factory interface
IDSPNode* CDSPNodeModel::InstantiateNode(uint64_t ID)
{
  NodeInfoVector_t::iterator iter = GetNodeData(ID);
  if (iter == m_Nodes.end())
  {
    return nullptr;
  }

  return iter->NodeCreator->InstantiateNode(ID);
}

DSPErrorCode_t CDSPNodeModel::DestroyNode(IDSPNode *&Node)
{
  NodeInfoVector_t::iterator iter = GetNodeData(Node->ID);
  if (iter == m_Nodes.end())
  {
    return DSP_ERR_CREATOR_ID_NOT_FOUND;
  }

  return iter->NodeCreator->DestroyNode(Node);
}
