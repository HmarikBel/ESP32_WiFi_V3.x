// Keyboard.h

#ifndef _KEYBOARD_h
#define _KEYBOARD_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define KEY_PRESSED	1
#define SHORT_KEY	2
#define LONG_KEY	3

class KeyboardClass
{
	unsigned long m_timePressed = 0;

	unsigned long m_prevCheck;

	int m_prevState = 1;
	byte m_counter = 0;

	bool isPressed();
 protected:


 public:
	 uint8_t init();

	 uint8_t getKey();
};

extern KeyboardClass Keyboard;

#endif

