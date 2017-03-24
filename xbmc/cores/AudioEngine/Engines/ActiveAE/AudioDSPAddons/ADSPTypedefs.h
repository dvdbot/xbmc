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

typedef enum
{
  ADSP_DataFormatINVALID = -1,

  ADSP_DataFormatFloat,
  ADSP_DataFormatDouble,
  ADSP_DataFormatLongDouble,

  /* planar formats */
  ADSP_DataFormatFloatPlanes,
  ADSP_DataFormatDoublePlanes,
  ADSP_DataFormatLongDoublePlanes,

  ADSP_DataFormatMAX
}ADSPDataFormat_t;

typedef enum
{
  ADSP_DataFormatFlagINVALID = 0,

  ADSP_DataFormatFlagFloat            = 1 << ADSP_DataFormatFloat,
  ADSP_DataFormatFlagDouble           = 1 << ADSP_DataFormatDouble,
  ADSP_DataFormatFlagLongDouble       = 1 << ADSP_DataFormatLongDouble,

  /* planar formats */
  ADSP_DataFormatFlagFloatPlanes      = 1 << ADSP_DataFormatFloatPlanes,
  ADSP_DataFormatFlagDoublePlanes     = 1 << ADSP_DataFormatDoublePlanes,
  ADSP_DataFormatFlagLongDoublePlanes = 1 << ADSP_DataFormatLongDoublePlanes,

  ADSP_DataFormatFlagMAX
}ADSPDataFormatFlags_t;
}
}
