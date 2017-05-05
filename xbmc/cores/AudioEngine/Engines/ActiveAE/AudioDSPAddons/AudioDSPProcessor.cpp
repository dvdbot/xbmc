/*
 *     Copyright (C) 2005-2017 Team Kodi
 *     http://kodi.tv
 *
 * This Program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This Program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Kodi; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 */

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPProcessor.h"
#include "cores/DSP/Models/DSPNodeModel.h"

using namespace ActiveAE;
using namespace DSP;
using namespace DSP::AUDIO;

CAudioDSPProcessor::CAudioDSPProcessor(const CAudioDSPController &Controller, IDSPNodeFactory &NodeFactory) :
  IADSPProcessor("CAudioDSPProcessor"),
  m_AudioDSPController(Controller),
  m_NodeFactory(NodeFactory)
{
}

CAudioDSPProcessor::~CAudioDSPProcessor()
{
}

// private methods

DSPErrorCode_t CAudioDSPProcessor::ReCreateNodeChain()
{
  //if (!m_NeedsNodesUpdate)
  //{
  //  return DSP_ERR_NO_ERR;
  //}

  //ADSPChain_t tempNodeList;
  //ADSPChain_t destroyNodeList;
  //IDSPNodeModel::DSPNodeInfoVector_t activeNodes;
  //m_NodeModel->GetActiveNodes(activeNodes);

  //for (uint32_t ii = 0; ii < activeNodes.size(); ii++)
  //{
  //  IADSPChainNode *node = nullptr;
  //  for (ADSPChain_t::iterator iter = m_ActiveDSPChain.begin(); iter != m_ActiveDSPChain.end(); ++iter)
  //  {// search node
  //    if ((*iter)->ID == activeNodes.at(ii).ID)
  //    {
  //      node = *iter;
  //      break;
  //    }
  //  }

  //  if (activeNodes.at(ii).Active && !node)
  //  {// create and add new node
  //    node = dynamic_cast<IADSPChainNode*>(m_NodeFactory->InstantiateNode(activeNodes.at(ii).ID));
  //    if (node)
  //    {
  //      // todo save parameters into common memory
  //      void *InParameters = nullptr;
  //      void *OutParameters = nullptr;
  //      void *Options = nullptr;
  //      // todo save parameters into common memory
  //      node->Create(InParameters, OutParameters, Options);
  //    }
  //  }

  //  tempNodeList.push_back(node);
  //}

  //if (m_ActiveDSPChain.size() != tempNodeList.size())
  //{// disable nodes
  //  for (ADSPChain_t::iterator activeIter = m_ActiveDSPChain.begin(); activeIter != m_ActiveDSPChain.end(); ++activeIter)
  //  {
  //    bool found = false;
  //    for (ADSPChain_t::iterator tmpIter = tempNodeList.begin(); tmpIter != tempNodeList.end(); ++tmpIter)
  //    {// search node
  //      if ((*activeIter)->ID == (*tmpIter)->ID)
  //      {
  //        found = true;
  //        break;
  //      }
  //    }

  //    if (!found)
  //    {
  //      destroyNodeList.push_back(*activeIter);
  //    }
  //  }
  //}

  ////! @todo move this to an other thread
  //DSPErrorCode_t dspErr = DSP_ERR_NO_ERR;
  //if (destroyNodeList.size() > 0)
  //{
  //  for (ADSPChain_t::iterator tmpIter = destroyNodeList.begin(); tmpIter != destroyNodeList.end(); ++tmpIter)
  //  {
  //    IDSPNode *node = dynamic_cast<IDSPNode*>(*tmpIter);
  //    DSPErrorCode_t locErr = m_NodeFactory->DestroyNode(node);
  //    if (locErr != DSP_ERR_NO_ERR)
  //    {
  //      dspErr = locErr;
  //    }
  //  }
  //}

  //// set new DSP chain
  //m_ActiveDSPChain = tempNodeList;
  //m_NeedsNodesUpdate = false;

  //return dspErr;
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPProcessor::Create(const AEAudioFormat *InFormat, AEAudioFormat *OutFormat)
{
  IDSPNodeModel::DSPNodeInfoVector_t nodeInfos;
  DSPErrorCode_t dspErr = m_AudioDSPController.GetNodeInfos(nodeInfos);
  if (dspErr != DSP_ERR_NO_ERR)
  {
    return dspErr;
  }

  m_InFormat = *InFormat;
  m_OutFormat = *OutFormat;
  AEAudioFormat tmpParameters[2];
  AEAudioFormat *configInParameters = &tmpParameters[0];
  AEAudioFormat *configOutParameters = &tmpParameters[1];

  *configInParameters = m_InFormat;
  *configOutParameters = m_InFormat;

  // create node chain
  for(uint32_t ii = 0; ii < nodeInfos.size(); ii++)
  {
    IDSPNode *node = m_NodeFactory.InstantiateNode(nodeInfos.at(ii).ID);
    if (!node)
    {
      return DSP_ERR_FATAL_ERROR;
    }
    DSPErrorCode_t dspErr = node->Create(configInParameters, configOutParameters);
    if (dspErr != DSP_ERR_NO_ERR)
    {
      m_NodeFactory.DestroyNode(node);
      return dspErr;
    }

    IADSPNode *adspNode = dynamic_cast<IADSPNode*>(node);
    if (!adspNode)
    {
      m_NodeFactory.DestroyNode(node);
      return DSP_ERR_FATAL_ERROR;
    }

    // swap pointer for parameters
    AEAudioFormat *p = configInParameters;
    configInParameters = configOutParameters;
    configOutParameters = p;

    m_DSPNodeChain.push_back(adspNode);
  }

  // configure buffers
  //! @todo implement buffer configuration
  if (m_DSPNodeChain.size() == 0)
  {
    if (!(m_InFormat == m_OutFormat))
    { // create a output conversion buffer
      //! @todo add buffer
    }
  }
  else if (m_DSPNodeChain.size() == 1)
  {
    AEAudioFormat inFmt = m_DSPNodeChain.at(0)->GetInputFormat();
    if (!(inFmt == m_InFormat))
    { // create a output conversion buffer
      //! @todo add buffer
    }

    m_OutFormat = m_DSPNodeChain.at(0)->GetOutputFormat();
  }
  else
  {
    AEAudioFormat inFmt = m_InFormat;
    for (uint32_t ii = 0; ii < m_DSPNodeChain.size(); ii++)
    {
      AEAudioFormat outFmt;
      outFmt = m_DSPNodeChain.at(ii)->GetInputFormat();

      if (!(inFmt == outFmt))
      { // create conversion buffer
        //! @todo add buffer
      }

      inFmt = m_DSPNodeChain.at(ii)->GetOutputFormat();
    }

    m_OutFormat = inFmt;
  }

  *OutFormat = m_OutFormat;
    
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPProcessor::Process(const CSampleBuffer *In, CSampleBuffer *Out)
{
  if(!In || !Out)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  int out_samples = 0;
  Out->pkt->nb_samples += In->pkt->nb_samples;

  return DSP_ERR_NO_ERR;
}

//DSPErrorCode_t CAudioDSPProcessor::ProcessInstance(float *In, float *Out)
//{
//  DSPErrorCode_t dspErr = ReCreateNodeChain();
//  if (dspErr != DSP_ERR_NO_ERR)
//  {
//    return dspErr;
//  }

//  float *in = In;
//  float *out = Out;
//  for (AudioDSPNodeChain_t::iterator iter = m_DSPNodeChain.begin(); iter != m_DSPNodeChain.end(); ++iter)
//  {
//    dspErr = (*iter)->Process(in, out);
//    if (dspErr != DSP_ERR_NO_ERR)
//    {
//      return dspErr;
//    }

//    float *tmp = in;
//    in = out;
//    out = tmp;
//  }
//}

DSPErrorCode_t CAudioDSPProcessor::Destroy()
{
  for (AudioDSPNodeChain_t::iterator iter = m_DSPNodeChain.begin(); iter != m_DSPNodeChain.end(); ++iter)
  {
    if (*iter)
    {
      IDSPNode *node = *iter;
      
      DSPErrorCode_t dspErr = m_NodeFactory.DestroyNode(node);
      if (dspErr != DSP_ERR_NO_ERR)
      { //! @todo handle error code
      }
    }
  }

  m_DSPNodeChain.clear();

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPProcessor::EnableNodeCallback(uint64_t ID, uint32_t Position)
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPProcessor::DisableNodeCallback(uint64_t ID)
{
  return DSP_ERR_NO_ERR;
}
