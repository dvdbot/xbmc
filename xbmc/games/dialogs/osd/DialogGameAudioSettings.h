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

#include "settings/dialogs/GUIDialogSettingsManualBase.h"

#include <memory>
#include <string>
#include <vector>

class CVariant;

namespace KODI
{
namespace GAME
{
  class CDialogGameAudioSettings : public CGUIDialogSettingsManualBase
  {
  public:
    CDialogGameAudioSettings();
    virtual ~CDialogGameAudioSettings() = default;

    // specialization of CGUIWindow via CGUIDialogSettingsManualBase
    virtual void FrameMove();

  protected:
    // implementations of ISettingCallback via CGUIDialogSettingsManualBase
    virtual void OnSettingChanged(std::shared_ptr<const CSetting> setting);
    virtual void OnSettingAction(std::shared_ptr<const CSetting> setting);

    // specialization of CGUIDialogSettingsBase via CGUIDialogSettingsManualBase
    virtual bool AllowResettingSettings() const { return false; }
    virtual void Save();
    virtual void SetupView();

    // specialization of CGUIDialogSettingsManualBase
    virtual void InitializeSettings();

    // Utility function
    static std::string SettingFormatterPercentAsDecibel(std::shared_ptr<const CSettingControlSlider> control, const CVariant &value, const CVariant &minimum, const CVariant &step, const CVariant &maximum);

    // External parameters
    float m_volume = 100.0f; // Application volume
    bool m_dspEnabled = false; // Value of "AudioDSP enabled" setting
  };
}
}
