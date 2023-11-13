/*
N64:
      LOOKING AT THE PLUG FROM CONTROLLER
           . ~~~ .
       .-~         ~-.
      /   o   o   o   \
     /    1   2   3    \
     |_________________|


PIN # USAGE (colors from my extension cable, check your own)

  1:  GND            - red
  2:  DATA           - white
  3:  VCC +3.3V ONLY - black

GAMECUBE:
      LOOKING AT THE PLUG ON FRONT OF CONSOLE (not coming from controller)

               7
        .-------------.
       /       o       \
      /                 \
     |     1   2   3     |
     |     o   o   o     |
     |                   |
      \    o   o   o    /
       \   4   5   6   /
        `-.         .-'
            ~-._.-~

   PIN # USAGE (colors from my extension cable, check your own)

  1:  VCC 5V rumble  - grey
  2:  DATA           - yellow
  3:  GND            - black
  4:  GND            - black
  5:  UNKNOWN
  6:  VCC 3.3V ONLY  - red
  7:  GND            - black

  I tie all 3 grounds together
*/

#include "pins.h"

const int DATA_PIN_64M = OR_PIN_2;

// how often to poll, 100? 14? polling must not occur faster than every 20 ms
const int POLL_DELAY = 14;

#include "Arduino.h"

#include "Nintendo.h"

//#define DEBUG

#ifdef GAMECUBE

/*
// these are my average values from my apparantly very bad wavebird controller
const int axis_center_in_m64 = 135;
const int axis_max_in_m64 = 230;
const int axis_min_in_m64 = 30;

const int trigger_max_in_m64 = 236;
const int trigger_min_in_m64 = 36;
*/
// these look like much more proper values from a pelican gc controller
const int axis_center_in_m64 = 128;
const int axis_max_in_m64 = 255;
const int axis_min_in_m64 = 0;

const int trigger_max_in_m64 = 255;
const int trigger_min_in_m64 = 0;

#else  // N64
const int axis_center_in_m64 = 0;
const int axis_max_in_m64 = 80;
const int axis_min_in_m64 = -80;
#endif


#include "gamepad/Gamepad.h"
#include "util.cpp"

#ifdef GAMECUBE
typedef CGamecubeController NintendoController;
typedef Gamecube_Report_t ControllerReport;
#define NINTENDO_REPORT_SIZE 8
#else  // N64
typedef CN64Controller NintendoController;
typedef N64_Report_t ControllerReport;
#define NINTENDO_REPORT_SIZE 4
#endif

// Define a Controller
NintendoController controller(DATA_PIN_64M);

uint8_t oldReport[NINTENDO_REPORT_SIZE];

GAMEPAD_CLASS gamepad;

#ifdef DEBUG
void print_report(ControllerReport &controller) {
	Serial.print("buttons: ");
	Serial.print(controller.a ? "A" : "-");
	Serial.print(controller.b ? "B" : "-");
	Serial.print(controller.z ? "Z" : "-");
	Serial.print(controller.l ? "L" : "-");
	Serial.print(controller.r ? "R" : "-");
	Serial.print(controller.start ? "S" : "-");
#ifdef GAMECUBE
	Serial.print(controller.y ? "Y" : "-");
	Serial.print(controller.x ? "X" : "-");
#endif
	Serial.print(" DPAD: ");
	Serial.print(controller.dup ? "U" : "-");
	Serial.print(controller.ddown ? "D" : "-");
	Serial.print(controller.dleft ? "L" : "-");
	Serial.print(controller.dright ? "R" : "-");
	Serial.print(" Y: ");
	Serial.print(controller.yAxis);
	Serial.print(" YT: ");
	Serial.print(-translateAxis(controller.yAxis));
	Serial.print(" X: ");
	Serial.print(controller.xAxis);
	Serial.print(" XT: ");
	Serial.print(translateAxis(controller.xAxis));
#ifdef GAMECUBE
	Serial.print(" CY: ");
	Serial.print(controller.cyAxis);
	Serial.print(" CYT: ");
	Serial.print(-translateAxis(controller.cyAxis));
	Serial.print(" CX: ");
	Serial.print(controller.cxAxis);
	Serial.print(" CXT: ");
	Serial.print(translateAxis(controller.cxAxis));
	Serial.print(" LEFT: ");
	Serial.print(controller.left);
	Serial.print(" TLEFT: ");
	Serial.print(translateTrigger(controller.left));
	Serial.print(" RIGHT: ");
	Serial.print(controller.right);
	Serial.print(" TRIGHT: ");
	Serial.print(translateTrigger(controller.right));
#else  // N64
	Serial.print(" C: ");
	Serial.print(controller.cup ? "U" : "-");
	Serial.print(controller.cdown ? "D" : "-");
	Serial.print(controller.cleft ? "L" : "-");
	Serial.print(controller.cright ? "R" : "-");
#endif
	Serial.println();
}
#endif

#ifdef UNIVERSAL_MODE
void setup_m64() {
#else
void setup() {
#endif
	// n64 low because it *should* be 3.3V
	digitalWrite(DATA_PIN_64M, LOW);
#ifdef DEBUG
	Serial.begin(115200);
	if (controller.begin()) {
		Serial.println(F("controller.begin() success."));
	} else {
		Serial.println(F("controller.begin() fail."));
	}
#else
	controller.begin();
#endif

	gamepad.begin();
}

#ifdef UNIVERSAL_MODE
void loop_m64() {
#else
void loop() {
#endif
	delay(POLL_DELAY);
	// Try to read the controller data
	if (controller.read()) {
		// Print Controller information
		auto report = controller.getReport();

		if (memcmp(oldReport, report.raw8, NINTENDO_REPORT_SIZE)) {
			memcpy(oldReport, report.raw8, NINTENDO_REPORT_SIZE);
		} else {
			// nothing changed
			return;
		}

#ifdef DEBUG
		print_report(report);
#endif
		uint8_t c = 0;	// for now just do 1 pad
		gamepad.buttons(c, 0);
		if (report.start) {
			if (report.ddown) {
				// then only send menu, nothing else
				gamepad.press(c, BUTTON_MENU);
				gamepad.setHatSync(c, DPAD_CENTER);
				return;
			}
			gamepad.press(c, BUTTON_START);
		}
		if (report.a) {
			gamepad.press(c, BUTTON_A);
		}
		if (report.b) {
			gamepad.press(c, BUTTON_B);
		}
		if (report.z) {
			gamepad.press(c, BUTTON_TL2);
		}
		if (report.l) {
			gamepad.press(c, BUTTON_L);
		}
		if (report.r) {
			gamepad.press(c, BUTTON_R);
		}
		auto hat = calculateDpadDirection(report.dup, report.ddown, report.dleft, report.dright);
#ifdef GAMECUBE
		if (report.y) {
			gamepad.press(c, BUTTON_Y);
		}
		if (report.x) {
			gamepad.press(c, BUTTON_X);
		}
		gamepad.setAxis(c,
						translateAxis(report.xAxis),
						-translateAxis(report.yAxis),
						translateAxis(report.cxAxis),
						-translateAxis(report.cyAxis),
						translateTrigger(report.left),
						translateTrigger(report.right),
						hat);
#else  // N64
		auto cHat = dpadToAxis(calculateDpadDirection(report.cup, report.cdown, report.cleft, report.cright));
		gamepad.setAxis(c, translateAxis(report.xAxis), -translateAxis(report.yAxis), cHat.x, cHat.y, 0, 0, hat);
#endif
	} else {
		// Add debounce if reading failed
		delay(100);
#ifdef DEBUG
		Serial.println(F("Error reading controller."));
		if (controller.begin()) {
			Serial.println(F("controller.begin() success."));
		} else {
			Serial.println(F("controller.begin() fail."));
		}
#else
		controller.begin();
#endif
	}
}
