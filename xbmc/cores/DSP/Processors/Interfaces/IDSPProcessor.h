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

#include "cores/DSP/Typedefs/DSPTypedefs.h"
#include <string>

namespace DSP
{
class IDSPProcessor
{
public:
  IDSPProcessor(std::string Name, DSPCategory_t Category) :
    Name(Name),
    Category(Category)
  {
  }

  const std::string Name;
  const DSPCategory_t Category;

  virtual DSPErrorCode_t Create(const void *InParameters, void *OutParameters, void *Options = nullptr) = 0;
  virtual DSPErrorCode_t Destroy() = 0;
  virtual DSPErrorCode_t Process(void *In, void *Out) = 0;
  virtual DSPErrorCode_t UpdateNodes() = 0;
};
}
