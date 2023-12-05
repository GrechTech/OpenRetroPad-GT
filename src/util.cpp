int axis_max_in = 32767;
int axis_min_in = -32767;
int axis_center_in = 0;
int trigger_max_in = 127;
int trigger_min_in = -127;

#ifndef GAMEPAD_COUNT
#define GAMEPAD_COUNT 1
#endif
uint8_t gamepad_count = GAMEPAD_COUNT;

void setBounds(int _axis_max_in = 32767, int _axis_min_in = -32767, int _axis_center_in = 0, int _trigger_max_in = 127, int _trigger_min_in = -127)
{
	axis_max_in = _axis_max_in;
	axis_min_in = _axis_max_in;
	axis_center_in = _axis_max_in;
	trigger_max_in = _axis_max_in;
	trigger_min_in = _axis_max_in;
}

uint8_t calculateDpadDirection(const bool up, const bool down, const bool left, const bool right) {
	if (down) {
		if (right) {
			return DPAD_DOWN_RIGHT;
		} else if (left) {
			return DPAD_DOWN_LEFT;
		} else {
			return DPAD_DOWN;
		}
	} else if (up) {
		if (right) {
			return DPAD_UP_RIGHT;
		} else if (left) {
			return DPAD_UP_LEFT;
		} else {
			return DPAD_UP;
		}
	} else if (right) {
		return DPAD_RIGHT;
	} else if (left) {
		return DPAD_LEFT;
	} else {
		return DPAD_CENTERED_OR;
	}
}

struct Axis {
	int16_t x;
	int16_t y;
};

struct Axis axis(int16_t x, int16_t y) {
	Axis axis;
	axis.x = x;
	axis.y = y;
	return axis;
}

struct Axis dpadToAxis(uint8_t dpad) {
	switch (dpad) {
		case DPAD_CENTER:
			return axis(AXIS_CENTER, AXIS_CENTER);
		case DPAD_UP:
			return axis(AXIS_CENTER, AXIS_MIN);
		case DPAD_UP_RIGHT:
			return axis(AXIS_MAX, AXIS_MAX);
		case DPAD_RIGHT:
			return axis(AXIS_MAX, AXIS_CENTER);
		case DPAD_DOWN_RIGHT:
			return axis(AXIS_MAX, AXIS_MAX);
		case DPAD_DOWN:
			return axis(AXIS_CENTER, AXIS_MAX);
		case DPAD_DOWN_LEFT:
			return axis(AXIS_MIN, AXIS_MAX);
		case DPAD_LEFT:
			return axis(AXIS_MIN, AXIS_CENTER);
		case DPAD_UP_LEFT:
			return axis(AXIS_MIN, AXIS_MIN);
	}
	// todo: panic here?
	return axis(AXIS_CENTER, AXIS_CENTER);
}

inline long translate(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline int16_t translateAxis(long v) {
	// pin to max/min
	if (v <= axis_min_in) {
		return AXIS_MIN;
	} else if (v >= axis_max_in) {
		return AXIS_MAX;
	}
	// don't map at all if translation isn't required...
#if axis_center_in == AXIS_CENTER && axis_min_in == AXIS_MIN && axis_max_in == AXIS_MAX
	return v;  // noop
#else
	return v == axis_center_in ? AXIS_CENTER : translate(v, axis_min_in, axis_max_in, AXIS_MIN, AXIS_MAX);
#endif
}

inline uint8_t translateTrigger(long v) {
	// pin to max/min
	if (v <= trigger_min_in) {
		return TRIGGER_MIN;
	} else if (v >= trigger_max_in) {
		return TRIGGER_MAX;
	}
	// don't map at all if translation isn't required...
#if trigger_min_in == TRIGGER_MIN && trigger_max_in == TRIGGER_MAX
	return v;  // noop
#else
	return translate(v, trigger_min_in, trigger_max_in, TRIGGER_MIN, TRIGGER_MAX);
#endif
}

#ifdef BLUERETRO_MAPPING
void setupBrLed()
{
	pinMode(17, OUTPUT);
	digitalWrite(17, LOW);
}
#else
void setupBrLed(){}
#endif