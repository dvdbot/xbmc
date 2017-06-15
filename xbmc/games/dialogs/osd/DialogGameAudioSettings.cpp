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
#include "cores/AudioEngine/Utils/AEUtil.h"
#include "dialogs/GUIDialogYesNo.h"
#include "guilib/GUIWindowManager.h"
#include "guilib/LocalizeStrings.h"
#include "guilib/WindowIDs.h"
#include "profiles/ProfilesManager.h"
#include "settings/lib/Setting.h"
#include "settings/lib/SettingsManager.h"
#include "settings/MediaSettings.h"
#include "settings/Settings.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "Application.h"
#include "ServiceBroker.h"

using namespace KODI;
using namespace GAME;

// Audio settings
#define SETTING_GAME_VOLUME                   "game.volume"
#define SETTING_GAME_DSP                      "game.dsp"

// Audio actions
#define SETTING_GAME_MAKE_DEFAULT             "game.makedefault"

CDialogGameAudioSettings::CDialogGameAudioSettings() :
  CGUIDialogSettingsManualBase(WINDOW_DIALOG_GAME_AUDIO_SETTINGS, "DialogSettings.xml")
{
}

void CDialogGameAudioSettings::FrameMove()
{
  // Update the volume setting if necessary
  float newVolume = g_application.GetVolume(false);
  if (newVolume != m_volume)
    GetSettingsManager()->SetNumber(SETTING_GAME_VOLUME, newVolume);

  CGUIDialogSettingsManualBase::FrameMove();
}

void CDialogGameAudioSettings::OnSettingChanged(std::shared_ptr<const CSetting> setting)
{
  if (!setting)
    return;

  CGUIDialogSettingsManualBase::OnSettingChanged(setting);

  const std::string &settingId = setting->GetId();
  if (settingId == SETTING_GAME_VOLUME)
  {
    m_volume = static_cast<float>(std::static_pointer_cast<const CSettingNumber>(setting)->GetValue());
    g_application.SetVolume(m_volume, false); // false - value is not in percent
  }
}

void CDialogGameAudioSettings::OnSettingAction(std::shared_ptr<const CSetting> setting)
{
  if (!setting)
    return;

  CGUIDialogSettingsManualBase::OnSettingAction(setting);

  const std::string &settingId = setting->GetId();
  if (settingId == SETTING_GAME_DSP)
  {
    g_windowManager.ActivateWindow(WINDOW_DIALOG_AUDIO_DSP_OSD_SETTINGS);
  }
  else if (settingId == SETTING_GAME_MAKE_DEFAULT)
    Save();
}

void CDialogGameAudioSettings::Save()
{
  if (!g_passwordManager.CheckSettingLevelLock(SettingLevel::Expert) &&
      CProfilesManager::GetInstance().GetMasterProfile().getLockMode() != LOCK_MODE_EVERYONE)
    return;

  // Prompt user if they are sure
  // "Set as default for all media"
  // "This will reset any previously saved values. Are you sure?"
  if (!CGUIDialogYesNo::ShowAndGetInput(CVariant{12376}, CVariant{12377}))
    return;

  CMediaSettings::GetInstance().GetDefaultGameSettings() = CMediaSettings::GetInstance().GetCurrentGameSettings();
  CServiceBroker::GetSettings().Save();
}

void CDialogGameAudioSettings::SetupView()
{
  CGUIDialogSettingsManualBase::SetupView();

  SetHeading(35224); // "Audio settings"
  SET_CONTROL_HIDDEN(CONTROL_SETTINGS_OKAY_BUTTON);
  SET_CONTROL_HIDDEN(CONTROL_SETTINGS_CUSTOM_BUTTON);
  SET_CONTROL_LABEL(CONTROL_SETTINGS_CANCEL_BUTTON, 15067); // "Close"
}

void CDialogGameAudioSettings::InitializeSettings()
{
  CGUIDialogSettingsManualBase::InitializeSettings();

  const std::shared_ptr<CSettingCategory> category = AddCategory("gameaudiosettings", -1);
  if (!category)
  {
    CLog::Log(LOGERROR, "CDialogGameAudioSettings: unable to setup settings");
    return;
  }

  // Get all necessary setting groups
  const std::shared_ptr<CSettingGroup> groupAudio = AddGroup(category);
  if (!groupAudio)
  {
    CLog::Log(LOGERROR, "CDialogGameAudioSettings: unable to setup settings");
    return;
  }
  const std::shared_ptr<CSettingGroup> groupActions = AddGroup(category);
  if (!groupActions)
  {
    CLog::Log(LOGERROR, "CDialogGameAudioSettings: unable to setup settings");
    return;
  }

  CGameSettings &gameSettings = CMediaSettings::GetInstance().GetCurrentGameSettings();

  m_dspEnabled = CServiceBroker::GetSettings().GetBool(CSettings::SETTING_AUDIOOUTPUT_DSPADDONSENABLED);

  m_volume = g_application.GetVolume(false);
  std::shared_ptr<CSettingNumber> settingAudioVolume = AddSlider(groupAudio, SETTING_GAME_VOLUME, 13376, SettingLevel::Basic, m_volume, 14054, VOLUME_MINIMUM, VOLUME_MAXIMUM / 100.0f, VOLUME_MAXIMUM);
  std::static_pointer_cast<CSettingControlSlider>(settingAudioVolume->GetControl())->SetFormatter(SettingFormatterPercentAsDecibel);

  if (m_dspEnabled)
    AddButton(groupAudio, SETTING_GAME_DSP, 24136, SettingLevel::Basic);
}

std::string CDialogGameAudioSettings::SettingFormatterPercentAsDecibel(std::shared_ptr<const CSettingControlSlider> control, const CVariant &value, const CVariant &minimum, const CVariant &step, const CVariant &maximum)
{
  if (!control || !value.isDouble())
    return "";

  std::string formatString = control->GetFormatString();
  if (control->GetFormatLabel() > -1)
    formatString = g_localizeStrings.Get(control->GetFormatLabel());

  return StringUtils::Format(formatString.c_str(), CAEUtil::PercentToGain(value.asFloat()));
}
