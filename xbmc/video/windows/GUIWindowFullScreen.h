#pragma once

/*
 *      Copyright (C) 2005-2013 Team XBMC
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

#include "guilib/GUIWindow.h"

class CGUIWindowFullScreen : public CGUIWindow
{
public:
  CGUIWindowFullScreen(void);
  virtual ~CGUIWindowFullScreen(void);
  virtual bool OnMessage(CGUIMessage& message);
  virtual bool OnAction(const CAction &action);
  virtual void ClearBackground();
  virtual void FrameMove();
  virtual void Process(unsigned int currentTime, CDirtyRegionList &dirtyregion);
  virtual void Render();
  virtual void RenderEx();
  virtual void OnWindowLoaded();
  virtual bool HasVisibleControls();

protected:
  virtual EVENT_RESULT OnMouseEvent(const CPoint &point, const CMouseEvent &event);

private:
  void SeekChapter(int iChapter);
  void ToggleOSD();
  void TriggerOSD();
  int GetOSDWindowID();

  bool m_viewModeChanged;
  unsigned int m_dwShowViewModeTimeout;

  bool m_bShowCurrentTime;
};
