
#include "main.h"

const int BUTTON_COUNT_NES = 12;	 // SNES has 12, NES only has 8
const int GAMEPAD_COUNT_MAX_NES = 4; 

//shared pins between all controllers
static const int LATCH_PIN = OR_PIN_1;	// brown
static const int CLOCK_PIN = OR_PIN_2;	// white

const int DATA_P1 = OR_PIN_3;
const int DATA_P3 = OR_PIN_5;

#ifdef BLUERETRO_MAPPING
const int DATA_P2 = OR_PIN_10;
const int DATA_P4 = OR_PIN_11;
#else
const int DATA_P2 = OR_PIN_4;
const int DATA_P4 = OR_PIN_6;
#endif

//individual data pin for each controller
static const int DATA_PIN_NES[GAMEPAD_COUNT_MAX_NES] = {
	DATA_P1,
	DATA_P2,
	DATA_P3,
	DATA_P4,
};

// power red, ground black

//#define DEBUG

static const uint8_t translateToNES[12] = {1, 8, 2, 3, 4, 5, 6, 7, 0, 8, 8, 8};

static const uint8_t nesButtons[4] = {0, 2, 3, 8};
static const uint8_t snesButtons[8] = {0, 1, 2, 3, 8, 9, 10, 11};

class GameControllers {
   public:
	enum Type
	{
		NES = 0,
		SNES = 1,
	};

	enum Button
	{
		B = 0,
		Y = 1,
		SELECT = 2,
		START = 3,
		UP = 4,
		DOWN = 5,
		LEFT = 6,
		RIGHT = 7,
		A = 8,
		X = 9,
		L = 10,
		R = 11,
	};
	Type types[GAMEPAD_COUNT_MAX_NES];
	int latchPin;
	int clockPin;
	int dataPins[GAMEPAD_COUNT_MAX_NES];
	long buttons[GAMEPAD_COUNT_MAX_NES][12];
	int maxButtons = 12;

	int changedControllers[GAMEPAD_COUNT_MAX_NES];

	///This has to be initialized once for the shared pins latch and clock
	void init(int latch, int clock) {
		latchPin = latch;
		clockPin = clock;
		pinMode(latchPin, OUTPUT);
		digitalWrite(latchPin, LOW);
		pinMode(clockPin, OUTPUT);
		digitalWrite(clockPin, HIGH);
	}

	///This sets the controller type and initializes its individual data pin
	void setController(int controller, Type type, int dataPin) {
		types[controller] = type;
		dataPins[controller] = dataPin;
		pinMode(dataPins[controller], INPUT_PULLUP);
		for (int i = 0; i < 12; i++) {
			buttons[controller][i] = -1;
		}
	}

	void poll(void (*controllerChangedNes)(const int controller)) {
		memset(changedControllers, 0, sizeof(changedControllers));

		digitalWrite(latchPin, HIGH);
		delayMicroseconds(12);
		digitalWrite(latchPin, LOW);
		delayMicroseconds(6);
		//Serial.print("snes: ");
		for (int i = 0; i < maxButtons; i++) {
			for (int c = 0; c < gamepad_count; c++) {
				if (digitalRead(dataPins[c])) {
					// up
					//Serial.print("-");
					if (-1 != buttons[c][i]) {
						buttons[c][i] = -1;
						changedControllers[c] = 1;
					}
				} else {
					// down
					//Serial.print(i);
					//++buttons[c][i];
					//changedControllers[c] = 1;
					if (0 != buttons[c][i]) {
						buttons[c][i] = 0;
						changedControllers[c] = 1;
					}
				}
			}
			digitalWrite(clockPin, LOW);
			delayMicroseconds(6);
			digitalWrite(clockPin, HIGH);
			delayMicroseconds(6);
		}
		//Serial.println();

		for (int c = 0; c < gamepad_count; c++) {
			// have any buttons changed state?
			if (1 == changedControllers[c]) {
				controllerChangedNes(c);
			}
		}
	}

	uint8_t translate(int controller, uint8_t b) {
		if (types[controller] == SNES)
			return b;
		return translateToNES[b];
	}

	///returns if button is currently down
	bool down(int controller, uint8_t b) {
		return buttons[controller][translate(controller, b)] >= 0;
	}

	void pressAll(int controller, void (*pushButton)(const int controller, const uint8_t btn)) {
		if (types[controller] == SNES) {
			for (uint8_t i = 0; i < sizeof(snesButtons); i++) {
				if (buttons[controller][snesButtons[i]] >= 0) {
					pushButton(controller, snesButtons[i]);
				}
			}
		} else {
			for (uint8_t i = 0; i < sizeof(nesButtons); i++) {
				if (buttons[controller][translateToNES[nesButtons[i]]] >= 0) {
					pushButton(controller, nesButtons[i]);
				}
			}
		}
	}
};

static const int translateToHidNes[12] = {
	BUTTON_B,
	BUTTON_Y,
	BUTTON_SELECT,
	BUTTON_START,
	DPAD_UP,
	DPAD_DOWN,
	DPAD_LEFT,
	DPAD_RIGHT,
	BUTTON_A,
	BUTTON_X,
	BUTTON_L,
	BUTTON_R,
};

GameControllers controllersNes;

void dummyControllerChanged(const int c) {
}

#ifdef DEBUG
void controllerChangedDebug(const int c) {
	Serial.print("controllerChangedNes!!!!: ");
	Serial.println(c);
	Serial.print("c: ");
	Serial.print(c);
	for (uint8_t btn = 0; btn < 12; btn++) {
		Serial.print("; ");
		Serial.print(btn);
		Serial.print(", ");
		Serial.print(controllersNes.buttons[c][controllersNes.translate(c, btn)]);
		Serial.print(",");
		Serial.print(controllersNes.buttons[c][btn]);
	}
	Serial.println("");
}
#endif	// DEBUG

#ifdef UNIVERSAL_MODE
void setup_nes() {
#else
void setup() {
#endif
	setupBrLed();
	bool allNes = true;
#ifdef DEBUG
	delay(5000);
#endif	// DEBUG
	gamepad.begin();

	//initialize shared pins
	controllersNes.init(LATCH_PIN, CLOCK_PIN);

	//activate first controller and set the type to SNES
	for (int c = 0; c < gamepad_count; c++) {
		controllersNes.setController(c, GameControllers::SNES, DATA_PIN_NES[c]);
	}

	// poll controllersNes once to detect NES vs SNES
	controllersNes.poll(dummyControllerChanged);

	for (int c = 0; c < gamepad_count; c++) {
		// for NES, A+X+L+R are down always, re-initialize
		if (controllersNes.down(c, GameControllers::A) && controllersNes.down(c, GameControllers::X) && controllersNes.down(c, GameControllers::L) && controllersNes.down(c, GameControllers::R)) {
#ifdef DEBUG
			Serial.println("detected NES");
#endif	// DEBUG
			controllersNes.types[c] = GameControllers::NES;
		} else {
#ifdef DEBUG
			Serial.println("detected SNES");
#endif	// DEBUG
			allNes = false;
		}
	}
	if (allNes) {
#ifdef DEBUG
		Serial.println("detected ONLY NES");
#endif	// DEBUG
		controllersNes.maxButtons = 8;
	}
}

void pushButton(const int c, const uint8_t btn) {
	gamepad.press(c, translateToHidNes[btn]);
}

void controllerChangedNes(const int c) {
#ifdef DEBUG
	controllerChangedDebug(c);
#endif	// DEBUG

	gamepad.buttons(c, 0);
	// if start and select are held at the same time, send menu and only menu
	if (controllersNes.down(c, GameControllers::START) && controllersNes.down(c, GameControllers::SELECT)) {
		gamepad.press(c, BUTTON_MENU);
	} else {
		// actually send buttons held
		controllersNes.pressAll(c, pushButton);
	}
	if (controllersNes.down(c, GameControllers::DOWN)) {
		if (controllersNes.down(c, GameControllers::RIGHT)) {
			gamepad.setHatSync(c, DPAD_DOWN_RIGHT);
		} else if (controllersNes.down(c, GameControllers::LEFT)) {
			gamepad.setHatSync(c, DPAD_DOWN_LEFT);
		} else {
			gamepad.setHatSync(c, DPAD_DOWN);
		}
	} else if (controllersNes.down(c, GameControllers::UP)) {
		if (controllersNes.down(c, GameControllers::RIGHT)) {
			gamepad.setHatSync(c, DPAD_UP_RIGHT);
		} else if (controllersNes.down(c, GameControllers::LEFT)) {
			gamepad.setHatSync(c, DPAD_UP_LEFT);
		} else {
			gamepad.setHatSync(c, DPAD_UP);
		}
	} else if (controllersNes.down(c, GameControllers::RIGHT)) {
		gamepad.setHatSync(c, DPAD_RIGHT);
	} else if (controllersNes.down(c, GameControllers::LEFT)) {
		gamepad.setHatSync(c, DPAD_LEFT);
	} else {
		gamepad.setHatSync(c, DPAD_CENTERED_OR);
	}
}

#ifdef UNIVERSAL_MODE
void loop_nes() {
#else
void loop() {
#endif
	if (gamepad.isConnected()) {
		controllersNes.poll(controllerChangedNes);  //read all controllersNes at once
	}
}