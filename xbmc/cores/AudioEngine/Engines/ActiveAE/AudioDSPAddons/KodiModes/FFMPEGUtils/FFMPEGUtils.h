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

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ADSPTypedefs.h"

extern "C" {
#include "libavutil/samplefmt.h"
}

namespace ActiveAE
{
class CFFMPEGUtils
{
public:  
  static uint64_t GetChannel(AE_DSP_CHANNEL Channel);
  static const int GetChannelIndex(AE_DSP_CHANNEL Channel, uint64_t layout);
  static uint64_t GetChannelLayout(const DSP::AUDIO::CChannelInformation &channelInfo);
  static const unsigned int DataFormatToBits(const DSP::AUDIO::ADSPDataFormat_t dataFormat);
  static const unsigned int DataFormatToDitherBits(const DSP::AUDIO::ADSPDataFormat_t dataFormat);
  static const unsigned int DataFormatToUsedBits(const DSP::AUDIO::ADSPDataFormat_t dataFormat);
  static AVSampleFormat GetSampleFormat(const DSP::AUDIO::ADSPDataFormat_t format);
};
}
