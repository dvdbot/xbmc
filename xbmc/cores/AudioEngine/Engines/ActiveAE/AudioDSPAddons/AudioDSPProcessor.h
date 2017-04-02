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

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPProcessor.h"
#include "cores/DSP/Models/Interfaces/IDSPNodeModelCallback.h"

class CAudioDSPProcessor : public DSP::AUDIO::IADSPProcessor, public DSP::IDSPNodeModelCallback
{
public:
  CAudioDSPProcessor();
  virtual ~CAudioDSPProcessor();

private:
  // processor callbacks
  virtual DSPErrorCode_t CreateInstance(const void *InParameters, void *OutParameters, void *Options = nullptr) override;
  virtual DSPErrorCode_t DestroyInstance() override;

  // node model callbacks
  virtual DSPErrorCode_t EnableNodeCallback(uint64_t ID, uint32_t Position = 0) override;
  virtual DSPErrorCode_t DisableNodeCallback(uint64_t ID) override;
};

