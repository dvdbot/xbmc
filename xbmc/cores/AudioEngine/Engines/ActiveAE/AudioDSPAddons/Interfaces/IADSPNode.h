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
    m_InputDataFormat = AE_FMT_INVALID;
    m_OutputDataFormat = AE_FMT_INVALID;
  }

  const ADSPDataFormatFlags_t FormatFlags;

  virtual DSPErrorCode_t Create(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options = nullptr)
  {
    // the default behavoiur is that the node uses the same output parameters as the input parameters
    *OutputProperties = *InputProperties;

    DSPErrorCode_t err = CreateInstance(InputProperties, OutputProperties, Options);

    if (err == DSP_ERR_NO_ERR)
    { // no error occured, copy the parameters into own structures
      m_InputDataFormat   = InputProperties->m_dataFormat;
      m_OutputDataFormat  = OutputProperties->m_dataFormat;
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

    if (m_InputDataFormat == m_OutputDataFormat)
    {
      switch (m_InputDataFormat)
      {
        //case AE_FMT_LONGDOUBLE:
        //  err = ProcessInstance(reinterpret_cast<long double*>(In), reinterpret_cast<long double*>(Out));
        //break;

        case AE_FMT_DOUBLE:
          err = ProcessInstance(reinterpret_cast<double*>(In), reinterpret_cast<double*>(Out));
        break;

        case AE_FMT_FLOAT:
          err = ProcessInstance(reinterpret_cast<float*>(In), reinterpret_cast<float*>(Out));
        break;

        //  /* planar formats */
        //case AE_FMT_LONGDOUBLEP:
        //  err = ProcessInstance(reinterpret_cast<long double**>(In), reinterpret_cast<long double**>(Out));
        //break;

        case AE_FMT_DOUBLEP:
          err = ProcessInstance(reinterpret_cast<double**>(In), reinterpret_cast<double**>(Out));
        break;

        case AE_FMT_FLOATP:
          err = ProcessInstance(reinterpret_cast<float**>(In), reinterpret_cast<float**>(Out));
        break;

        default:
          err = DSP_ERR_INVALID_DATA_FORMAT;
        break;
      }
    }
    else
    {
      switch (m_InputDataFormat)
      {
        case AE_FMT_U8:
        case AE_FMT_S16BE:
        case AE_FMT_S16LE:
        case AE_FMT_S16NE:
        case AE_FMT_S32BE:
        case AE_FMT_S32LE:
        case AE_FMT_S32NE:
        case AE_FMT_S24BE4:
        case AE_FMT_S24LE4:
        case AE_FMT_S24NE4:     // 24 bits in lower 3 bytes
        case AE_FMT_S24NE4MSB:  // S32 with bits_per_sample < 32
        case AE_FMT_S24BE3:
        case AE_FMT_S24LE3:
        case AE_FMT_S24NE3:     // S24 in 3 bytes */
          switch (m_OutputDataFormat)
          {
            //case AE_FMT_LONGDOUBLE:
            //  err = ProcessInstance(reinterpret_cast<long double*>(In), reinterpret_cast<long double*>(Out));
            //break;

            case AE_FMT_DOUBLE:
              err = ProcessInstance(reinterpret_cast<uint8_t*>(In), reinterpret_cast<double*>(Out));
            break;

            case AE_FMT_FLOAT:
              err = ProcessInstance(reinterpret_cast<uint8_t*>(In), reinterpret_cast<float*>(Out));
            break;

            default:
              err = DSP_ERR_INVALID_DATA_FORMAT;
            break;
          }
        break;

        /* planar formats */
        case AE_FMT_U8P:
        case AE_FMT_S16NEP:
        case AE_FMT_S32NEP:
        case AE_FMT_S24NE4P:
        case AE_FMT_S24NE4MSBP:
        case AE_FMT_S24NE3P:
          switch (m_OutputDataFormat)
          {
            //case AE_FMT_LONGDOUBLE:
            //  err = ProcessInstance(reinterpret_cast<uint8_t**>(In), reinterpret_cast<long double**>(Out));
            //break;

            case AE_FMT_DOUBLEP:
              err = ProcessInstance(reinterpret_cast<uint8_t**>(In), reinterpret_cast<double**>(Out));
            break;

            case AE_FMT_FLOATP:
              err = ProcessInstance(reinterpret_cast<uint8_t**>(In), reinterpret_cast<float**>(Out));
            break;

            default:
              err = DSP_ERR_INVALID_DATA_FORMAT;
            break;
          }
        break;

        default:
          err = DSP_ERR_INVALID_DATA_FORMAT;
        break;
      }

      switch (m_OutputDataFormat)
      {
        case AE_FMT_U8:
        case AE_FMT_S16BE:
        case AE_FMT_S16LE:
        case AE_FMT_S16NE:
        case AE_FMT_S32BE:
        case AE_FMT_S32LE:
        case AE_FMT_S32NE:
        case AE_FMT_S24BE4:
        case AE_FMT_S24LE4:
        case AE_FMT_S24NE4:    // 24 bits in lower 3 bytes
        case AE_FMT_S24NE4MSB: // S32 with bits_per_sample < 32
        case AE_FMT_S24BE3:
        case AE_FMT_S24LE3:
        case AE_FMT_S24NE3: /* S24 in 3 bytes */
          switch (m_InputDataFormat)
          {
            //case AE_FMT_LONGDOUBLE:
            //  err = ProcessInstance(reinterpret_cast<long double*>(In), reinterpret_cast<uint8_t*>(Out));
            //break;

            case AE_FMT_DOUBLE:
              err = ProcessInstance(reinterpret_cast<double*>(In), reinterpret_cast<uint8_t*>(Out));
            break;

            case AE_FMT_FLOAT:
              err = ProcessInstance(reinterpret_cast<float*>(In), reinterpret_cast<uint8_t*>(Out));
            break;

            default:
              err = DSP_ERR_INVALID_DATA_FORMAT;
            break;
          }
        break;

        /* planar formats */
        case AE_FMT_U8P:
        case AE_FMT_S16NEP:
        case AE_FMT_S32NEP:
        case AE_FMT_S24NE4P:
        case AE_FMT_S24NE4MSBP:
        case AE_FMT_S24NE3P:
          switch (m_InputDataFormat)
          {
            //case AE_FMT_LONGDOUBLE:
            //  err = ProcessInstance(reinterpret_cast<long double**>(In), reinterpret_cast<uint8_t**>(Out));
            //break;

            case AE_FMT_DOUBLEP:
              err = ProcessInstance(reinterpret_cast<double**>(In), reinterpret_cast<uint8_t**>(Out));
            break;

            case AE_FMT_FLOATP:
              err = ProcessInstance(reinterpret_cast<float**>(In), reinterpret_cast<uint8_t**>(Out));
            break;

            default:
              err = DSP_ERR_INVALID_DATA_FORMAT;
            break;
          }
        break;

        default:
          err = DSP_ERR_INVALID_DATA_FORMAT;
        break;
      }
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

  // input: fixed point, output: floating point
  virtual DSPErrorCode_t ProcessInstance(uint8_t      *In,  float         *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(uint8_t      *In,  double        *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(uint8_t      *In,  long double   *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(uint8_t      **In, float         **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(uint8_t      **In, double        **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(uint8_t      **In, long double   **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }

  // input: floating point, output: fixed point
  virtual DSPErrorCode_t ProcessInstance(float        *In,  uint8_t       *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double       *In,  uint8_t       *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double  *In,  uint8_t       *Out)   { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(float        **In, uint8_t       **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double       **In, uint8_t       **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double  **In, uint8_t       **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }

private:
  AEDataFormat m_InputDataFormat;
  AEDataFormat m_OutputDataFormat;
  AEAudioFormat m_InputProperties;
  AEAudioFormat m_OutputProperties;
};
}
}
