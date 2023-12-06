/*
Wii Nunchuck/Wii Classic/SNES+NES Classic:
      LOOKING AT THE PLUG ON FRONT OF CONSOLE/BACK OF WIIMOTE (not coming from controller)

       |---------------|
       |   1   3   5   |
       |   o   o   o   |
       |   o   o   o   |
       |   2   4   6   |
       |   |-------|   |
       |---|       |---|

   PIN # USAGE (colors from my extension cable, check your own)

  1:  VCC 3.3V ONLY      - white
  2:  SCL                - yellow
  3:  3.3V SENSE, unused - red
  4:  unused             -
  5:  SDA                - green
  6:  GND                - black
*/

#include "main.h"

// we only support 1 pad here

#include <NintendoExtensionCtrl.h>


ExtensionPort port;	 // Port for communicating with extension controllers

Nunchuk::Shared nchuk(port);			  // Read Nunchuk formatted data from the port
ClassicController::Shared classic(port);  // Read Classic Controller formatted data from the port

NintendoExtensionCtrl::ExtensionController* controllersWii[] = {
	// Array of available controllers, for controller-specific init
	&nchuk,
	&classic,
};

const int NumControllers = sizeof(controllersWii) / sizeof(NintendoExtensionCtrl::ExtensionController*);	// # of controllers, auto-generated

void (*controllerChangedWii)();

const uint8_t c = 0;  // for now just do 1 pad

void nunchuckChanged() {
	//nchuk.printDebug(); return;
	gamepad.buttons(c, 0);
	if (nchuk.buttonC()) {
		gamepad.press(c, BUTTON_A);
	}
	if (nchuk.buttonZ()) {
		gamepad.press(c, BUTTON_B);
	}
	// todo: anything with roll/pitch/accel ?
	gamepad.setAxis(c, translateAxis(nchuk.joyX()), -translateAxis(nchuk.joyY()), 0, 0, 0, 0, DPAD_CENTER);
}

void classicChanged() {
	//classic.printDebug(); return;
	gamepad.buttons(c, 0);
	if (classic.buttonA()) {
		gamepad.press(c, BUTTON_A);
	}
	if (classic.buttonB()) {
		gamepad.press(c, BUTTON_B);
	}
	if (classic.buttonY()) {
		gamepad.press(c, BUTTON_Y);
	}
	if (classic.buttonX()) {
		gamepad.press(c, BUTTON_X);
	}
	if (classic.buttonZL()) {
		gamepad.press(c, BUTTON_L);
	}
	if (classic.buttonZR()) {
		gamepad.press(c, BUTTON_R);
	}
	if (classic.buttonL()) {
		gamepad.press(c, BUTTON_TL2);
	}
	if (classic.buttonR()) {
		gamepad.press(c, BUTTON_TR2);
	}
	if (classic.buttonPlus()) {
		gamepad.press(c, BUTTON_PLUS);
	}
	if (classic.buttonMinus()) {
		gamepad.press(c, BUTTON_MINUS);
	}
	if (classic.buttonHome()) {
		gamepad.press(c, BUTTON_HOME);
	}
	auto hat = calculateDpadDirection(classic.dpadUp(), classic.dpadDown(), classic.dpadLeft(), classic.dpadRight());
	gamepad.setAxis(c,
					translateAxis(classic.leftJoyX()),
					-translateAxis(classic.leftJoyY()),
					translateAxis(classic.rightJoyX()),
					-translateAxis(classic.rightJoyY()),
					translateTrigger(classic.triggerL()),
					translateTrigger(classic.triggerR()),
					hat);
}

boolean connectController() {
	boolean connected = port.connect();	 // Connect to the controller

	if (connected == true) {
		for (int i = 0; i < NumControllers; i++) {
			if (controllersWii[i]->controllerTypeMatches()) {	 // If this controller is connected...
				connected = controllersWii[i]->specificInit();	 // ...run the controller-specific initialization
				if (connected == true) {
					ExtensionType conType = port.getControllerType();
					switch (conType) {
						case (ExtensionType::Nunchuk):
							controllerChangedWii = nunchuckChanged;
							break;
						case (ExtensionType::ClassicController):
							controllerChangedWii = classicChanged;
							break;
						default:
							//Serial.println("Other controller connected!");
							return false;
					}
					return true;
				}
			}
		}
	}

	return connected;
}

#ifdef UNIVERSAL_MODE
void setup_wii() {
#else
void setup() {
#endif
	setupBrLed();
	setBounds(230, 15, 126, 255, 40);
	gamepad.begin();
	port.begin();  // init I2C

	while (!connectController()) {
		//Serial.println("No controller found!");
		delay(1000);
	}
}

#ifdef UNIVERSAL_MODE
void loop_wii() {
#else
void loop() {
#endif
	boolean success = port.update();  // Get new data from the controller

	if (success == true) {	// We've got data!
		// todo: only call this if data changed?
		controllerChangedWii();
	} else {  // Data is bad :(
		while (!connectController()) {
			//Serial.println("Controller Disconnected!");
			delay(1000);
		}
	}
}
