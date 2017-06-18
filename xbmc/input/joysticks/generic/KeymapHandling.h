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

#include "input/joysticks/JoystickTypes.h"
#include "input/joysticks/IActionMap.h"
#include "input/joysticks/IInputHandler.h"
#include "input/joysticks/IKeymapHandler.h"
#include "input/Action.h"

#include "input/joysticks/IInputProvider.h" //! @todo

#include <memory>
#include <vector>

#define ANALOG_DIGITAL_THRESHOLD  0.5f

namespace KODI
{
namespace JOYSTICK
{
  // --- CKeymapUtils ----------------------------------------------------------

  class CKeymapUtils
  {
  public:
    static const std::vector<ANALOG_STICK_DIRECTION>& GetDirections(void);
  };

  // --- CHotkey ---------------------------------------------------------------

  class CHotkey
  {
  public:
    void OnButtonPress(bool bPressed);
    void OnButtonMotion(float magnitude);

    virtual bool IsPressed() const { return m_bPressed; }

  private:
    bool m_bPressed = false;
  };

  // --- CKeymapButton ---------------------------------------------------------

  class IActionHandler;
  class IInputProvider;

  class CKeymapButton : public IKeymapHandler
  {
  protected:
    CKeymapButton(CAction action, IActionHandler *actionHandler);

  public:
    virtual ~CKeymapButton() = default;

    //! @todo Move me
    static std::unique_ptr<IKeymapHandler> CreateButton(std::string actionString, IActionHandler *actionHandler);

    // partial implementation of IKeymapHandler
    virtual unsigned int GetActionID() const override;

  protected:
    CAction m_action;
    IActionHandler *m_actionHandler;
  };

  // --- CAnalogButton ---------------------------------------------------------

  class CAnalogButton : public CKeymapButton
  {
  public:
    CAnalogButton(CAction action, IActionHandler *actionHandler);

    virtual ~CAnalogButton() = default;

    // partial implementation of IKeymapHandler via CKeymapButton
    virtual INPUT_TYPE GetInputType() const override { return INPUT_TYPE::ANALOG; }
    virtual void OnDigitalKey(bool bPressed, unsigned int holdTimeMs) override;
    virtual void OnAnalogKey(float magnitude, unsigned int motionTimeMs) override;
  };

  // --- CDigitalButton --------------------------------------------------------

  class CDigitalButton : public CKeymapButton
  {
  public:
    CDigitalButton(CAction action, IActionHandler *actionHandler);

    virtual ~CDigitalButton() = default;

    // partial implementation of IKeymapHandler via CKeymapButton
    virtual INPUT_TYPE GetInputType() const override { return INPUT_TYPE::DIGITAL; }
    virtual void OnDigitalKey(bool bPressed, unsigned int holdTimeMs) override;
    virtual void OnAnalogKey(float magnitude, unsigned int motionTimeMs) override;

  private:
    bool m_bPressed = false;
    unsigned int m_lastDigitalActionMs = 0;
  };

  // --- COverloadedButton -----------------------------------------------------

  class COverloadedButton : public IKeymapHandler
  {
  public:
    COverloadedButton() = default;
    virtual ~COverloadedButton() = default;

    void RegisterAction(unsigned int holdtimeMs, std::shared_ptr<const CHotkey> hotkey, std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> handler);
    void Clear();

    // implementation of IKeymapHandler
    virtual INPUT_TYPE GetInputType() const override;
    virtual unsigned int GetActionID() const override;
    virtual void OnDigitalKey(bool bPressed, unsigned int holdTimeMs) override;
    virtual void OnAnalogKey(float magnitude, unsigned int motionTimeMs) override;

  private:
    using HoldTimeMS = unsigned int;
    struct KeymapEntry
    {
      std::shared_ptr<const CHotkey> hotkey;
      std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> handler;
    };

    std::map<HoldTimeMS, KeymapEntry> m_actions;
  };

  // --- CJoystickKeymap -------------------------------------------------------

  class CJoystickKeymap : public IInputHandler
  {
  public:
    CJoystickKeymap(std::string controllerId);

    virtual ~CJoystickKeymap() = default;

    void AddHandler(const FeatureName& feature, ANALOG_STICK_DIRECTION dir, unsigned int holdtimeMs, const std::string &hotkey, std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> handler);
    void Clear();

    // implementation of IInputHandler
    virtual std::string ControllerID(void) const override { return m_controllerId; }
    virtual bool HasFeature(const FeatureName& feature) const override;
    virtual bool AcceptsInput(void) override { return true; } //! @todo
    virtual INPUT_TYPE GetInputType(const FeatureName& feature) const override;
    virtual bool OnButtonPress(const FeatureName& feature, bool bPressed) override;
    virtual void OnButtonHold(const FeatureName& feature, unsigned int holdTimeMs) override;
    virtual bool OnButtonMotion(const FeatureName& feature, float magnitude, unsigned int motionTimeMs) override;
    virtual bool OnAnalogStickMotion(const FeatureName& feature, float x, float y, unsigned int motionTimeMs = 0) override;
    virtual bool OnAccelerometerMotion(const FeatureName& feature, float x, float y, float z) override;

    /*
    // implementation of IActionMap
    virtual unsigned int GetActionID(const FeatureName& feature) override;
    */

  private:
    bool ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs);
    void DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir);

    struct KeymapFeature
    {
      KeymapFeature(std::string feature, ANALOG_STICK_DIRECTION dir = ANALOG_STICK_DIRECTION::UNKNOWN);

      std::string feature;
      ANALOG_STICK_DIRECTION dir;

      bool operator<(const KeymapFeature &other) const
      {
        if (feature < other.feature) return true;
        if (feature > other.feature) return false;

        return dir < other.dir;
      }
    };

    bool HasButton(const KeymapFeature& feature) const;
    COverloadedButton &GetButton(const KeymapFeature& feature);
    const COverloadedButton &GetButton(const KeymapFeature& feature) const;

    // Construction parameter
    std::string m_controllerId;

    std::map<FeatureName, std::shared_ptr<CHotkey>> m_hotkeys;

    std::map<KeymapFeature, COverloadedButton> m_keymap;
  };

  /*!
   * \ingroup joystick
   * \brief
   *
  class CKeymapHandling
  {
  public:
    CKeymapHandling(IInputProvider *inputProvider, bool pPromiscuous) :
      m_inputProvider(inputProvider)
    {
      std::vector<std::string> controllersIds = CButtonTranslator::GetControllers();
      for (const auto &controllerId : controllersIds)
      {
        std::unique_ptr<IInputHandler> inputHandler(new CJoystickKeymap(controllerId));
        m_inputProvider->RegisterInputHandler(inputHandler.get(), pPromiscuous);
        m_inputHandlers.emplace_back(std::move(inputHandler));
      }
    }

    virtual ~CKeymapHandling()
    {
      for (const auto &inputHandler : m_inputHandlers)
        m_inputProvider->UnregisterInputHandler(inputHandler.get());
    }

  private:

    std::vector<std::unique_ptr<IInputHandler>> m_inputHandlers;

    void Register(const std::string& controllerId, std::unique_ptr<IInputHandler> handler)
    {
      auto it = m_joystickInput.find(controllerId);

      CJoystickKeymap &keymap = m_joystickInput[controllerId];
      m_inputProvider->RegisterInputHandler(handler.get(), false);
      
      m_joystickInput[controllerId] = std::move(handler);
    }

    int GetWindowID() const
    {
      //return g_windowManager.GetActiveWindowID();
      return -1; // TODO
    }

    using WindowID = int;
    using WindowKeymap = std::vector<CJoystickKeymap>;

    std::map<WindowID, WindowKeymap> m_joystickKeymap;

    using ControllerID = std::string;
    std::map<ControllerID, std::unique_ptr<IInputHandler>> m_joystickInput;

    void MapAction(int windowId, const std::string& controllerId, const std::string& feature, KODI::JOYSTICK::ANALOG_STICK_DIRECTION dir, unsigned int holdtimeMs, const std::string &hotkey, std::unique_ptr<KODI::JOYSTICK::IKeymapHandler> keymapHandler)
    {
      WindowKeymap& windowKeymap = m_joystickKeymap[windowId];

      // Look up existing keymap for the controller profile
      auto itJoystick = std::find_if(windowKeymap.begin(), windowKeymap.end(),
        [&controllerId](const CJoystickKeymap& keymap)
        {
          return keymap.ControllerID() == controllerId;
        });

      if (itJoystick == windowKeymap.end())
      {
        std::unique_ptr<CJoystickKeymap> keymap(new CJoystickKeymap(controllerId));
        Register(controllerId, std::move(keymap));

        itJoystick = std::find_if(windowKeymap.begin(), windowKeymap.end(),
          [&controllerId](const CJoystickKeymap& entry)
          {
            return entry.ControllerID() == controllerId;
          });
      }

      //itJoystick->AddHandler

      /*
      windowKeymap.emplace_back(controllerId);
      itJoystick = windowKeymap.end() - 1;
      m_inputProvider->RegisterInputHandler(m_joystickProfiles);
    
      m_joystickProfiles.insert();
      m_controllers.insert(controllerId);
      m_inputProvider->RegisterInputHandler(m_joystickProfiles);
      *
    }

    bool ActivateDirection(const FeatureName& feature, float magnitude, ANALOG_STICK_DIRECTION dir, unsigned int motionTimeMs);
    void DeactivateDirection(const FeatureName& feature, ANALOG_STICK_DIRECTION dir);

    /*!
     * \brief Return a vector of the four cardinal directions
     *
    static const std::vector<ANALOG_STICK_DIRECTION>& GetDirections(void);

    // Construction parameter
    IInputProvider *const m_inputProvider;
  };
  */
}
}
