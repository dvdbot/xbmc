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

#include "cores/AudioEngine/Interfaces/IAudioDSP.h"
#include "cores/DSP/Models/DSPChainModel.h"
#include "cores/DSP/Factory/DSPNodeFactory.h"

 // internal Kodi AudioDSP processing mode include files
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPKodiModes.h"

#include <map>

// notes for configuration of ActiveAE and AudioDSP
// first configure AE
// then AudioDSP
// after that configure the sink

namespace ActiveAE
{
using namespace Actor;

class CADSPAddonControlProtocol : public Protocol
{
public:
  CADSPAddonControlProtocol(std::string name, CEvent* inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {};
  enum OutSignal
  {
    CONFIGURE = 0,
    UNCONFIGURE,
    INIT,
    REMOVE_ADDON,
    FLUSH,
    ENABLE_ADDON,
    DISABLE_ADDON,
    TIMEOUT
  };
  enum InSignal
  {
    ACC,
    ERR,
    STATS,
  };
};

class CADSPAddonDataProtocol : public Protocol
{
public:
  CADSPAddonDataProtocol(std::string name, CEvent* inEvent, CEvent *outEvent) : Protocol(name, inEvent, outEvent) {};
  enum OutSignal
  {
    DRAIN = 0,
  };
  enum InSignal
  {
    RETURNSAMPLE,
    ACC,
  };
};

class CActiveAudioDSP : private CThread, 
                        public IAudioDSP,
                        public ADDON::IAddonMgrCallback
{
  typedef std::shared_ptr<ActiveAE::CActiveAEDSPAddon>  pAudioDSPAddon_t;
  typedef std::map<std::string, pAudioDSPAddon_t>       AudioDSPAddonMap_t;
  typedef std::vector<std::unique_ptr<DSP::IDSPNodeCreator>> vAudioDSPNodeCreators_t;

public:
  CActiveAudioDSP(CEvent *inMsgEvent);
  ~CActiveAudioDSP();
  void Start();
  void Dispose();
  CADSPAddonControlProtocol m_ADSPAddonControlPort;
  CADSPAddonDataProtocol m_ADSPAddonDataPort;

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


  virtual void EnableAddon(const std::string& Id, bool Enable) override;
  virtual bool GetAddon(const std::string& Id, ADDON::AddonPtr &addon) override;

  virtual void RegisterAddon(const std::string& Id, bool restart = false, bool update = false) override;
  virtual void UnregisterAddon(const std::string& Id) override;

protected:
  void Process();
  void StateMachine(int signal, Protocol *port, Message *msg);

  CEvent m_outMsgEvent;
  CEvent *m_inMsgEvent;
  int m_state;
  unsigned int m_extSilenceTimeout;
  XbmcThreads::EndTime m_extSilenceTimer;

  int m_extTimeout;
  bool m_bStateMachineSelfTrigger;

  enum
  {
    CHECK_SWAP,
    NEED_CONVERT,
    NEED_BYTESWAP,
    SKIP_SWAP,
  } m_swapState;

private:
  void PrepareAddons();
  void PrepareAddonModes();
  void CreateDSPNodeModel();

  DSP::CDSPNodeFactory    m_DSPNodeFactory;
  DSP::CDSPChainModel     m_DSPChainModelObject;
  AudioDSPAddonMap_t      m_EnabledAddons;
  AudioDSPAddonMap_t      m_DisabledAddons;
  CActiveAEDSPDatabase    m_databaseDSP;  /*!< database for all audio DSP related data */

  vAudioDSPNodeCreators_t m_AddonNodeCreators;

  // internal Kodi AudioDSP modes
  CAudioDSPKodiModes m_KodiModes;
};
}
