// BaseState.h

#ifndef _BASESTATE_h
#define _BASESTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Defines.h"

class BaseScreen
{
private:
	unsigned long m_timeToReturn;
	uint8_t m_parent;
	uint8_t m_prev;
	uint8_t m_next;

protected:

	void FormatCurrentDateTime(char* buffer);

	byte getSecondsToDisplay();


public:
	BaseScreen(uint8_t parent, uint8_t prev, uint8_t next)
	{
		m_parent = parent;
		m_prev = prev;
		m_next = next;

		ResetEnterTime();
	}

	virtual ~BaseScreen()
	{

	}

	void ResetEnterTime()
	{
		m_timeToReturn = millis() + SCREEN_TIMEOUT;
	};

	virtual uint8_t GetStateIndex();

	virtual void Init()	{};

	virtual uint8_t Process(uint8_t key);
};

#endif

