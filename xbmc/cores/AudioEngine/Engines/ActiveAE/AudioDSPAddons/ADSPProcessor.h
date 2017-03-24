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

#include "DSPChain/Processors/Interfaces/IADSPProcessor.h"
#include "DSPChain/Factory/Interfaces/IDSPNodeFactory.h"
#include "DSPChain/Models/Interfaces/IDSPChainModel.h"

namespace DSPChain
{
class CADSPProcessor : public IADSPProcessor
{
  typedef std::list<IADSPChainNode*> ADSPChain_t;
public:
  CADSPProcessor(IDSPNodeFactory *NodeFactory, IDSPChainModel *NodeModel) :
    IADSPProcessor("CADSPProcessor", ADSP_DataFormatFloat),
    m_NodeFactory(NodeFactory), 
    m_NodeModel(NodeModel),
    m_NeedsNodesUpdate(false)
  {
  }

  virtual DSPErrorCode_t UpdateNodes() override
  {
    m_NeedsNodesUpdate = true;
    return DSP_ERR_NO_ERR;
  }

protected:
  virtual DSPErrorCode_t AddADSPNode(IADSPChainNode *ADSPNode) override
  {
    return DSP_ERR_NO_ERR;
  }
  
  virtual DSPErrorCode_t RemoveADSPNode(IADSPChainNode *ADSPNode) override
  {
    return DSP_ERR_NO_ERR;
  }

  //! @todo think about the create interface
  virtual DSPErrorCode_t CreateInstance(const void *InParameters, void *OutParameters, void *Options = nullptr) override
  {
    //m_NodeFactory.InstantiateNode(0);
    IDSPChainModel::DSPNodeInfoVector_t activeNodes;
    m_NodeModel->GetActiveNodes(activeNodes);

    for (int ii = 0; ii < activeNodes.size(); ii++)
    {
      IDSPChainNode *node = (m_NodeFactory->InstantiateNode(activeNodes.at(ii).ID));
      if (!node)
      {
        return DSP_ERR_FATAL_ERROR;
      }
      DSPErrorCode_t dspErr = node->Create(InParameters, OutParameters);
      if (dspErr != DSP_ERR_NO_ERR)
      {
        m_NodeFactory->DestroyNode(node);
        return dspErr;
      }

      IADSPChainNode *adspNode = dynamic_cast<IADSPChainNode*>(node);
      if (!adspNode)
      {
        m_NodeFactory->DestroyNode(node);
        return DSP_ERR_FATAL_ERROR;
      }

      m_ActiveDSPChain.push_back(adspNode);
    }

    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t ProcessInstance(float  *In, float   *Out) override
  {
    DSPErrorCode_t dspErr = ReCreateNodeChain();
    if (dspErr != DSP_ERR_NO_ERR)
    {
      return dspErr;
    }

    float *in = In;
    float *out = Out;
    for (ADSPChain_t::iterator iter = m_ActiveDSPChain.begin(); iter != m_ActiveDSPChain.end(); ++iter)
    {
      dspErr = (*iter)->Process(in, out);
      if (dspErr != DSP_ERR_NO_ERR)
      {
        return dspErr;
      }

      float *tmp = in;
      in = out;
      out = tmp;
    }

    return DSP_ERR_NO_ERR;
  }

  //! @todo think about the destroy interface
  virtual DSPErrorCode_t DestroyInstance() override
  {
    for (ADSPChain_t::iterator iter = m_ActiveDSPChain.begin(); iter != m_ActiveDSPChain.end(); ++iter)
    {
      if (*iter)
      {
        (*iter)->Destroy();
        delete *iter;
      }
    }

    m_ActiveDSPChain.clear();

    return DSP_ERR_NO_ERR;
  }

private: // private methods
  DSPErrorCode_t ReCreateNodeChain()
  {
    if (!m_NeedsNodesUpdate)
    {
      return DSP_ERR_NO_ERR;
    }

    ADSPChain_t tempNodeList;
    ADSPChain_t destroyNodeList;
    IDSPChainModel::DSPNodeInfoVector_t activeNodes;
    m_NodeModel->GetActiveNodes(activeNodes);

    for (uint32_t ii = 0; ii < activeNodes.size(); ii++)
    {
      IADSPChainNode *node = nullptr;
      for (ADSPChain_t::iterator iter = m_ActiveDSPChain.begin(); iter != m_ActiveDSPChain.end(); ++iter)
      {// search node
        if ((*iter)->ID == activeNodes.at(ii).ID)
        {
          node = *iter;
          break;
        }
      }

      if (activeNodes.at(ii).Active && !node)
      {// create and add new node
        node = dynamic_cast<IADSPChainNode*>(m_NodeFactory->InstantiateNode(activeNodes.at(ii).ID));
        if (node)
        {
          // todo save parameters into common memory
          void *InParameters = nullptr;
          void *OutParameters = nullptr;
          void *Options = nullptr;
          // todo save parameters into common memory
          node->Create(InParameters, OutParameters, Options);
        }
      }

      tempNodeList.push_back(node);
    }

    if (m_ActiveDSPChain.size() != tempNodeList.size())
    {// disable nodes
      for (ADSPChain_t::iterator activeIter = m_ActiveDSPChain.begin(); activeIter != m_ActiveDSPChain.end(); ++activeIter)
      {
        bool found = false;
        for (ADSPChain_t::iterator tmpIter = tempNodeList.begin(); tmpIter != tempNodeList.end(); ++tmpIter)
        {// search node
          if ((*activeIter)->ID == (*tmpIter)->ID)
          {
            found = true;
            break;
          }
        }

        if (!found)
        {
          destroyNodeList.push_back(*activeIter);
        }
      }
    }

    //! @todo move this to an other thread
    DSPErrorCode_t dspErr = DSP_ERR_NO_ERR;
    if (destroyNodeList.size() > 0)
    {
      for (ADSPChain_t::iterator tmpIter = destroyNodeList.begin(); tmpIter != destroyNodeList.end(); ++tmpIter)
      {
        IDSPChainNode *node = dynamic_cast<IDSPChainNode*>(*tmpIter);
        DSPErrorCode_t locErr = m_NodeFactory->DestroyNode(node);
        if (locErr != DSP_ERR_NO_ERR)
        {
          dspErr = locErr;
        }
      }
    }

    // set new DSP chain
    m_ActiveDSPChain = tempNodeList;
    m_NeedsNodesUpdate = false;

    return dspErr;
  }


private: // private member variables
  CADSPProcessor() : IADSPProcessor("CADSPProcessor", ADSP_DataFormatINVALID), m_NodeFactory(nullptr) {}
  /*const*/ IDSPNodeFactory *m_NodeFactory;
  /*const*/ IDSPChainModel *m_NodeModel;
  ADSPChain_t m_ActiveDSPChain;

  bool m_NeedsNodesUpdate;
};
}
