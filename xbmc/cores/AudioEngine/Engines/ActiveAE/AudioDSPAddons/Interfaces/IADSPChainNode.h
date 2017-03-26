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

namespace DSP
{
namespace AUDIO
{
class IADSPChainNode : public IDSPNode
{
public:
  IADSPChainNode(uint64_t ID, ADSPDataFormatFlags_t FormatFlags, std::string Name) :
    IDSPNode(Name, ID, DSP_CATEGORY_Audio),
    FormatFlags(FormatFlags),
    m_DataFormat(ADSP_DataFormatINVALID)
  {
  }

  const ADSPDataFormatFlags_t FormatFlags;

  inline ADSPDataFormat_t DataFormat()
  {
    return m_DataFormat;
  }

  virtual DSPErrorCode_t Create(const void *InParameters, void *OutParameters, void *Options = nullptr)
  {
    if (!InParameters || !OutParameters)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    // does this node support the requested data format?
    if (!(1 << reinterpret_cast<const ADSP_Properties_t*>(InParameters)->dataFormat & FormatFlags))
    {
      return DSP_ERR_INVALID_DATA_FORMAT;
    }
    // the default behavoiur is that the node uses the same output parameters as the input parameters
    *reinterpret_cast<ADSP_Properties_t*>(OutParameters) = *reinterpret_cast<const ADSP_Properties_t*>(InParameters);

    DSPErrorCode_t err = CreateInstance(reinterpret_cast<const ADSP_Properties_t*>(InParameters), reinterpret_cast<ADSP_Properties_t*>(OutParameters), Options);

    if (err == DSP_ERR_NO_ERR)
    { // no error occured, copy the parameters into own structures
      m_DataFormat = reinterpret_cast<const ADSP_Properties_t*>(InParameters)->dataFormat;
      m_InputProperties = *reinterpret_cast<const ADSP_Properties_t*>(InParameters);
      m_OutputProperties = *reinterpret_cast<ADSP_Properties_t*>(OutParameters);
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

protected:
  virtual DSPErrorCode_t CreateInstance(const ADSP_Properties_t *InParameters, ADSP_Properties_t *OutParameters, void *Options = nullptr) = 0;
  virtual DSPErrorCode_t DestroyInstance() = 0;
  
  virtual DSPErrorCode_t ProcessInstance(float  *In,  float   *Out)             { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double *In,  double  *Out)             { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double *In, long double  *Out)    { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(float  **In, float   **Out)            { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double **In, double  **Out)            { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double **In, long double  **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }

private:
  ADSPDataFormat_t m_DataFormat;
  ADSP_Properties_t m_InputProperties;
  ADSP_Properties_t m_OutputProperties;
};
}
}
