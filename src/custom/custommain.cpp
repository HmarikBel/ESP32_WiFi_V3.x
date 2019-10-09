#include "CustomMain.h"
#include "Data.h"
#include "WiFiLogic.h"

#include "defines.h"
#include "RapiSender.h"


unsigned long gNextTimeCorrection;
unsigned long gNextReadW;

extern RapiSender rapiSender;

void readData()
{
	unsigned long tmpNow = millis();

	if (tmpNow > gNextReadW && tmpNow - gNextReadW < OVERFLOW_CHECK)
	{
		gNextReadW += READ_DATA_MSEC;

		rapiSender.sendCmd("$G5", [](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 8)
				{
					Data.m_energyTotal = strtol(rapiSender.getToken(1), NULL, 10);
					Data.m_energy = strtol(rapiSender.getToken(2), NULL, 10);
					Data.m_power.m_currentValue = strtol(rapiSender.getToken(3), NULL, 10);
					Data.m_voltage.m_currentValue = strtol(rapiSender.getToken(4), NULL, 10);
					Data.m_current.m_currentValue = strtol(rapiSender.getToken(5), NULL, 10) / 10.0;
				} 
			}
			else {
				Data.m_energyTotal = -1;
				Data.m_energy = -1;
				Data.m_power.m_currentValue = -1;
				Data.m_voltage.m_currentValue = -1;
				Data.m_current.m_currentValue = -1;
			}

			Data.addPower(Data.m_power.m_currentValue);

			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_POWER, &Data.m_power, 200, SEND_DATA_MIN, SEND_DATA_MAX);
			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_VOLTAGE, &Data.m_voltage, 2, SEND_DATA_MIN, SEND_DATA_MAX);
			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_CURRENT, &Data.m_current, 1, SEND_DATA_MIN, SEND_DATA_MAX);
		});

		rapiSender.sendCmd("$G6", [](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 8)
				{
					Data.m_temperature[0].m_currentValue = strtol(rapiSender.getToken(1), NULL, 10) / 10.0;
					Data.m_temperatureMax[0] = strtol(rapiSender.getToken(2), NULL, 10) / 10.0;
					Data.m_temperature[1].m_currentValue = strtol(rapiSender.getToken(3), NULL, 10) / 10.0;
					Data.m_temperatureMax[1] = strtol(rapiSender.getToken(4), NULL, 10) / 10.0;
					Data.m_temperature[2].m_currentValue = strtol(rapiSender.getToken(5), NULL, 10) / 10.0;
					Data.m_temperatureMax[2] = strtol(rapiSender.getToken(6), NULL, 10) / 10.0;
				} 
			}
			else {
				Data.m_temperature[0].m_currentValue = -1;
				Data.m_temperatureMax[0] = -1;
				Data.m_temperature[1].m_currentValue = -1;
				Data.m_temperatureMax[1] = -1;
				Data.m_temperature[2].m_currentValue = -1;
				Data.m_temperatureMax[2] = -1;
			}

			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_TEMPERATURE, &Data.m_temperature[0], 1, SEND_DATA_MIN, SEND_DATA_MAX);
			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_TEMPERATURE + 1, &Data.m_temperature[1], 1, SEND_DATA_MIN, SEND_DATA_MAX);
			WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_TEMPERATURE + 2, &Data.m_temperature[2], 1, SEND_DATA_MIN, SEND_DATA_MAX);
		});
	}
}

void customLoop()
{
	//int tmp;
	String log;
	/*if (NTPClient.SyncTime(&tmp, &log))
	{
		Data.m_lastCorrValue = tmp;
		Data.m_lastCorrTime = now();
	}*/

	if (log.length() > 0)
	{
		Data.addLog(log);
	}

	readData();
}