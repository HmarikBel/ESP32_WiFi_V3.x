// SetDateTimeState.h

#ifndef _MAINScreen_h
#define _MAINScreen_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "BaseScreen.h"

#define UPDATE_INTERVAL 1000

class EnergyLimitScreen : public BaseScreen
{
private:

	int m_limit = 0;

	bool m_writeDone = false;

	void Show(bool displayNow);

	void formatValue(char* buffer, float value);
public:
	EnergyLimitScreen(uint8_t parent, uint8_t prev, uint8_t next) :
		BaseScreen(parent, prev, next)
	{
	}

	virtual void Init();


	virtual uint8_t GetStateIndex()
	{
		return INDEX_ENERGY_LIMIT_SCREEN;
	};

	virtual uint8_t Process(uint8_t key);
};

#endif

