set(SOURCES DeadzoneFilter.cpp
            DefaultController.cpp
            DefaultJoystick.cpp
            DefaultRemote.cpp
            DriverPrimitive.cpp
            JoystickEasterEgg.cpp
            JoystickKeymap.cpp
            JoystickMonitor.cpp
            JoystickTranslator.cpp
            RumbleGenerator.cpp)

set(HEADERS DeadzoneFilter.h
            DefaultController.h
            DefaultJoystick.h
            DefaultRemote.h
            DriverPrimitive.h
            IActionHandler.h
            IActionMap.h
            IButtonMap.h
            IButtonMapCallback.h
            IButtonMapper.h
            IButtonSequence.h
            IDriverHandler.h
            IDriverReceiver.h
            IInputHandler.h
            IInputProvider.h
            IInputReceiver.h
            IKeymapHandler.h
            JoystickEasterEgg.h
            JoystickIDs.h
            JoystickKeymap.h
            JoystickMonitor.h
            JoystickTranslator.h
            JoystickTypes.h
            JoystickUtils.h
            RumbleGenerator.h)

core_add_library(input_joystick)
