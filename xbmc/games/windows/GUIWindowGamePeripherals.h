/*
 *      Copyright (C) 2014 Team XBMC
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
#pragma once

#include "addons/Addon.h"
#include "games/GameTypes.h"
#include "guilib/GUIWindow.h"
#include "input/joysticks/IJoystickInputHandler.h"
#include "FileItem.h"

#include <map>

namespace PERIPHERALS
{
  class CPeripheral;
}

class CGUIWindowGamePeripherals;

class CGUIJoystickInputHandler : public IJoystickInputHandler
{
public:
  CGUIJoystickInputHandler(CGUIWindowGamePeripherals* window, PERIPHERALS::CPeripheral* device, const std::string& strDeviceId);

  virtual ~CGUIJoystickInputHandler(void);

  // Implementation of IJoystickInputHandler
  virtual std::string DeviceID(void) const { return m_strDeviceId; }
  virtual bool OnButtonPress(unsigned int featureIndex, bool bPressed);
  virtual bool OnButtonMotion(unsigned int featureIndex, float magnitude);
  virtual bool OnAnalogStickMotion(unsigned int featureIndex, float x, float y);
  virtual bool OnAccelerometerMotion(unsigned int featureIndex, float x, float y, float z);

private:
  CGUIWindowGamePeripherals* const m_window;
  PERIPHERALS::CPeripheral* const  m_device;
  std::string                      m_strDeviceId;
};

class CGUIWindowGamePeripherals : public CGUIWindow
{
public:
  CGUIWindowGamePeripherals(void);
  virtual ~CGUIWindowGamePeripherals(void) { }

  // implementation of CGUIControl
  virtual bool OnMessage(CGUIMessage& message);
  virtual bool OnAction(const CAction& action);

  // implementation of CGUIWindow
  virtual void OnDeinitWindow(int nextWindowID);

  bool OnButtonPress(PERIPHERALS::CPeripheral* device, unsigned int featureIndex, bool bPressed);
  bool OnButtonMotion(PERIPHERALS::CPeripheral* device, unsigned int featureIndex, float magnitude);
  bool OnAnalogStickMotion(PERIPHERALS::CPeripheral* device, unsigned int featureIndex, float x, float y);
  bool OnAccelerometerMotion(PERIPHERALS::CPeripheral* device, unsigned int featureIndex, float x, float y, float z);

protected:
  GAME::GamePeripheralPtr GetPeripheral(const ADDON::AddonPtr& addon) const;
  GAME::GamePeripheralPtr LoadPeripheral(const ADDON::AddonPtr& addon);

  // implementation of CGUIWindow
  virtual void OnInitWindow(void);

private:
  std::vector<PERIPHERALS::CPeripheral*> ScanPeripherals(void);

  bool OnClick(int iItem);
  bool OnSelect(int iItem);

  int GetSelectedItem(void);

  std::vector<CGUIJoystickInputHandler*> m_inputHandlers;
  GAME::GamePeripheralVector m_peripherals;
  CFileItemList              m_items;
  int                        m_selectedItem;
};
