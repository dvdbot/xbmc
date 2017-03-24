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

#include <sstream>

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ActiveAudioDSP.h"
#include "cores/DSP/Utils/DSPIDFactory.h"
#include "utils/log.h"

#include "ServiceBroker.h"

 // includes for AudioDSP add-on modes
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonNodeCreator.h"

using namespace Actor;
using namespace ADDON;
using namespace std;
using namespace DSP;


namespace ActiveAE
{
CActiveAudioDSP::CActiveAudioDSP(CEvent *inMsgEvent) :
  CThread("ActiveAudioDSP"),
  m_ADSPAddonControlPort("ADSPAddonControlPort", inMsgEvent, &m_outMsgEvent),
  m_ADSPAddonDataPort("ADSPAddonDataPort", inMsgEvent, &m_outMsgEvent)
{
  m_inMsgEvent = inMsgEvent;
}

CActiveAudioDSP::~CActiveAudioDSP()
{
  Dispose();
}

void CActiveAudioDSP::Start()
{
  if (!IsRunning())
  {
    Create();
    SetPriority(THREAD_PRIORITY_NORMAL);
  }
}

void CActiveAudioDSP::Dispose()
{

  m_bStop = true;
  m_outMsgEvent.Set();
  StopThread();

  m_KodiModes.ReleaseAllModes(m_DSPNodeFactory);

  for (vAudioDSPNodeCreators_t::iterator iter = m_AddonNodeCreators.begin(); iter != m_AddonNodeCreators.end(); ++iter)
  {
    m_DSPNodeFactory.DeregisterCreator((*iter)->ID);
  }
  m_AddonNodeCreators.clear();

  m_ADSPAddonControlPort.Purge();
  m_ADSPAddonDataPort.Purge();

  CServiceBroker::GetAddonMgr().UnregisterAddonMgrCallback(ADDON_ADSPDLL);

  m_databaseDSP.Close();
}

bool CActiveAudioDSP::RequestRestart(ADDON::AddonPtr addon, bool bDataChanged)
{
  return true;
}

bool CActiveAudioDSP::RequestRemoval(ADDON::AddonPtr addon)
{
  return true;
}

void CActiveAudioDSP::EnableAddon(const std::string& Id, bool Enable)
{
  if (Enable)
  {
    m_ADSPAddonControlPort.SendOutMessage(CADSPAddonControlProtocol::ENABLE_ADDON, (void*)Id.c_str(), sizeof(char)*Id.length() + 1);
  }
  else
  {
    m_ADSPAddonControlPort.SendOutMessage(CADSPAddonControlProtocol::DISABLE_ADDON, (void*)Id.c_str(), sizeof(char)*Id.length() + 1);
  }
}

bool CActiveAudioDSP::GetAddon(const std::string& Id, ADDON::AddonPtr& addon)
{
  return false;
}

void CActiveAudioDSP::RegisterAddon(const std::string& Id, bool restart, bool update)
{
}

void CActiveAudioDSP::UnregisterAddon(const std::string& Id)
{
}

enum SINK_STATES
{
  ADSP_TOP = 0,                      // 0
  ADSP_TOP_UNCONFIGURED,             // 1
  ADSP_TOP_CONFIGURED,               // 2
  ADSP_TOP_CONFIGURED_MANAGE_ADDONS, // 3
};

int ADSP_parentStates[] = {
    -1,
    ADSP_TOP,             //TOP_UNCONFIGURED
    ADSP_TOP,             //TOP_CONFIGURED
    ADSP_TOP_CONFIGURED,  //ADSP_TOP_CONFIGURED_MANAGE_ADDONS
};

void CActiveAudioDSP::StateMachine(int signal, Protocol *port, Message *msg)
{
  for (int state = m_state; 1; state = ADSP_parentStates[state])
  {
    switch (state)
    {
    case ADSP_TOP: // TOP
      if (port == &m_ADSPAddonControlPort)
      {
        switch (signal)
        {
        case CADSPAddonControlProtocol::CONFIGURE:
          m_state = ADSP_TOP_CONFIGURED;
          return;

        case CADSPAddonControlProtocol::UNCONFIGURE:
          m_state = ADSP_TOP_UNCONFIGURED;
          msg->Reply(CADSPAddonControlProtocol::ACC);
          return;

        default:
          break;
        }
      }
      else if (port == &m_ADSPAddonDataPort)
      {
        switch (signal)
        {
        case CADSPAddonDataProtocol::DRAIN:
          msg->Reply(CADSPAddonDataProtocol::ACC);
          m_state = ADSP_TOP_UNCONFIGURED;
          m_extTimeout = 0;
          return;
        default:
          break;
        }
      }
      {
        std::string portName = port == nullptr ? "timer" : port->portName;
        CLog::Log(LOGWARNING, "%s - signal: %d form port: %s not handled for state: %d", __FUNCTION__, signal, portName.c_str(), m_state);
      }
      return;

    case ADSP_TOP_UNCONFIGURED:
      if (port == nullptr) // timeout
      {
        switch (signal)
        {
        case CADSPAddonControlProtocol::TIMEOUT:
          m_extTimeout = 1000;
          return;
        default:
          break;
        }
      }
      else if (port == &m_ADSPAddonControlPort)
      {
        switch (signal)
        {
          case CADSPAddonControlProtocol::INIT:
          {
            m_databaseDSP.Open();

            //std::set<std::string> settingSet;
            //settingSet.insert(CSettings::SETTING_AUDIOOUTPUT_DSPADDONSENABLED);
            //settingSet.insert(CSettings::SETTING_AUDIOOUTPUT_DSPSETTINGS);
            //settingSet.insert(CSettings::SETTING_AUDIOOUTPUT_DSPRESETDB);
            //CServiceBroker::GetSettings().RegisterCallback(this, settingSet);

            CServiceBroker::GetAddonMgr().RegisterAddonMgrCallback(ADDON_ADSPDLL, this);

            //CSingleLock lock(m_critSection);

            //CLog::Log(LOGNOTICE, "ActiveAE DSP - starting");
            m_KodiModes.ReleaseAllModes(m_DSPNodeFactory);
            m_KodiModes.PrepareModes(m_DSPNodeFactory, m_DSPChainModelObject);
            PrepareAddons();
            PrepareAddonModes();

            m_state = ADSP_TOP_CONFIGURED_MANAGE_ADDONS;
          }
          break;
          
          default:
          break;
        }
      }
      break;

    case ADSP_TOP_CONFIGURED:
      if (port == &m_ADSPAddonControlPort)
      {
        switch (signal)
        {
        default:
          break;
        }
      }
      else if (port == &m_ADSPAddonDataPort)
      {
        switch (signal)
        {
        case CADSPAddonDataProtocol::DRAIN:
          msg->Reply(CADSPAddonDataProtocol::ACC);
          m_state = ADSP_TOP_CONFIGURED_MANAGE_ADDONS;
          m_extTimeout = 10000;
          return;
        default:
          break;
        }
      }
      break;

    case ADSP_TOP_CONFIGURED_MANAGE_ADDONS:
      if (port == &m_ADSPAddonDataPort)
      {
        switch (signal)
        {
        default:
          break;
        }
      }
      if (port == &m_ADSPAddonControlPort)
      {
        switch (signal)
        {
        case CADSPAddonControlProtocol::ENABLE_ADDON:
          {
            string addonId = (char*)msg->data;
            AudioDSPAddonMap_t::iterator iter = m_DisabledAddons.find(addonId);
            if(iter == m_DisabledAddons.end())
            { 
              CLog::Log(LOGERROR, "%s, Tried to enable the unknown addon \"%s\"", __FUNCTION__, addonId.c_str());
            }
            else
            {
              m_EnabledAddons[addonId] = iter->second;
              //! @todo add addon to processing object
              m_DisabledAddons.erase(addonId);
              
              //! @todo implement hash ID here
              iter->second->Create(0);
            }
          }
          break;

        case CADSPAddonControlProtocol::DISABLE_ADDON:
          {
            string addonId = (char*)msg->data;
            AudioDSPAddonMap_t::iterator iter = m_EnabledAddons.find(addonId);
            if(iter == m_EnabledAddons.end())
            { 
              CLog::Log(LOGERROR, "%s, Tried to enable the unknown addon \"%s\"", __FUNCTION__, addonId.c_str());
            }
            else
            {
              iter->second->Destroy();
              m_DisabledAddons[addonId] = iter->second;
              //! @todo remove addon from processing object
              m_EnabledAddons.erase(addonId);
            }
          }
          break;

        default:
          break;
        }
      }
      else if (port == nullptr) // timeout
      {
        switch (signal)
        {
        default:
          break;
        }
      }
      break;

    default: // AudioDSP is in an unknown state, should not happen!
      CLog::Log(LOGERROR, "CActiveAudioDSP::%s - no valid state: %d", __FUNCTION__, m_state);
      return;
    }
  } // for
}

void CActiveAudioDSP::PrepareAddons()
{
  VECADDONS addons;
  CServiceBroker::GetAddonMgr().GetInstalledAddons(addons, ADDON_ADSPDLL);

  if (addons.empty())
    return;

  for (auto &addon : addons)
  {
    pAudioDSPAddon_t dspAddon = std::dynamic_pointer_cast<CActiveAEDSPAddon>(addon);
    if(!dspAddon)
    {
      CLog::Log(LOGERROR, "%s - failed to cast addon to CActiveAEDSPAddon", __FUNCTION__);
      continue;
    }
    if (CServiceBroker::GetAddonMgr().IsAddonDisabled(addon->ID()))
    {
      m_DisabledAddons[addon->ID()] = dspAddon;

      //! @todo implement hash for dll calls
      //std::hash<std::string> hasher;
      //int iAddonId = static_cast<int>(hasher(addon->ID()));
      //if (iAddonId < 0)
      //  iAddonId = -iAddonId;
    }
    else
    {
      m_EnabledAddons[addon->ID()] = dspAddon;
    }
  }
}

void CActiveAudioDSP::PrepareAddonModes()
{
  IDSPChainModel::DSPNodeInfoVector_t tmpNodeInfos;
  IDSPChainModel::DSPNodeInfoVector_t tmpActiveNodeInfos;

  // get all add-on modes
  for (AudioDSPAddonMap_t::iterator iter = m_EnabledAddons.begin(); iter != m_EnabledAddons.end(); ++iter)
  {
    AE_DSP_ADDON_CAPABILITIES caps = iter->second->GetAddonCapabilities();
    unsigned int maxModeCount = 1;
    for (unsigned int modeCount = 0; modeCount < maxModeCount; modeCount++)
    {
      uint32_t addonID = 0;         //! @todo get real parameters
      uint16_t modeID = 0;          //! @todo get real parameters
      uint16_t modeInstanceID = 0;  //! @todo get real parameters

      NodeID_t id(addonID, modeID, modeInstanceID);
      m_DSPChainModelObject.AddNode(IDSPChainModel::DSPNodeInfo_t(id, iter->first, false)); //! @todo how to handle errors?

      m_AddonNodeCreators.push_back(std::unique_ptr<DSP::IDSPNodeCreator>(dynamic_cast<IDSPNodeCreator*>(new CAudioDSPAddonNodeCreator(id, iter->second))));
      m_DSPNodeFactory.RegisterCreator(m_AddonNodeCreators.back().get()); //! @todo how to handle errors?
    }
  }

  // set mode order from json file
  //GetActiveNodesFromJsonFile(tmpActiveNodeInfos);
  for (IDSPChainModel::DSPNodeInfoVector_t::iterator iter = tmpActiveNodeInfos.begin(); iter != tmpActiveNodeInfos.end(); ++iter)
  {
    uint32_t addonID;         //! @todo get real parameters
    uint16_t modeID;          //! @todo get real parameters
    uint16_t modeInstanceID;  //! @todo get real parameters
    uint32_t pos;             //! @todo get real parameters
    
    NodeID_t id(addonID, modeID, modeInstanceID);

    m_DSPChainModelObject.SetNodePosition(id, pos);
    m_DSPChainModelObject.EnableNode(id);
  }

}

void CActiveAudioDSP::CreateDSPNodeModel()
{
}

void CActiveAudioDSP::Process()
{
  Message *msg = nullptr;
  Protocol *port = nullptr;
  bool gotMsg;
  XbmcThreads::EndTime timer;

  m_state = ADSP_TOP_UNCONFIGURED;
  m_extTimeout = 1000;
  m_bStateMachineSelfTrigger = false;

  while (!m_bStop)
  {
    gotMsg = false;
    timer.Set(m_extTimeout);

    if (m_bStateMachineSelfTrigger)
    {
      m_bStateMachineSelfTrigger = false;
      // self trigger state machine
      StateMachine(msg->signal, port, msg);
      if (!m_bStateMachineSelfTrigger)
      {
        msg->Release();
        msg = nullptr;
      }
      continue;
    }
    // check control port
    else if (m_ADSPAddonControlPort.ReceiveOutMessage(&msg))
    {
      gotMsg = true;
      port = &m_ADSPAddonControlPort;
    }
    // check data port
    else if (m_ADSPAddonDataPort.ReceiveOutMessage(&msg))
    {
      gotMsg = true;
      port = &m_ADSPAddonDataPort;
    }

    if (gotMsg)
    {
      StateMachine(msg->signal, port, msg);
      if (!m_bStateMachineSelfTrigger)
      {
        msg->Release();
        msg = nullptr;
      }
      continue;
    }
    else if (m_outMsgEvent.WaitMSec(m_extTimeout))
    { // wait for message
      m_extTimeout = timer.MillisLeft();
      continue;
    }
    else
    { // time out
      msg = m_ADSPAddonControlPort.GetMessage();
      msg->signal = CADSPAddonControlProtocol::TIMEOUT;
      port = nullptr;
      // signal timeout to state machine
      StateMachine(msg->signal, port, msg);
      if (!m_bStateMachineSelfTrigger)
      {
        msg->Release();
        msg = nullptr;
      }
    }
  }
}
};
