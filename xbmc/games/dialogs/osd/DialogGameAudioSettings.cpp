/*
 *      Copyright (C) 2017 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "DialogGameAudioSettings.h"
#include "guilib/GUISliderControl.h"
#include "guilib/LocalizeStrings.h"
#include "guilib/WindowIDs.h"
#include "Application.h"

using namespace KODI;
using namespace GAME;

CDialogGameAudioSettings::CDialogGameAudioSettings()
{
  CGUIDialogSlider::SetID(WINDOW_DIALOG_GAME_AUDIO_SETTINGS);
}

void CDialogGameAudioSettings::OnInitWindow()
{
  CGUIDialogSlider::OnInitWindow();

  m_volume = g_application.GetVolume(false);

  const std::string label = g_localizeStrings.Get(13376); // "Volume"
  const float value = m_volume;
  const float min = VOLUME_MINIMUM;
  const float delta = (VOLUME_MAXIMUM - VOLUME_MINIMUM) / 20.0f;
  const float max = VOLUME_MAXIMUM;

  SetSlider(label, value, min, delta, max, this, nullptr);

  SetModalityType(DialogModalityType::MODAL);
}

void CDialogGameAudioSettings::FrameMove()
{
  // Update the volume slider if necessary
  float newVolume = g_application.GetVolume(false);
  if (newVolume != m_volume)
    SetVolume(newVolume);

  CGUIDialogSlider::FrameMove();
}

void CDialogGameAudioSettings::OnSliderChange(void *data, CGUISliderControl *slider)
{
  if (slider == nullptr)
    return;

  OnVolumeChanged(slider->GetFloatValue());
}

void CDialogGameAudioSettings::SetVolume(float volume)
{
  m_volume = volume;
  SetSliderValue(m_volume);
}

void CDialogGameAudioSettings::OnVolumeChanged(float volume)
{
  m_volume = volume;
  g_application.SetVolume(m_volume, false); // false - value is not in percent
}
