/* -------------------------------------------------------------------------
Saturn controller socket (looking face-on at the front of the socket):
___________________
/ 1 2 3 4 5 6 7 8 9 \
|___________________|

Saturn controller plug (looking face-on at the front of the controller plug):
___________________
/ 9 8 7 6 5 4 3 2 1 \
|___________________|

Saturn
-----------------
1 VCC     - black
2 DATA1   - white
3 DATA0   - gray
4 SEL1    - blue
5 SEL0    - green
6 TL (5V) - yellow
7 DATA3   - orange
8 DATA2   - red
9 GND     - brown

NOTE: The receiver of the Retro Bit 2.4GHz controller needs to be plugged
      in after the adapter has been connected to USB and the RETROBIT_WL
      define needs to be uncommented.
------------------------------------------------------------------------- */
#include "main.h"

#include <digitalWriteFast.h>
//#define digitalWriteFast digitalWrite
//#define digitalReadFast digitalRead

// How many microseconds to wait after setting select lines? (2µs is enough according to the Saturn developer's manual)
// 20µs is a "safe" value that seems to work for original Saturn controllers and Retrobit wired controllers
const int SELECT_PAUSE = 10;

// Uncomment to support the Retro Bit 2.4GHz wireless controller (this will increase lag a lot)
//#define RETROBIT_WL

// pins
const int SS_P1_2 = OR_PIN_2;
const int SS_P1_3 = OR_PIN_3;
const int SS_P1_7 = OR_PIN_1;
const int SS_P1_8 = OR_PIN_11;

const int SS_PX_5 = OR_PIN_5;

const int SS_P2_2 = OR_PIN_20;
const int SS_P2_3 = OR_PIN_21;
const int SS_P2_6 = OR_PIN_10;
const int SS_P2_7 = OR_PIN_18;
const int SS_P2_8 = OR_PIN_19;

#ifdef BLUERETRO_MAPPING
const int SS_P1_6 = ALT_PIN_1;
const int SS_PX_4 = ALT_PIN_2;
#else
const int SS_P1_6 = OR_PIN_4;
const int SS_PX_4 = OR_PIN_6;
#endif

ScratchGamepad currentGamepad;

// Read R, X, Y, Z
void read1() {
	// Set select outputs to 00
	digitalWriteFast(SS_PX_4, LOW);
	digitalWriteFast(SS_PX_5, LOW);
	delayMicroseconds(SELECT_PAUSE);
	if (!digitalReadFast(SS_P1_3)) {
		// Z
		currentGamepad.press(0, BUTTON_R);
	}
	if (!digitalReadFast(SS_P1_2)) {
		// Y
		currentGamepad.press(0, BUTTON_X);
	}
	if (!digitalReadFast(SS_P1_8)) {
		// X
		currentGamepad.press(0, BUTTON_L);
	}
	if (!digitalReadFast(SS_P1_7)) {
		// R
		currentGamepad.press(0, BUTTON_R2);
	}
	if (gamepad_count == 2)
	{
		if (!digitalReadFast(SS_P2_3)) {
			// Z
			currentGamepad.press(1, BUTTON_R);
		}
		if (!digitalReadFast(SS_P2_2)) {
			// Y
			currentGamepad.press(1, BUTTON_X);
		}
		if (!digitalReadFast(SS_P2_8)) {
			// X
			currentGamepad.press(1, BUTTON_L);
		}
		if (!digitalReadFast(SS_P2_7)) {
			// R
			currentGamepad.press(1, BUTTON_R2);
		}
	}
}

// Read ST, A, C, B
void read2() {
	// Toggle select outputs (01->10 or 10->01)
	digitalWriteFast(SS_PX_4, HIGH);
	digitalWriteFast(SS_PX_5, LOW);
	delayMicroseconds(SELECT_PAUSE);
	if (!digitalReadFast(SS_P1_3)) {
		// B
		currentGamepad.press(0, BUTTON_B);
	}
	if (!digitalReadFast(SS_P1_2)) {
		// C
		currentGamepad.press(0, BUTTON_A);
	}
	if (!digitalReadFast(SS_P1_8)) {
		// A
		currentGamepad.press(0, BUTTON_Y);
	}
	if (!digitalReadFast(SS_P1_7)) {
		// ST
		currentGamepad.press(0, BUTTON_START);
	}
	if (gamepad_count == 2)
	{
		if (!digitalReadFast(SS_P2_3)) {
			// B
			currentGamepad.press(1, BUTTON_B);
		}
		if (!digitalReadFast(SS_P2_2)) {
			// C
			currentGamepad.press(1, BUTTON_A);
		}
		if (!digitalReadFast(SS_P2_8)) {
			// A
			currentGamepad.press(1, BUTTON_Y);
		}
		if (!digitalReadFast(SS_P2_7)) {
			// ST
			currentGamepad.press(1, BUTTON_START);
		}
	}
}

// Read DR, DL, DD, DU
void read3() {
	// Set select outputs to 10 from 11 (toggle)
	digitalWriteFast(SS_PX_4, LOW);
	digitalWriteFast(SS_PX_5, HIGH);
	delayMicroseconds(SELECT_PAUSE);
	if (!digitalReadFast(SS_P1_3)) {
		// UP
		currentGamepad.pressDpad(0, DPAD_BIT_UP);
	}
	if (!digitalReadFast(SS_P1_2)) {
		// DOWN
		currentGamepad.pressDpad(0, DPAD_BIT_DOWN);
	}
	if (!digitalReadFast(SS_P1_8)) {
		// LEFT
		currentGamepad.pressDpad(0, DPAD_BIT_LEFT);
	}
	if (!digitalReadFast(SS_P1_7)) {
		// RIGHT
		currentGamepad.pressDpad(0, DPAD_BIT_RIGHT);
	}
	if (gamepad_count == 2)
	{
		if (!digitalReadFast(SS_P2_3)) {
			// UP
			currentGamepad.pressDpad(1, DPAD_BIT_UP);
		}
		if (!digitalReadFast(SS_P2_2)) {
			// DOWN
			currentGamepad.pressDpad(1, DPAD_BIT_DOWN);
		}
		if (!digitalReadFast(SS_P2_8)) {
			// LEFT
			currentGamepad.pressDpad(1, DPAD_BIT_LEFT);
		}
		if (!digitalReadFast(SS_P2_7)) {
			// RIGHT
			currentGamepad.pressDpad(1, DPAD_BIT_RIGHT);
		}
	}
}

// Read L, *, *, *
void read4() {
	// Set select outputs to 11
	digitalWriteFast(SS_PX_4, HIGH);
	digitalWriteFast(SS_PX_5, HIGH);
	delayMicroseconds(SELECT_PAUSE);
	if (!digitalReadFast(SS_P1_7)) {
		// L
		currentGamepad.press(0, BUTTON_L2);
	}
	if (gamepad_count == 2)
	{
		if (!digitalReadFast(SS_P2_7)) {
			// L
			currentGamepad.press(1, BUTTON_L2);
		}
	}
}

#ifdef UNIVERSAL_MODE
void setup_ss() {
#else
void setup() {
#endif
	setupBrLed();
	gamepad.begin();

	// Set P1 data pins  as inputs and enable pull-up resistors
	pinMode(SS_P1_3, INPUT_PULLUP);
	pinMode(SS_P1_2, INPUT_PULLUP);
	pinMode(SS_P1_7, INPUT_PULLUP);
	pinMode(SS_P1_8, INPUT_PULLUP);
	digitalWrite(SS_P1_3, HIGH);
	digitalWrite(SS_P1_2, HIGH);
	digitalWrite(SS_P1_7, HIGH);
	digitalWrite(SS_P1_8, HIGH);

	// Set P1 TL as input and enable pull-up resistor
	pinMode(SS_P1_6, INPUT_PULLUP);
	digitalWrite(SS_P1_6, HIGH);

	if (gamepad_count == 2)
	{
		// Set P2 data pins as inputs and enable pull-up resistors
		pinMode(SS_P2_3, INPUT_PULLUP);
		pinMode(SS_P2_2, INPUT_PULLUP);
		pinMode(SS_P2_7, INPUT_PULLUP);
		pinMode(SS_P2_8, INPUT_PULLUP);
		digitalWrite(SS_P2_3, HIGH);
		digitalWrite(SS_P2_2, HIGH);
		digitalWrite(SS_P2_7, HIGH);
		digitalWrite(SS_P2_8, HIGH);

		// Set P2 TL as input and enable pull-up resistor
		pinMode(SS_P2_6, INPUT_PULLUP);
		digitalWrite(SS_P2_6, HIGH);
	}

	// Set P1+P2 select pins as outputs and set them HIGH
	pinMode(SS_PX_4, OUTPUT);
	pinMode(SS_PX_5, OUTPUT);
	digitalWrite(SS_PX_4, HIGH);
	digitalWrite(SS_PX_5, HIGH);

	// Wait for the controller(s) to settle
	delay(100);
}

#ifdef UNIVERSAL_MODE
void loop_ss() {
#else
void loop() {
#endif
	// Read all button and axes states
	read3();
	read2();
	read1();
	read4();

	// Send data to USB if values have changed
	for (uint8_t gp = 0; gp < gamepad_count; gp++) {
		if (currentGamepad.changed(gp, gamepad)) {
			const auto hat = gamepad.getHat(gp);
			if (hat == DPAD_DOWN && gamepad.isPressed(gp, BUTTON_START)) {
				gamepad.buttons(gp, 0);
				gamepad.press(gp, BUTTON_MENU);
				gamepad.setHatSync(gp, DPAD_CENTERED_OR);
				currentGamepad.changed(gp, gamepad);
				return;
			}
			gamepad.setHatSync(gp, hat);
		}
		// Clear button data
		currentGamepad.reset(gp);
	}

#ifdef RETROBIT_WL
	// This delay is needed for the retro bit 2.4GHz wireless controller, making it more or less useless with this adapter
	delay(17);
#endif
}