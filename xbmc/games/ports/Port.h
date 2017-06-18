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
#pragma once

#include "input/joysticks/IInputHandler.h"

#include <memory>

namespace PERIPHERALS
{
  class CPeripheral;
}

namespace KODI
{
namespace JOYSTICK
{
  class IInputProvider;
}

namespace GAME
{
  class CGameClient;

  class CPort : public JOYSTICK::IInputHandler
  {
  public:
    CPort(JOYSTICK::IInputHandler* gameInput, CGameClient& gameClient);
    ~CPort();

    void RegisterInput(JOYSTICK::IInputProvider *provider);
    void UnregisterInput(JOYSTICK::IInputProvider *provider);

    JOYSTICK::IInputHandler *InputHandler() { return m_gameInput; }

    // Implementation of IInputHandler
    virtual std::string ControllerID() const override;
    virtual bool HasFeature(const std::string& feature) const override { return true; }
    virtual bool AcceptsInput() override { return true; }
    virtual JOYSTICK::INPUT_TYPE GetInputType(const std::string& feature) const override;
    virtual unsigned int GetDelayMs(const std::string& feature) const { return 0; }
    virtual bool OnButtonPress(const std::string& feature, bool bPressed) override;
    virtual void OnButtonHold(const std::string& feature, unsigned int holdTimeMs) override;
    virtual bool OnButtonMotion(const std::string& feature, float magnitude, unsigned int motionTimeMs) override;
    virtual bool OnAnalogStickMotion(const std::string& feature, float x, float y, unsigned int motionTimeMs = 0) override;
    virtual bool OnAccelerometerMotion(const std::string& feature, float x, float y, float z) override;

  private:
    // Construction parameter
    JOYSTICK::IInputHandler* const m_gameInput;

    // Handles input to Kodi
    std::unique_ptr<JOYSTICK::IInputHandler> m_appInput;

    // Prevents input falling through to Kodi when not handled by the game
    std::unique_ptr<JOYSTICK::IInputHandler> m_inputSink;
  };
}
}
