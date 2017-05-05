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

#pragma once

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPProcessor.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPController.h"

#include "cores/DSP/Factory/Interfaces/IDSPNodeFactory.h"
#include "cores/DSP/Models/Interfaces/IDSPNodeModelCallback.h"

namespace ActiveAE
{
class CAudioDSPProcessor : public DSP::AUDIO::IADSPProcessor, public DSP::IDSPNodeModelCallback
{
  typedef std::vector<DSP::AUDIO::IADSPNode*> AudioDSPNodeChain_t;
public:
  CAudioDSPProcessor(const CAudioDSPController &Controller, DSP::IDSPNodeFactory &NodeFactory);
  virtual ~CAudioDSPProcessor();

private: // private methods
  DSPErrorCode_t ReCreateNodeChain();

private:
  // processor interface
  virtual DSPErrorCode_t Create(const AEAudioFormat *InFormat, AEAudioFormat *OutFormat) override;
  virtual DSPErrorCode_t Process(const ActiveAE::CSampleBuffer *In, ActiveAE::CSampleBuffer *Out) override;
  virtual DSPErrorCode_t Destroy() override;

  // node model callbacks
  virtual DSPErrorCode_t EnableNodeCallback(uint64_t ID, uint32_t Position = 0) override;
  virtual DSPErrorCode_t DisableNodeCallback(uint64_t ID) override;

  AudioDSPNodeChain_t m_DSPNodeChain;
  AEAudioFormat m_InFormat;
  AEAudioFormat m_OutFormat;

  const CAudioDSPController &m_AudioDSPController;
  DSP::IDSPNodeFactory &m_NodeFactory;
};
}
