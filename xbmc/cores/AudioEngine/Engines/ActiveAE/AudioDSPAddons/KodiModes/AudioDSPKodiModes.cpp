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

#include "cores/AudioEngine/Engines//ActiveAE/AudioDSPAddons/KodiModes/AudioDSPKodiModes.h"
#include "utils/log.h"

using namespace DSP;

namespace ActiveAE
{
CAudioDSPKodiModes::CAudioDSPKodiModes()
{
}

void CAudioDSPKodiModes::PrepareModes(IDSPNodeFactory &Factory, IDSPNodeModel &Model, IDSPIDFactory &IDFactory)
{
  IDFactory.RegisterNodeID("Kodi", "AudioDSPCopyMode");
  NodeID_t id(IDFactory.GetNodeID("Kodi", "AudioDSPCopyMode"));
  if (id != 0x0)
  {
    CAudioDSPCopyModeCreator *copyModeCreator = new CAudioDSPCopyModeCreator;
    m_NodeCreators.push_back(dynamic_cast<IDSPNodeCreator*>(copyModeCreator));

    DSPErrorCode_t err = Model.AddNode(IDSPNodeModel::CDSPNodeInfo(id, "AudioDSPCopyMode", false), copyModeCreator);
    if (err != DSP_ERR_NO_ERR)
    {
      delete copyModeCreator;
      CLog::Log(LOGERROR, "%s failed to add AudioDSPCopyMode!", __FUNCTION__);
    }

    err = Factory.RegisterCreator(m_NodeCreators.back());
    if (err != DSP_ERR_NO_ERR)
    {
      delete copyModeCreator;
      Model.RemoveNode(id);
      CLog::Log(LOGERROR, "%s failed to add AudioDSPCopyModeCreator!", __FUNCTION__);
    }
  }
  else
  {
    CLog::Log(LOGERROR, "%s failed to get valid node ID for AudioDSPCopyMode!", __FUNCTION__);
  }
}

void CAudioDSPKodiModes::ReleaseAllModes(IDSPNodeFactory &Factory, IDSPIDFactory &IDFactory)
{
  // release all internal Kodi AudioDSP modes
  NodeID_t id(IDFactory.GetNodeID("Kodi", "AudioDSPCopyMode"));

  for (std::vector<DSP::IDSPNodeCreator*>::iterator iter = m_NodeCreators.begin(); iter != m_NodeCreators.end(); ++iter)
  {
    DSPErrorCode_t err = Factory.DeregisterCreator((*iter)->GetID());
    if (err != DSP_ERR_NO_ERR)
    {
      CLog::Log(LOGERROR, "%s failed to deregister %i!", __FUNCTION__, (*iter)->GetID());
    }

    delete *iter;
    *iter = nullptr;
  }

}
}
