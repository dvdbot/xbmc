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
  IADSPProcessor("CAudioDSPProcessor", ADSP_DataFormatFloat),
  m_AudioDSPController(Controller),
  m_NodeFactory(NodeFactory)
{
}

CAudioDSPProcessor::~CAudioDSPProcessor()
{
}

DSPErrorCode_t CAudioDSPProcessor::CreateInstance(const void *InParameters, void *OutParameters, void *Options)
{
  IDSPNodeModel::DSPNodeInfoVector_t nodeInfos;
  m_AudioDSPController.GetNodeInfos(nodeInfos);

  for(uint32_t ii = 0; ii < nodeInfos.size(); ii++)
  {
    m_NodeFactory.InstantiateNode(nodeInfos.at(ii).ID);
  }

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPProcessor::DestroyInstance()
{
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
