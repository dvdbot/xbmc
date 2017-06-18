#pragma once

/*
 *      Copyright (C) 2005-2014 Team XBMC
 *      http://xbmc.org
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
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "dialogs/GUIDialogSlider.h"
#include "guilib/ISliderCallback.h"

namespace KODI
{
namespace GAME
{
  class CDialogGameAudioSettings : public CGUIDialogSlider,
                                   public ISliderCallback
  {
  public:
    CDialogGameAudioSettings();
    virtual ~CDialogGameAudioSettings() = default;

    // specialization of CGUIWindow via CGUIDialogSlider
    virtual void OnWindowLoaded() override;
    virtual void FrameMove() override;

    // implementation of ISliderCallback
    virtual void OnSliderChange(void *data, CGUISliderControl *slider) override;

  protected:
    // Called when application volume changes
    void SetVolume(float volume);

    // Called when slider value changes
    void OnVolumeChanged(float volume);

  private:
    float m_volume = 1.0f; // Application volume
  };
}
}
