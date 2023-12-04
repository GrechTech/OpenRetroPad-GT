
#ifndef CODE_PLATFORM
#if defined(ARDUINO_ARCH_ESP32)

#define CODE_PLATFORM 1	 // optimized for esp32

#elif defined(ARDUINO_AVR_MICRO)

#define CODE_PLATFORM 2	 // optimized for micro

#else

#define CODE_PLATFORM 0	 // generic

#endif	// CODE_PLATFORM detection
#endif	// ifndef CODE_PLATFORM

#if GAMEPAD_OUTPUT == 0

#include "Debug-Gamepad/DebugGamepad.h"

#elif GAMEPAD_OUTPUT == 1

#include "Radio-Gamepad/RadioGamepad.h"

#elif GAMEPAD_OUTPUT == 2

#include "USB-Gamepad/UsbGamepad.h"

#elif GAMEPAD_OUTPUT == 3

#ifndef GAMEPAD_CLASS
#define GAMEPAD_CLASS BleGamepad
#endif
#include "common.h"
#include "BleGamepad.h"
#define press(c, b) press(b)
#define setHatSync(c, b) setHat(b)
#define setAxis(c, x, y, rx, ry, l, r, h) setAxes(x, y, 0, rx, ry, 0, l, r)
#define isPressedZ(c, b) isPressed(b)
#define buttons(c, a) setBatteryLevel(c)

#elif GAMEPAD_OUTPUT == 4

#include "UsbRadio-Gamepad/UsbRadioGamepad.h"

#elif GAMEPAD_OUTPUT == 5

#include "Switch-USB-Gamepad/SwitchUsbGamepad.h"

#else

#error Unsupported value for GAMEPAD_OUTPUT, must be 0-3

#endif	// GAMEPAD_OUTPUT detection
