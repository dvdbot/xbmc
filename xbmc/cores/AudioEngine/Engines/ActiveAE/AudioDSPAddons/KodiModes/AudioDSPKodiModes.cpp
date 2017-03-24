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
CAudioDSPKodiModes::CAudioDSPKodiModes() :
  m_DummyModeCreator(NodeID_t()) //! @todo how to set Mode ID?
{
}

void CAudioDSPKodiModes::PrepareModes(DSP::IDSPNodeFactory &Factory, DSP::IDSPChainModel &Model)
{
  uint32_t addonID;
  uint16_t modeID;
  uint16_t modeInstanceID;
  uint32_t pos;

  NodeID_t id(addonID, modeID, modeInstanceID);

  DSPErrorCode_t err = Model.AddNode(IDSPChainModel::DSPNodeInfo_t(id, "", false));
  if (err != DSP_ERR_NO_ERR)
  {
    CLog::Log(LOGERROR, "%s failed to add CAudioDSPDummyMode!", __FUNCTION__);
  }

  err = Factory.RegisterCreator(dynamic_cast<IDSPNodeCreator*>(&m_DummyModeCreator));
  if (err != DSP_ERR_NO_ERR)
  {
    CLog::Log(LOGERROR, "%s failed to add CAudioDSPDummyModeCreator!", __FUNCTION__);
  }
}

void CAudioDSPKodiModes::ReleaseAllModes(DSP::IDSPNodeFactory &Factory)
{
  // release all internal Kodi AudioDSP modes
  DSPErrorCode_t err = Factory.DeregisterCreator(m_DummyModeCreator.ID);
  if (err != DSP_ERR_NO_ERR)
  {
    CLog::Log(LOGERROR, "%s failed to deregister CAudioDSPDummyMode!", __FUNCTION__);
  }
}
}
