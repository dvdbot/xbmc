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

#include "cores/DSP/Processors/Interfaces/IDSPProcessor.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"

namespace DSP
{
namespace AUDIO
{
class IADSPProcessor : public DSP::IDSPProcessor
{
public:
  IADSPProcessor(std::string Name, ADSPDataFormatFlags_t DataFormatFlags) :
    DSP::IDSPProcessor(Name, DSP_CATEGORY_Audio),
    m_DataFormatFlags(DataFormatFlags),
    m_DataFormat(ADSP_DataFormatINVALID)
  {
  }

  virtual DSPErrorCode_t Create(const DSPObject *InParameters, DSPObject *OutParameters, void *Options = nullptr)
  {
    return CreateInstance(reinterpret_cast<const CADSPProperties*>(InParameters), reinterpret_cast<CADSPProperties*>(OutParameters), Options);
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

  virtual DSPErrorCode_t Destroy()
  {
    return DestroyInstance();
  }
  
  const ADSPDataFormatFlags_t m_DataFormatFlags;

protected:
  virtual DSPErrorCode_t CreateInstance(const DSP::AUDIO::CADSPProperties *InputProperties, DSP::AUDIO::CADSPProperties *OutputProperties, void *Options = nullptr) = 0;

  virtual DSPErrorCode_t ProcessInstance(float  *In,  float   *Out)             { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double *In,  double  *Out)             { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double *In, long double  *Out)    { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(float  **In, float   **Out)            { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(double **In, double  **Out)            { return DSP_ERR_NOT_IMPLEMENTED; }
  virtual DSPErrorCode_t ProcessInstance(long double **In, long double  **Out)  { return DSP_ERR_NOT_IMPLEMENTED; }

  virtual DSPErrorCode_t DestroyInstance() = 0;

private:
  ADSPDataFormat_t m_DataFormat;
};
}
}
