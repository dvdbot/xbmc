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

#include "DialogGameOSD.h"
#include "guilib/GUIMessage.h"
#include "guilib/GUIWindowManager.h"
#include "input/Action.h"
#include "input/ActionIDs.h"

//! @todo Move reset code
#include "utils/StringUtils.h"
#include "Util.h"

using namespace KODI;
using namespace GAME;

CDialogGameOSD::CDialogGameOSD() :
  CGUIDialog(WINDOW_DIALOG_GAME_OSD, "GameOSD.xml")
{
  // Initialize CGUIWindow
  m_loadType = KEEP_IN_MEMORY;
}

bool CDialogGameOSD::OnAction(const CAction &action)
{
  switch (action.GetID())
  {
  case ACTION_SHOW_OSD:
  {
    Close();
    return true;
  }
  case ACTION_PLAY:
  case ACTION_PREV_ITEM:
  case ACTION_STOP:
  case ACTION_PLAYER_RESET:
  {
    Close();
    break;
  }
  //! @todo Move this to input handler
  // Intercept game reset commands so that we can tell which player hit Reset
  case ACTION_BUILT_IN_FUNCTION:
  {
    std::string builtInFunction;
    std::vector<std::string> params;
    if (action.GetBuiltInFunction(builtInFunction, params, true))
    {
      if (builtInFunction == "playercontrol" && !params.empty())
      {
        if (params[0] == "reset")
        {
          //! @todo Handle reset
          return true;
        }
      }
    }
  }
  default:
    break;
  }

  return CGUIDialog::OnAction(action);
}

bool CDialogGameOSD::OnMessage(CGUIMessage& message)
{
  switch (message.GetMessage())
  {
  case GUI_MSG_WINDOW_DEINIT:  // Fired when OSD is hidden
  {
    // Remove our subdialogs if visible
    CGUIDialog *pDialog = g_windowManager.GetDialog(WINDOW_DIALOG_GAME_VIDEO_SETTINGS);
    if (pDialog && pDialog->IsDialogRunning())
      pDialog->Close(true);

    pDialog = g_windowManager.GetDialog(WINDOW_DIALOG_GAME_AUDIO_SETTINGS);
    if (pDialog && pDialog->IsDialogRunning())
      pDialog->Close(true);

    pDialog = g_windowManager.GetDialog(WINDOW_DIALOG_GAME_CONTROLLERS);
    if (pDialog && pDialog->IsDialogRunning())
      pDialog->Close(true);

    break;
  }
  default:
    break;
  }

  return CGUIDialog::OnMessage(message);
}
