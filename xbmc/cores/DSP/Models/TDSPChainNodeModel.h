#pragma
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

#include <stdint.h>
#include <string>
#include "DSPChain/Models/Interfaces/IDSPChainNodeModel.h"


namespace DSPChain
{
template<typename TID>
class TModelAccessorHelper
{
public:
  TModelAccessorHelper()
  {
    m_Model = nullptr;
  }

  DSPErrorCode_t Create(IDSPChainNodeModel *Model)
  {
    if (!Model)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    m_Model = Model;

    return DSP_ERR_NO_ERR;
  }

  template<class T>
  DSPErrorCode_t RegisterParameter(TID ID, const std::string &Name, const T &Val)
  {
    if (!m_Model)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    return m_Model->RegisterValue(ID, Name, sizeof(T));
  }

  DSPErrorCode_t RegisterParameter(TID ID, const std::string &Name, size_t Size)
  {
    if (!m_Model)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    return m_Model->RegisterValue(ID, Name, Size);
  }

  template<class T>
  DSPErrorCode_t Set(TID ID, T &Val)
  {
    if (!m_Model)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    return m_Model->Set(ID, reinterpret_cast<uint8_t*>(&Val), sizeof(T));
  }

  template<class T>
  DSPErrorCode_t Get(TID ID, T &Val)
  {
    if (!m_Model)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    return m_Model->Get(ID, reinterpret_cast<uint8_t*>(&Val), sizeof(T));
  }

  template<class T>
  DSPErrorCode_t Set(TID ID, T &Val, size_t Size)
  {
    return m_Model->Set(ID, reinterpret_cast<uint8_t*>(&Val), Size);
  }

  template<class T>
  DSPErrorCode_t Get(TID ID, T &Val, size_t Size)
  {
    if (!m_Model)
    {
      return DSP_ERR_FATAL_ERROR;
    }

    return m_Model->Get(ID, reinterpret_cast<uint8_t*>(&Val), Size);
  }

private:
  IDSPChainNodeModel *m_Model;
};
}