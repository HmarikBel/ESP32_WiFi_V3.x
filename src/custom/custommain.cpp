#include "CustomMain.h"
#include "Data.h"
#include "WiFiLogic.h"

#include "defines.h"
#include "RapiSender.h"

#include "debug.h"

#include "EnergyLimitScreen.h"


unsigned long gNextTimeCorrection = 0;
unsigned long gNextReadW = 0;

extern RapiSender rapiSender;

AiEsp32RotaryEncoder gRotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, -1, -1);

byte gCurScreenIndex = INDEX_NO_SCREEN;
byte gPrevScreenIndex = INDEX_NO_SCREEN;
BaseScreen* gpCurScreen = NULL;

void readData()
{
	unsigned long tmpNow = millis();

	if (tmpNow > gNextReadW && tmpNow - gNextReadW < OVERFLOW_CHECK)
	{
		gNextReadW += READ_DATA_MSEC;

		rapiSender.sendCmd("$G5", [](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 6)
				{
					Data.m_energyTotal = strtol(rapiSender.getToken(1), NULL, 10);
					Data.m_energy = strtol(rapiSender.getToken(2), NULL, 10);
					Data.m_power.m_currentValue = strtol(rapiSender.getToken(3), NULL, 10);
					Data.m_voltage.m_currentValue = strtol(rapiSender.getToken(4), NULL, 10);
					Data.m_current.m_currentValue = strtol(rapiSender.getToken(5), NULL, 10) / 10.0;

					Data.addPower(Data.m_power.m_currentValue);

					if(Data.m_power.m_currentValue >= 0)
					{
						WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_POWER, &Data.m_power, 200, SEND_DATA_MIN, SEND_DATA_MAX);
					}
					
					if(Data.m_voltage.m_currentValue > 0)
					{
						WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_VOLTAGE, &Data.m_voltage, 2, SEND_DATA_MIN, SEND_DATA_MAX);
					}
					
					if(Data.m_current.m_currentValue >= 0)
					{
						WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_CURRENT, &Data.m_current, 1, SEND_DATA_MIN, SEND_DATA_MAX);
					}
				} 
			}
			else {
				Data.m_energyTotal = -1;
				Data.m_energy = -1;
				Data.m_power.m_currentValue = -1;
				Data.m_voltage.m_currentValue = -1;
				Data.m_current.m_currentValue = -1;
			}		
		});

		rapiSender.sendCmd("$G6", [](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 4)
				{
					float t1 = strtol(rapiSender.getToken(1), NULL, 10) / 10.0;
					float t2 = strtol(rapiSender.getToken(2), NULL, 10) / 10.0;
					float t3 = strtol(rapiSender.getToken(3), NULL, 10) / 10.0;

					if(t1 > 100 || t2 > 100 || t3 > 100 )
					{
						return;  // incorrect readings
					}

					Data.m_temperature[0].m_currentValue = t1;
					Data.m_temperature[1].m_currentValue = t2;
					Data.m_temperature[2].m_currentValue = t3;

					for(int i=0; i<3; i++)
					{
						if(Data.m_temperature[i].m_currentValue != -1)
						{
							WiFiLogic.SendIfNecessary(COUNTER_ID_CHARGER_TEMPERATURE + i, &Data.m_temperature[i], 1, SEND_DATA_MIN, SEND_DATA_MAX);
						}
					}
				} 
			}
			else {
				Data.m_temperature[0].m_currentValue = -1;
				Data.m_temperature[1].m_currentValue = -1;
				Data.m_temperature[2].m_currentValue = -1;
			}		
		});

		rapiSender.sendCmd("$G7", [](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 4)
				{
					Data.m_temperatureMax[0] = strtol(rapiSender.getToken(1), NULL, 10) / 10.0;
					Data.m_temperatureMax[1] = strtol(rapiSender.getToken(2), NULL, 10) / 10.0;
					Data.m_temperatureMax[2] = strtol(rapiSender.getToken(3), NULL, 10) / 10.0;
				} 
			}
			else {
				Data.m_temperatureMax[0] = -1;
				Data.m_temperatureMax[1] = -1;
				Data.m_temperatureMax[2] = -1;
			}
		});
	}
}

void customSetup() {

	Serial.begin(115200);

//	pinMode(ROTARY_ENCODER_A_PIN, INPUT_PULLUP);
//	pinMode(ROTARY_ENCODER_B_PIN, INPUT_PULLUP);

	//we must initialize rorary encoder 
	gRotaryEncoder.begin();
	gRotaryEncoder.setup([]{gRotaryEncoder.readEncoder_ISR();});
	//optionally we can set boundaries and if values should cycle or not
	gRotaryEncoder.setBoundaries(1, 20, false); //minValue, maxValue, cycle values (when max go to min and vice versa)
	gRotaryEncoder.disable();

	Keyboard.init();
}

char* createState(int index)
{
	switch (index)
	{
		case INDEX_ENERGY_LIMIT_SCREEN: 
		return reinterpret_cast<char*>(new EnergyLimitScreen(INDEX_ENERGY_LIMIT_SCREEN, INDEX_NO_SCREEN, INDEX_NO_SCREEN));

		default: return NULL;
	}

}

void processScreen()
{
	uint8_t key = Keyboard.getKey();
	if(key > KEY_PRESSED)
	{
		if(gCurScreenIndex == INDEX_NO_SCREEN)
		{
			gCurScreenIndex = INDEX_ENERGY_LIMIT_SCREEN;
			key = 0;
		}
	}

	if (gPrevScreenIndex != gCurScreenIndex)
	{
		if (gpCurScreen != NULL)
		{
			DBUGLN("Delete screen");
			delete gpCurScreen;
			gRotaryEncoder.disable();
		}
		gpCurScreen = (BaseScreen*)createState(gCurScreenIndex);
		if (gpCurScreen != NULL)
		{
			DBUGLN("Create screen");
			gpCurScreen->Init();
			gRotaryEncoder.enable();
		}
		gPrevScreenIndex = gCurScreenIndex;
	}

	if (gpCurScreen != NULL)
	{
		gCurScreenIndex = gpCurScreen->Process(key);
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

	processScreen();
}




