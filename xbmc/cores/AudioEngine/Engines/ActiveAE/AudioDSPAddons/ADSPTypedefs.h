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

#include "addons/kodi-addon-dev-kit/include/kodi/kodi_adsp_types.h"
#include "cores/DSP/Typedefs/DSPTypedefs.h"

namespace DSP
{
namespace AUDIO
{
typedef enum
{
  ADSP_BASE_ID_INVALID = -1,

  ADSP_BASE_ID_PROPERTIES,

  ADSP_BASE_ID_MAX
}ADSPBaseClassIDs_t;

//// FFMPEG Data Format
//AV_SAMPLE_FMT_NONE = -1,
//AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
//AV_SAMPLE_FMT_S16,         ///< signed 16 bits
//AV_SAMPLE_FMT_S32,         ///< signed 32 bits
//AV_SAMPLE_FMT_FLT,         ///< float
//AV_SAMPLE_FMT_DBL,         ///< double
//
//AV_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
//AV_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
//AV_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
//AV_SAMPLE_FMT_FLTP,        ///< float, planar
//AV_SAMPLE_FMT_DBLP,        ///< double, planar
//
//AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
//
//AE_FMT_INVALID = -1,
//
//// ActiveAE Data Format
//AE_FMT_U8,
//
//AE_FMT_S16BE,
//AE_FMT_S16LE,
//AE_FMT_S16NE,
//
//AE_FMT_S32BE,
//AE_FMT_S32LE,
//AE_FMT_S32NE,
//
//AE_FMT_S24BE4,
//AE_FMT_S24LE4,
//AE_FMT_S24NE4,    // 24 bits in lower 3 bytes
//AE_FMT_S24NE4MSB, // S32 with bits_per_sample < 32
//
//AE_FMT_S24BE3,
//AE_FMT_S24LE3,
//AE_FMT_S24NE3, /* S24 in 3 bytes */
//
//AE_FMT_DOUBLE,
//AE_FMT_FLOAT,
//
//// Bitstream
//AE_FMT_RAW,
//
///* planar formats */
//AE_FMT_U8P,
//AE_FMT_S16NEP,
//AE_FMT_S32NEP,
//AE_FMT_S24NE4P,
//AE_FMT_S24NE4MSBP,
//AE_FMT_S24NE3P,
//AE_FMT_DOUBLEP,
//AE_FMT_FLOATP,
//
//AE_FMT_MAX

typedef enum
{
  ADSP_DataFormatINVALID = -1,

  ADSP_DataFormatU8,

  ADSP_DataFormatS16BE,
  ADSP_DataFormatS16LE,
  ADSP_DataFormatS16NE,

  ADSP_DataFormatS32BE,
  ADSP_DataFormatS32LE,
  ADSP_DataFormatS32NE,

  ADSP_DataFormatS24BE4,
  ADSP_DataFormatS24LE4,
  ADSP_DataFormatS24NE4,    // 24 bits in lower 3 bytes
  ADSP_DataFormatS24NE4MSB, // S32 with bits_per_sample < 32

  ADSP_DataFormatS24BE3,
  ADSP_DataFormatS24LE3,
  ADSP_DataFormatS24NE3, /* S24 in 3 bytes */

  ADSP_DataFormatFloat,
  ADSP_DataFormatDouble,
  ADSP_DataFormatLongDouble,

  /* planar formats */
  ADSP_DataFormatU8Planes,
  ADSP_DataFormatS16NEPlanes,
  ADSP_DataFormatS32NEPlanes,
  ADSP_DataFormatS24NE4Planes,
  ADSP_DataFormatS24NE4MSBPlanes,
  ADSP_DataFormatS24NE3Planes,
  ADSP_DataFormatFloatPlanes,
  ADSP_DataFormatDoublePlanes,
  ADSP_DataFormatLongDoublePlanes,

  ADSP_DataFormatMAX
}ADSPDataFormat_t;

typedef enum
{
  ADSP_DataFormatFlagINVALID = 0,

  ADSP_DataFormatFlagU8                 = 1 << ADSP_DataFormatU8,

  ADSP_DataFormatFlagS16BE              = 1 << ADSP_DataFormatS16BE,
  ADSP_DataFormatFlagS16LE              = 1 << ADSP_DataFormatS16LE,
  ADSP_DataFormatFlagS16NE              = 1 << ADSP_DataFormatS16NE,

  ADSP_DataFormatFlagS32BE              = 1 << ADSP_DataFormatS32BE,
  ADSP_DataFormatFlagS32LE              = 1 << ADSP_DataFormatS32LE,
  ADSP_DataFormatFlagS32NE              = 1 << ADSP_DataFormatS32NE,

  ADSP_DataFormatFlagS24BE4             = 1 << ADSP_DataFormatS24BE4,
  ADSP_DataFormatFlagS24LE4             = 1 << ADSP_DataFormatS24LE4,
  ADSP_DataFormatFlagS24NE4             = 1 << ADSP_DataFormatS24NE4,    // 24 bits in lower 3 bytes
  ADSP_DataFormatFlagS24NE4MSB          = 1 << ADSP_DataFormatS24NE4MSB, // S32 with bits_per_sample < 32

  ADSP_DataFormatFlagS24BE3             = 1 << ADSP_DataFormatS24BE3,
  ADSP_DataFormatFlagS24LE3             = 1 << ADSP_DataFormatS24LE3,
  ADSP_DataFormatFlagS24NE3             = 1 << ADSP_DataFormatS24NE3, /* S24 in 3 bytes */

  ADSP_DataFormatFlagFloat              = 1 << ADSP_DataFormatFloat,
  ADSP_DataFormatFlagDouble             = 1 << ADSP_DataFormatDouble,
  ADSP_DataFormatFlagLongDouble         = 1 << ADSP_DataFormatLongDouble,

  /* planar formats */
  ADSP_DataFormatFlagU8Planes           = 1 << ADSP_DataFormatU8Planes,
  ADSP_DataFormatFlagS16NEPlanes        = 1 << ADSP_DataFormatS16NEPlanes,
  ADSP_DataFormatFlagS32NEPlanes        = 1 << ADSP_DataFormatS32NEPlanes,
  ADSP_DataFormatFlagS24NE4Planes       = 1 << ADSP_DataFormatS24NE4Planes,
  ADSP_DataFormatFlagS24NE4MSBPlanes    = 1 << ADSP_DataFormatS24NE4MSBPlanes,
  ADSP_DataFormatFlagS24NE3Planes       = 1 << ADSP_DataFormatS24NE3Planes,
  ADSP_DataFormatFlagFloatPlanes        = 1 << ADSP_DataFormatFloatPlanes,
  ADSP_DataFormatFlagDoublePlanes       = 1 << ADSP_DataFormatDoublePlanes,
  ADSP_DataFormatFlagLongDoublePlanes   = 1 << ADSP_DataFormatLongDoublePlanes,

  ADSP_DataFormatFlagMAX                = 1 << ADSP_DataFormatMAX
}ADSPDataFormatFlags_t;

class CChannelInformation
{
  friend class CChannelInformation;
public:
  CChannelInformation()
  {
    SetDefaultLayout();
  }

  void SetDefaultLayout()
  {
    for (uint8_t ii = AE_DSP_CH_FL; ii < AE_DSP_CH_MAX; ii++)
    {
      m_Channels[ii] = static_cast<AE_DSP_CHANNEL>(ii);
    }
    m_ChannelCount = AE_DSP_CH_MAX;
  }

  DSPErrorCode_t SetChannelLayout(const std::vector<AE_DSP_CHANNEL> &Channels)
  {
    if (Channels.size() == 0 || Channels.size() > AE_DSP_CH_MAX)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    ResetChannelLayout();

    for (uint8_t ch = 0; ch < Channels.size(); ch++)
    {
      m_Channels[ch] = Channels.at(ch);
      m_ChannelCount++;
    }

    return DSP_ERR_NO_ERR;
  }

  const AE_DSP_CHANNEL GetChannelAtIndex(uint32_t Index) const
  {
    if (Index >= AE_DSP_CH_MAX)
    {
      return AE_DSP_CH_INVALID;
    }

    return m_Channels[Index];
  }

  const uint32_t GetChannelCount() const
  {
    return m_ChannelCount;
  }

  bool operator != (const CChannelInformation &ChannelInfo)
  {
    if (m_ChannelCount != ChannelInfo.m_ChannelCount)
    {
      return false;
    }

    for (uint8_t ch = 0; ch < AE_DSP_CH_MAX; ch++)
    {
      if (m_Channels[ch] != ChannelInfo.m_Channels[ch])
      {
        return false;
      }
    }

    return false;
  }

  const bool HasChannel(const AE_DSP_CHANNEL Channel) const
  {
    for (uint8_t ch = 0; ch < AE_DSP_CH_MAX; ch++)
    {
      if (m_Channels[ch] == Channel)
      {
        return true;
      }
    }

    return false;
  }

private:
  void ResetChannelLayout()
  {
    m_ChannelCount = 0;
    for (uint8_t ii = AE_DSP_CH_FL; ii < AE_DSP_CH_MAX; ii++)
    {
      m_Channels[ii] = AE_DSP_CH_INVALID;
    }
  }
  AE_DSP_CHANNEL m_Channels[AE_DSP_CH_MAX];
  uint32_t m_ChannelCount;
};

typedef struct ADSP_Properties_t
{
  CChannelInformation speakerLayout;
  uint64_t            sampleFrequency;
  uint64_t            frameLength;
  ADSPDataFormat_t    dataFormat;

  ADSP_Properties_t& operator=(const ADSP_Properties_t& Props)
  {
    speakerLayout   = Props.speakerLayout;
    sampleFrequency = Props.sampleFrequency;
    frameLength     = Props.frameLength;
    dataFormat      = Props.dataFormat;
    
    return *this;
  }

  ADSP_Properties_t()
  {
    sampleFrequency = 0;
    frameLength     = 0;
    dataFormat      = ADSP_DataFormatINVALID;
  }
}ADSP_Properties_t;
}
}
