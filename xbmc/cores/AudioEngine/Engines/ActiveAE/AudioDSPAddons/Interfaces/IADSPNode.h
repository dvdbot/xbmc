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

#include "cores/DSP/Nodes/Interfaces/IDSPNode.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ADSPTypedefs.h"
#include "cores/AudioEngine/Utils/AEAudioFormat.h"

namespace DSP
{
namespace AUDIO
{
class CADSPProperties : public DSPObject
{
public:
  CChannelInformation speakerLayout;
  uint64_t            sampleFrequency;
  uint64_t            frameLength;
  ADSPDataFormat_t    dataFormat;

  CADSPProperties& operator=(const CADSPProperties& Props)
  {
    speakerLayout   = Props.speakerLayout;
    sampleFrequency = Props.sampleFrequency;
    frameLength     = Props.frameLength;
    dataFormat      = Props.dataFormat;
    
    return *this;
  }

  bool operator !=(const CADSPProperties& Props)
  {
    return  speakerLayout     != Props.speakerLayout    ||
            sampleFrequency   != Props.sampleFrequency  ||
            frameLength       != Props.frameLength      ||
            dataFormat        != Props.dataFormat;

    return true;
  }

  CADSPProperties() :
    DSPObject("CADSPProperties", ADSP_BASE_ID_PROPERTIES, DSP_CATEGORY_Audio)
  {
    sampleFrequency = 0;
    frameLength     = 0;
    dataFormat      = ADSP_DataFormatINVALID;
  }
};


class IADSPNode : public IDSPNode
{
public:
  IADSPNode(std::string Name, uint64_t ID, ADSPDataFormatFlags_t FormatFlags) :
    IDSPNode(Name, ID, DSP_CATEGORY_Audio),
    FormatFlags(FormatFlags)
  {
    m_DataFormat = AE_FMT_INVALID;
  }

  const ADSPDataFormatFlags_t FormatFlags;

  const inline AEDataFormat &DataFormat()
  {
    return m_DataFormat;
  }

  virtual DSPErrorCode_t Create(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options = nullptr)
  {
    // the default behavoiur is that the node uses the same output parameters as the input parameters
    *OutputProperties = *InputProperties;

    DSPErrorCode_t err = CreateInstance(InputProperties, OutputProperties, Options);

    if (err == DSP_ERR_NO_ERR)
    { // no error occured, copy the parameters into own structures
      m_DataFormat        = InputProperties->m_dataFormat;
      m_InputProperties   = *InputProperties;
      m_OutputProperties  = *OutputProperties;
    }

    return err;
  }

  virtual DSPErrorCode_t Destroy()
  {
    return DestroyInstance();
  }

  virtual DSPErrorCode_t Process(void *In, void *Out)
  {
    DSPErrorCode_t err = DSP_ERR_INVALID_INPUT;

    switch (m_DataFormat)
    {
      case ADSP_DataFormatFloat:
        err = ProcessInstance(reinterpret_cast<float*>(In), reinterpret_cast<float*>(Out));
      break;

      case ADSP_DataFormatLongDouble:
        err = ProcessInstance(reinterpret_cast<double*>(In), reinterpret_cast<double*>(Out));
      break;

      case ADSP_DataFormatDouble:
        err = ProcessInstance(reinterpret_cast<long double*>(In), reinterpret_cast<long double*>(Out));
      break;

        /* planar formats */
      case ADSP_DataFormatFloatPlanes:
        err = ProcessInstance(reinterpret_cast<float**>(In), reinterpret_cast<float**>(Out));
      break;

      case ADSP_DataFormatDoublePlanes:
        err = ProcessInstance(reinterpret_cast<double**>(In), reinterpret_cast<double**>(Out));
      break;

      case ADSP_DataFormatLongDoublePlanes:
        err = ProcessInstance(reinterpret_cast<long double**>(In), reinterpret_cast<long double**>(Out));
      break;

      default:
        err = DSP_ERR_INVALID_DATA_FORMAT;
      break;
    }

    return err;
  }

  const AEAudioFormat& GetInputFormat()  { return m_InputProperties;  }
  const AEAudioFormat& GetOutputFormat() { return m_OutputProperties; }

protected:
  virtual DSPErrorCode_t CreateInstance(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options = nullptr) = 0;
  virtual DSPErrorCode_t DestroyInstance() = 0;
  
  virtual DSPErrorCode_t ProcessInstance(float        *In,  float         *Out)    { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double       *In,  double        *Out)    { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double  *In,  long double   *Out)    { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(float        **In, float         **Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double       **In, double        **Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double  **In, long double   **Out)   { return DSP_ERR_NOT_IMPLEMENTED; }

private:
  AEDataFormat m_DataFormat;
  AEAudioFormat m_InputProperties;
  AEAudioFormat m_OutputProperties;
};
}
}
