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

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/FFMPEGUtils/FFMPEGUtils.h"

extern "C" {
#include "libavutil/channel_layout.h"
}

using namespace ActiveAE;
using namespace DSP;
using namespace DSP::AUDIO;

uint64_t ActiveAE::CFFMPEGUtils::GetChannel(AE_DSP_CHANNEL Channel)
{
  switch (Channel)
  {
  case AE_DSP_CH_FL:   return AV_CH_FRONT_LEFT;
  case AE_DSP_CH_FR:   return AV_CH_FRONT_RIGHT;
  case AE_DSP_CH_FC:   return AV_CH_FRONT_CENTER;
  case AE_DSP_CH_LFE:  return AV_CH_LOW_FREQUENCY;
  case AE_DSP_CH_BL:   return AV_CH_BACK_LEFT;
  case AE_DSP_CH_BR:   return AV_CH_BACK_RIGHT;
  case AE_DSP_CH_FLOC: return AV_CH_FRONT_LEFT_OF_CENTER;
  case AE_DSP_CH_FROC: return AV_CH_FRONT_RIGHT_OF_CENTER;
  case AE_DSP_CH_BC:   return AV_CH_BACK_CENTER;
  case AE_DSP_CH_SL:   return AV_CH_SIDE_LEFT;
  case AE_DSP_CH_SR:   return AV_CH_SIDE_RIGHT;
  case AE_DSP_CH_TC:   return AV_CH_TOP_CENTER;
  case AE_DSP_CH_TFL:  return AV_CH_TOP_FRONT_LEFT;
  case AE_DSP_CH_TFC:  return AV_CH_TOP_FRONT_CENTER;
  case AE_DSP_CH_TFR:  return AV_CH_TOP_FRONT_RIGHT;
  case AE_DSP_CH_TBL:  return AV_CH_TOP_BACK_LEFT;
  case AE_DSP_CH_TBC:  return AV_CH_TOP_BACK_CENTER;
  case AE_DSP_CH_TBR:  return AV_CH_TOP_BACK_RIGHT;
  default:
    return 0;
  }
}

const int CFFMPEGUtils::GetChannelIndex(AE_DSP_CHANNEL Channel, uint64_t layout)
{
  return av_get_channel_layout_channel_index(layout, GetChannel(Channel));
}

uint64_t CFFMPEGUtils::GetChannelLayout(const CChannelInformation & channelInfo)
{
  uint64_t channelLayout = 0;
  if (channelInfo.HasChannel(AE_DSP_CH_FL))   channelLayout |= AV_CH_FRONT_LEFT;
  if (channelInfo.HasChannel(AE_DSP_CH_FR))   channelLayout |= AV_CH_FRONT_RIGHT;
  if (channelInfo.HasChannel(AE_DSP_CH_FC))   channelLayout |= AV_CH_FRONT_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_LFE))  channelLayout |= AV_CH_LOW_FREQUENCY;
  if (channelInfo.HasChannel(AE_DSP_CH_BL))   channelLayout |= AV_CH_BACK_LEFT;
  if (channelInfo.HasChannel(AE_DSP_CH_BR))   channelLayout |= AV_CH_BACK_RIGHT;
  if (channelInfo.HasChannel(AE_DSP_CH_FLOC)) channelLayout |= AV_CH_FRONT_LEFT_OF_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_FROC)) channelLayout |= AV_CH_FRONT_RIGHT_OF_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_BC))   channelLayout |= AV_CH_BACK_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_SL))   channelLayout |= AV_CH_SIDE_LEFT;
  if (channelInfo.HasChannel(AE_DSP_CH_SR))   channelLayout |= AV_CH_SIDE_RIGHT;
  if (channelInfo.HasChannel(AE_DSP_CH_TC))   channelLayout |= AV_CH_TOP_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_TFL))  channelLayout |= AV_CH_TOP_FRONT_LEFT;
  if (channelInfo.HasChannel(AE_DSP_CH_TFC))  channelLayout |= AV_CH_TOP_FRONT_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_TFR))  channelLayout |= AV_CH_TOP_FRONT_RIGHT;
  if (channelInfo.HasChannel(AE_DSP_CH_TBL))   channelLayout |= AV_CH_TOP_BACK_LEFT;
  if (channelInfo.HasChannel(AE_DSP_CH_TBC))   channelLayout |= AV_CH_TOP_BACK_CENTER;
  if (channelInfo.HasChannel(AE_DSP_CH_TBR))   channelLayout |= AV_CH_TOP_BACK_RIGHT;

  return channelLayout;
}

const unsigned int CFFMPEGUtils::DataFormatToBits(const ADSPDataFormat_t dataFormat)
{
  if (dataFormat < 0 || dataFormat >= ADSP_DataFormatMAX)
    return 0;

  static const unsigned int formats[ADSP_DataFormatMAX] =
  {
    8,                            /* ADSP_DataFormatU8 */

    16,                           /* ADSP_DataFormatS16BE */
    16,                           /* ADSP_DataFormatS16LE */
    16,                           /* ADSP_DataFormatS16NE */

    32,                           /* ADSP_DataFormatS32BE */
    32,                           /* ADSP_DataFormatS32LE */
    32,                           /* ADSP_DataFormatS32NE */

    32,                           /* ADSP_DataFormatS24BE */
    32,                           /* ADSP_DataFormatS24LE */
    32,                           /* ADSP_DataFormatS24NE */
    32,                           /* ADSP_DataFormatS24NE2MSB */

    24,                           /* ADSP_DataFormatS24BE3 */
    24,                           /* ADSP_DataFormatS24LE3 */
    24,                           /* ADSP_DataFormatS24NE3 */

    sizeof(float) << 3,           /* ADSP_DataFormatFloat  */
    sizeof(double) << 3,          /* ADSP_DataFormatDouble */
    sizeof(long double) << 3,     /* ADSP_DataFormatLongDouble */

    8,                            /* ADSP_DataFormatU8P    */
    16,                           /* ADSP_DataFormatS16NEPlanes */
    32,                           /* ADSP_DataFormatS32NEPlanes */
    32,                           /* ADSP_DataFormatS24NE4Planes */
    32,                           /* ADSP_DataFormatS24NE4MSBPlanes */
    24,                           /* ADSP_DataFormatS24NE3Planes */
    sizeof(float) << 3,           /* ADSP_DataFormatFloatPlanes */
    sizeof(double) << 3,          /* ADSP_DataFormatDoublePlanes */
    sizeof(long double) << 3      /* ADSP_DataFormatLongDoublePlanes */
  };

  return formats[dataFormat];
}

const unsigned int CFFMPEGUtils::DataFormatToDitherBits(const ADSPDataFormat_t dataFormat)
{
  if (dataFormat == ADSP_DataFormatS24NE4MSB)
    return 8;
  if (dataFormat == ADSP_DataFormatS24NE3)
    return -8;
  else
    return 0;
}

const unsigned int CFFMPEGUtils::DataFormatToUsedBits(const ADSPDataFormat_t dataFormat)
{
  if (dataFormat == ADSP_DataFormatS24BE4 || dataFormat == ADSP_DataFormatS24LE4 ||
    dataFormat == ADSP_DataFormatS24NE4 || dataFormat == ADSP_DataFormatS24NE4MSB)
    return 24;
  else
    return DataFormatToBits(dataFormat);
}

AVSampleFormat CFFMPEGUtils::GetSampleFormat(const ADSPDataFormat_t format)
{
  switch (format)
  {
    case ADSP_DataFormatU8:
      return AV_SAMPLE_FMT_U8;
    case ADSP_DataFormatS16NE:
      return AV_SAMPLE_FMT_S16;
    case ADSP_DataFormatS32NE:
      return AV_SAMPLE_FMT_S32;
    case ADSP_DataFormatS24NE4:
      return AV_SAMPLE_FMT_S32;
    case ADSP_DataFormatS24NE4MSB:
      return AV_SAMPLE_FMT_S32;
    case ADSP_DataFormatS24NE3:
      return AV_SAMPLE_FMT_S32;
    case ADSP_DataFormatFloat:
      return AV_SAMPLE_FMT_FLT;
    case ADSP_DataFormatDouble:
      return AV_SAMPLE_FMT_DBL;
    case ADSP_DataFormatU8Planes:
      return AV_SAMPLE_FMT_U8P;
    case ADSP_DataFormatS16NEPlanes:
      return AV_SAMPLE_FMT_S16P;
    case ADSP_DataFormatS32NEPlanes:
      return AV_SAMPLE_FMT_S32P;
    case ADSP_DataFormatS24NE4Planes:
      return AV_SAMPLE_FMT_S32P;
    case ADSP_DataFormatS24NE4MSBPlanes:
      return AV_SAMPLE_FMT_S32P;
    case ADSP_DataFormatS24NE3Planes:
      return AV_SAMPLE_FMT_S32P;
    case ADSP_DataFormatFloatPlanes:
      return AV_SAMPLE_FMT_FLTP;
    case ADSP_DataFormatDoublePlanes:
      return AV_SAMPLE_FMT_DBLP;
    case ADSP_DataFormatMAX:
    case ADSP_DataFormatINVALID:
    default:
      return AV_SAMPLE_FMT_NONE;
  }
}
