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

#include "threads/Event.h"
#include "threads/Thread.h"
#include "utils/ActorProtocol.h"

#include "ActiveAEDSPDatabase.h"

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPProcessor.h"
#include "cores/AudioEngine/Interfaces/IAudioDSP.h"

// MVC Pattern object includes
#include "cores/DSP/Models/DSPNodeModel.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPController.h"

// internal Kodi AudioDSP processing mode include files
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPKodiModes.h"

#include <map>
#include <memory>

// notes for configuration of ActiveAE and AudioDSP
// first configure AE
// then AudioDSP
// after that configure the sink

namespace ActiveAE
{
using namespace Actor;
class CActiveAEStream;
class IActiveAEProcessingBuffer;

class CAudioDSPAddonControlProtocol : public Protocol
{
public:
  CAudioDSPAddonControlProtocol(std::string name, CEvent *inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {}
  enum OutSignal
  {
    REMOVE_ADDON,
    ENABLE_ADDON,
    DISABLE_ADDON,
  };
  enum InSignal
  {
    ACC,
    ERR,
  };
};

class CAudioDSPControlProtocol : public Protocol
{
public:
  struct CCreateBuffer
  {
    CCreateBuffer(const CActiveAEStream *AudioStream, AEAudioFormat &OutputFormat) : audioStream(AudioStream), outputFormat(OutputFormat) {}
    const CActiveAEStream *audioStream;
    AEAudioFormat &outputFormat;
  };

  CAudioDSPControlProtocol(std::string name, CEvent *inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {}
  enum OutSignal
  {
    DEINIT = 0,
    INIT,
    GET_PROCESSING_BUFFER,
    RELEASE_PROCESSING_BUFFER,
    TIMEOUT,
  };
  enum InSignal
  {
    ACC,
    SUCCESS,
    ERR,
  };
};

class CAudioDSPProcessorControlProtocol : public Protocol
{
public:
  CAudioDSPProcessorControlProtocol(std::string name, CEvent *inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {}
  enum OutSignal
  {
    CREATE_PROCESSOR = 0,
    DESTROY_PROCESSOR,
  };
  enum InSignal
  {
    ACC,
  };
};


class CActiveAudioDSP : public IAudioDSP,
                        public ADDON::IAddonMgrCallback,
                        private CThread
{
  typedef std::vector<DSP::AUDIO::IADSPProcessor*>      AudioDSPProcessorVector_t;
  typedef std::shared_ptr<ActiveAE::CActiveAEDSPAddon>  pAudioDSPAddon_t;
  typedef std::map<std::string, pAudioDSPAddon_t>       AudioDSPAddonMap_t;
  typedef std::map<int, IActiveAEProcessingBuffer*>     AudioDSPProcessingBufferMap_t;

public:
  CActiveAudioDSP(CEvent *inMsgEvent);
  ~CActiveAudioDSP();
  void Start();
  void Stop();

  /*!
   * @brief Restart an AudioDSP add-on.
   * @param addon The add-on to restart.
   * @param bDataChanged True if add-on's data has changed, false otherwise (unused).
   * @return True if the AudioDSP add-on was found and restarted, false otherwise.
   */
  virtual bool RequestRestart(ADDON::AddonPtr addon, bool bDataChanged) override;

  /*!
   * @brief Remove a single AudioDSP add-on.
   * @param addon AudioDSP add-on to remove.
   * @return True if the AudioDSP add-on was found and restarted, false otherwise.
   */
  virtual bool RequestRemoval(ADDON::AddonPtr addon) override;

  virtual DSP::AUDIO::IADSPProcessor *CreateProcessor();
  virtual void DestroyProcessor(DSP::AUDIO::IADSPProcessor *Processor);
  virtual void EnableAddon(const std::string &Id, bool Enable) override;
  virtual bool GetAddon(const std::string &Id, ADDON::AddonPtr &addon) override;

  virtual void RegisterAddon(const std::string &Id, bool restart = false, bool update = false) override;
  virtual void UnregisterAddon(const std::string &Id) override;

  IActiveAEProcessingBuffer* GetProcessingBuffer(const CActiveAEStream *AudioStream, AEAudioFormat &OutputFormat);
  DSPErrorCode_t ReleaseProcessingBuffer(int StreamID);

  // internal Kodi AudioDSP modes
  CAudioDSPKodiModes m_KodiModes;

protected:
  // ports
  CAudioDSPControlProtocol m_ControlPort;
  CAudioDSPAddonControlProtocol m_AddonControlPort;
  CAudioDSPProcessorControlProtocol m_ProcessorDataPort;

  void Process();
  void StateMachine(int signal, Protocol *port, Message *msg);

  CEvent m_outMsgEvent;
  CEvent *m_inMsgEvent;
  int m_state;
  unsigned int m_extSilenceTimeout;
  XbmcThreads::EndTime m_extSilenceTimer;

  bool m_hasError;
  int m_extTimeout;
  bool m_bStateMachineSelfTrigger;
  
private:
  void PrepareAddons();
  void PrepareAddonModes();
  void CreateDSPNodeModel();

  AudioDSPProcessorVector_t m_AudioDSPProcessors;
  AudioDSPProcessingBufferMap_t m_ProcessingBuffers;

  DSP::CDSPNodeModel m_DSPChainModelObject;
  CAudioDSPController m_Controller;
  AudioDSPAddonMap_t m_EnabledAddons;
  AudioDSPAddonMap_t m_DisabledAddons;
  CActiveAEDSPDatabase m_databaseDSP;  /*!< database for all audio DSP related data */
};
}