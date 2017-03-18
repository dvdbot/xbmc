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

#include "DSPChain/Nodes/Interfaces/IADSPChainNode.h"

#include <stdint.h>


namespace DSPChain
{
class CADSPBuffer
{
public:
  CADSPBuffer()
  {
    m_InputBuffer = nullptr;
    m_OutputBuffer = nullptr;
    m_PingBuffer = nullptr;
    m_PongBuffer = nullptr;
    m_CurrentOutputBuffer = nullptr;

    m_InputSampleFormat = ADSP_DataFormatINVALID;
    m_OutputSampleFormat = ADSP_DataFormatINVALID;

    m_InputFrameSize = 0;
    m_OutputFrameSize = 0;
    m_BufferChannelAmount = 2;
  }

  ~CADSPBuffer()
  {
    Destroy();
  }

  DSPErrorCode_t Create(uint64_t InputFrameSize, uint64_t OutputFrameSize)
  {
    if (InputFrameSize == 0 || OutputFrameSize == 0)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    m_InputFrameSize = InputFrameSize;
    m_OutputFrameSize = OutputFrameSize;

    m_InputBuffer = new uint8_t[(size_t)(sizeof(float)*m_InputFrameSize*m_BufferChannelAmount)];
    m_OutputBuffer = new uint8_t[(size_t)(sizeof(float)*m_OutputFrameSize*m_BufferChannelAmount*2)];
    if (!m_InputBuffer || !m_OutputBuffer)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    memset(m_InputBuffer, 0, (size_t)(sizeof(float)*m_InputFrameSize*m_BufferChannelAmount));
    memset(m_OutputBuffer, 0, (size_t)(sizeof(float)*m_OutputFrameSize*m_BufferChannelAmount*2));

    m_PingBuffer = m_OutputBuffer;
    m_PongBuffer = m_OutputBuffer + sizeof(float)*m_OutputFrameSize*m_BufferChannelAmount;
    m_CurrentOutputBuffer = m_PingBuffer;

    return DSP_ERR_NO_ERR;
  }

  inline void* GetInputBuffer()
  {
    return m_InputBuffer;
  }

  DSPErrorCode_t PrepareOutput()
  {
    return DSP_ERR_NO_ERR;
  }

  inline void* GetOutputBuffer()
  {
    return m_CurrentOutputBuffer;
  }

  DSPErrorCode_t Destroy()
  {
    delete m_InputBuffer;
    delete m_OutputBuffer;

    m_InputBuffer = nullptr;
    m_OutputBuffer = nullptr;
    m_PingBuffer = nullptr;
    m_PongBuffer = nullptr;
    m_CurrentOutputBuffer = nullptr;

    m_InputSampleFormat = ADSP_DataFormatINVALID;
    m_OutputSampleFormat = ADSP_DataFormatINVALID;

    m_InputFrameSize = 0;
    m_OutputFrameSize = 0;
    m_BufferChannelAmount = 0;

    return DSP_ERR_NO_ERR;
  }

private:
  uint8_t *m_InputBuffer;
  uint8_t *m_OutputBuffer;
  uint8_t *m_PingBuffer;
  uint8_t *m_PongBuffer;
  uint8_t *m_CurrentOutputBuffer;

  ADSPDataFormat_t m_InputSampleFormat;
  ADSPDataFormat_t m_OutputSampleFormat;

  uint64_t m_AvailableInputSamples;
  uint64_t m_AvailableOutputSamples;

  uint64_t m_InputFrameSize;
  uint64_t m_OutputFrameSize;
  uint64_t m_BufferChannelAmount;
};
}
