#include "cores/DSP/Models/DSPNodeModel.h"

using namespace DSP;
using namespace std;


CDSPNodeModel::~CDSPNodeModel()
{
  for (std::vector<IDSPNodeCreator*>::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
  {
    if (*iter)
    {
      delete *iter;
    }
  }

  m_DSPNodeCreators.clear();
}

DSPErrorCode_t CDSPNodeModel::RegisterNode(CDSPNodeInfoQuery &Node, IDSPNodeCreator* (*NodeCreatorCB)())
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


  if (GetNode(addonName, modeName, instanceModeName) >= 0)
  {
    return DSP_ERR_NODE_ALREADY_REGISTERED;
  }

  CNodeStringInfo id(addonName, modeName, instanceModeName);
  if (id == 0x0)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  IDSPNodeCreator *creator = NodeCreatorCB();
  if (!creator)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  m_Nodes.push_back(NodeInfo_t(id, false, creator, addonName, modeName, instanceModeName));

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::DeregisterNode(uint64_t ID)
{
  int32_t idx = GetNode(ID);
  if (idx < 0)
  {
    return DSP_ERR_NODE_NOT_FOUND;
  }

  NodeInfoVector_t::iterator iter = m_Nodes.begin() + idx;
  if (iter->NodeCreator)
  {
    delete iter->NodeCreator;
    iter->NodeCreator = nullptr;
  }
  m_Nodes.erase(iter);

  return DSP_ERR_NO_ERR;
}

IDSPNodeModel::CDSPNodeInfo CDSPNodeModel::GetNodeInfo(CDSPNodeInfoQuery & Node)
{
  return CDSPNodeInfo();
}

DSPErrorCode_t CDSPNodeModel::GetNodeInfos(DSPNodeInfoVector_t &NodeInfos)
{
  NodeInfos.clear();
  for (NodeInfoVector_t::iterator iter = m_Nodes.begin(); iter != m_Nodes.end(); ++iter)
  {
    NodeInfos.push_back(CDSPNodeInfo(iter->ID, iter->ModeName, iter->Active));
  }

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::GetActiveNodes(DSPNodeInfoVector_t &ActiveNodeInfos)
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
    ActiveNodeInfos.push_back(CDSPNodeInfo(node.ID, node.ModeName, node.Active));
  }

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::EnableNode(uint64_t ID, uint32_t Position)
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
  RemoveActiveNode(ID);
  m_ActiveNodes.insert(m_ActiveNodes.begin() + Position, ID);

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CDSPNodeModel::DisableNode(uint64_t ID)
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


// factory interface
IDSPNode * CDSPNodeModel::InstantiateNode(uint64_t ID)
{
  DSPNodeCreators_t::iterator iter = getCreator(ID);
  if (iter == m_DSPNodeCreators.end())
  {
    return nullptr;
  }

  return (*iter)->InstantiateNode();
}

DSPErrorCode_t CDSPNodeModel::DestroyNode(IDSPNode *&Node)
{
  DSPNodeCreators_t::iterator iter = getCreator(Node->ID);
  if (iter == m_DSPNodeCreators.end())
  {
    return DSP_ERR_CREATOR_ID_NOT_FOUND;
  }

  return (*iter)->DestroyNode(Node);
}


// node ID factory interface
DSPErrorCode_t CDSPNodeModel::RegisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
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

DSPErrorCode_t CDSPNodeModel::DeregisterNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
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

uint64_t CDSPNodeModel::GetNodeID(const std::string &AddonName, const std::string &NodeName, const std::string &CustomName)
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
