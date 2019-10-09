// Data.h

#ifndef _DATA_h
#define _DATA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include "WProgram.h"
#endif

#include "WiFiLogic.h"

#define S_ARRAY_SIZE 720

#define STORE_INTERVAL 30000

#define LOG_SIZE 20

struct LogItem
{
	unsigned long m_dateTime;
	String m_data;
};

class DataClass
{
	unsigned long m_nextSStore;

public:
	DataClass()
	{
		init();
	}

	SendDataInt m_power;
	SendDataInt m_voltage;
	SendDataFloat m_current;
	SendDataFloat m_temperature[3];

	long m_energyTotal;
	long m_energy;

	float m_temperatureMax[3];

	unsigned long m_lastCorrTime;
	int m_lastCorrValue;

	int m_sArray[S_ARRAY_SIZE];

	void init();
	void addPower(int value);

	LogItem m_log[LOG_SIZE];
	void addLog(String data);
};

extern DataClass Data;

#endif

