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

#include <stdint.h>

#include "DSPChain/Typedefs/DSPTypedefs.h"
#include "DSPChain/Models/Interfaces/IDSPChainModel.h"


namespace DSPChain
{
class CDSPChainBasicModel : public IDSPChainNodeModel
{
public:
  virtual DSPErrorCode_t RegisterValue(int32_t ID, const std::string &Name, size_t Size)
  {
    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t DeregisterValue(int32_t ID)
  {
    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t DeregisterValue(const std::string &Name)
  {
    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t Set(int32_t ID, uint8_t *Data, size_t Size)
  {
    return DSP_ERR_NO_ERR;
  }

  virtual DSPErrorCode_t Get(int32_t ID, uint8_t *Data, size_t Size)
  {
    return DSP_ERR_NO_ERR;
  }
};
}