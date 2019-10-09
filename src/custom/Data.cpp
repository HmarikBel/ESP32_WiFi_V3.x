// 
// 
// 

#include "Data.h"
//#include <timelib.h>

void DataClass::init()
{
	m_energyTotal = 0;
	m_energy = 0;
	m_power.m_currentValue = 0;
	m_current.m_currentValue = 0;
	m_voltage.m_currentValue = 0;
	m_temperatureIn.m_currentValue = 0;

	m_temperatureInMax = 0;


	m_lastCorrTime = 0;
	m_lastCorrValue = 0;

	for (int i = 0; i < S_ARRAY_SIZE; i++)
	{
		m_sArray[i] = 0;
	}

	m_nextSStore = 0;

	memset(m_log, 0, sizeof(LogItem) * LOG_SIZE);
}

void DataClass::addPower(int value)
{
	//m_energy = value;

	if (millis() > m_nextSStore)
	{
		memcpy(m_sArray, &(m_sArray[1]), sizeof(int) * (S_ARRAY_SIZE - 1));
		m_sArray[S_ARRAY_SIZE - 1] = value;

		m_nextSStore = m_nextSStore + STORE_INTERVAL;
	}
}

void DataClass::addLog(String value)
{
	for (int i = LOG_SIZE - 2; i >= 0; i--)
	{
		m_log[i + 1].m_data = m_log[i].m_data;
		m_log[i + 1].m_dateTime = m_log[i].m_dateTime;
	}
	m_log[0].m_data = value;
	m_log[0].m_dateTime = time(0);
}


DataClass Data;

