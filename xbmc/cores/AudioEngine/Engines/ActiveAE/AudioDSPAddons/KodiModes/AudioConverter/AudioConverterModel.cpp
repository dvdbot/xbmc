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

#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioConverter/AudioConverterModel.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioConverter/AudioDSPConverterFFMPEG.h"

using namespace ActiveAE;


CAudioConverterModel::CAudioConverterModel()
{
  m_stereoupmix = false;
  m_normalizeLevels = false;
  m_resampleQuality = AE_QUALITY_UNKNOWN;
}

CAudioConverterModel::~CAudioConverterModel()
{
}

bool CAudioConverterModel::StereoUpmix()
{
  CSingleLock lock(m_Lock);
  return m_stereoupmix;
}

void CAudioConverterModel::SetStereoUpmix(bool StereoUpmix)
{
  CSingleLock lock(m_Lock);
  m_stereoupmix = StereoUpmix;
}

bool CAudioConverterModel::NormalizeLevels()
{
  CSingleLock lock(m_Lock);
  return m_normalizeLevels;
}

void CAudioConverterModel::SetNormalizeLevels(bool NormalizeLevels)
{
  CSingleLock lock(m_Lock);
  m_normalizeLevels = NormalizeLevels;
}

AEQuality CAudioConverterModel::ResampleQuality()
{
  CSingleLock lock(m_Lock);
  return m_resampleQuality;
}

void CAudioConverterModel::SetResampleQuality(AEQuality ResampleQuality)
{
  CSingleLock lock(m_Lock);
  m_resampleQuality = ResampleQuality;
}

void CAudioConverterModel::Register(CAudioDSPConverterFFMPEG * Converter)
{
  CCriticalSection m_Lock;
}

void CAudioConverterModel::Deregister(CAudioDSPConverterFFMPEG * Converter)
{
  CCriticalSection m_Lock;
}
