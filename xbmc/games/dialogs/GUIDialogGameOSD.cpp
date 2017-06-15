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

#include "GUIDialogGameOSD.h"
#include "guilib/GUIMessage.h"
#include "input/Action.h"
#include "input/ActionIDs.h"

using namespace KODI;
using namespace GAME;

CGUIDialogGameOSD::CGUIDialogGameOSD() :
  CGUIDialog(WINDOW_DIALOG_GAME_OSD, "GameOSD.xml")
{
  m_loadType = KEEP_IN_MEMORY;
}

bool CGUIDialogGameOSD::OnAction(const CAction &action)
{
  if (action.GetID() == ACTION_SHOW_OSD)
  {
    Close();
    return true;
  }

  return CGUIDialog::OnAction(action);
}

bool CGUIDialogGameOSD::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_DEINIT:  // Fired when OSD is hidden
    {
      // Remove our subdialogs if visible
      /*
      CGUIDialog *pDialog = g_windowManager.GetDialog(WINDOW_DIALOG_AUDIO_DSP_OSD_SETTINGS);
      if (pDialog && pDialog->IsDialogRunning())
        pDialog->Close(true);
      pDialog = g_windowManager.GetDialog(WINDOW_DIALOG_AUDIO_OSD_SETTINGS);
      if (pDialog && pDialog->IsDialogRunning())
        pDialog->Close(true);
      */
    }
    break;
  }
  return CGUIDialog::OnMessage(message);
}
