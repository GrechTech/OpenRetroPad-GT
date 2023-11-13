#ifndef UTIL_H
#define UTIL_H

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
		return DPAD_CENTERED;
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

inline int16_t translateAxis(long v, long int_centr, long in_min, long in_max) {
	// pin to max/min
	if (v <= in_min) {
		return AXIS_MIN;
	} else if (v >= in_max) {
		return AXIS_MAX;
	}
	// don't map at all if translation isn't required...
	if (int_centr == AXIS_CENTER && in_min == AXIS_MIN && in_max == AXIS_MAX)
		return v;  // noop
	else
		return v == int_centr ? AXIS_CENTER : translate(v, in_min, in_max, AXIS_MIN, AXIS_MAX);
}

inline uint8_t translateTrigger(long v, long in_min, long in_max) {
	// pin to max/min
	if (v <= in_min) {
		return TRIGGER_MIN;
	} else if (v >= in_max) {
		return TRIGGER_MAX;
	}
	// don't map at all if translation isn't required...
	if (in_min == TRIGGER_MIN && in_max == TRIGGER_MAX)
		return v;  // noop
	else
		return translate(v, in_min, in_max, TRIGGER_MIN, TRIGGER_MAX);
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

#endif