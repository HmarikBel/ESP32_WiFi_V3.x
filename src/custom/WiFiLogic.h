// WiFiLogic.h

#ifndef _WIFILOGIC_h
#define _WIFILOGIC_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <FS.h>

template <typename ValueType> 
class SendData
{

public:
	SendData()
	{
		m_nextSend = 0;
	}

	ValueType m_currentValue;
	ValueType m_sentValue;

	unsigned long m_nextSend;
};

//using SendDataFloat = SendData<float>;
typedef SendData<float> SendDataFloat;

typedef SendData<int> SendDataInt;

class WiFiLogicClass
{
	bool m_listPrinted = false;
	bool m_statusPrinted = false;
	byte m_connectAttempt = 0;
	unsigned long m_prevTimeCheck = 0;

	static String addParamInt(String data, String paramName, String value, bool first);
 protected:


 public:
	 bool SendData(int sensorId, float value);

	 bool SendIfNecessary(int sensorId, float value, float* pSentValue, float diff, unsigned long* pNextSend,
		 unsigned long minInterval, unsigned long maxInterval, bool checkResult = false);
	 bool SendIfNecessary(int sensorId, int value, int* pSentValue, int diff, unsigned long* pNextSend,
		 unsigned long minInterval, unsigned long maxInterval, bool checkResult = false);
		 
	 bool SendIfNecessary(int sensorId, SendDataFloat* data, float diff, unsigned long minInterval, unsigned long maxInterval, bool checkResult = false);
	bool SendIfNecessary(int sensorId, SendDataInt* data, int diff, unsigned long minInterval, unsigned long maxInterval, bool checkResult = false);

	 static void sprintf_P_Float(char* s, float value);

	 static void formatUptime(char* s);
	 static bool formatCurrentDateTime(char* buffer);
	 static bool formatCurrentDateTimeToDisplay(char* buffer);
#ifndef ESP8266
	 static void formatDateTimeToDisplay(char* buffer, tm timeinfo);
#endif
	 static void formatDateTimeToDisplay(char* buffer, unsigned long epoch);


	 static String addParam(String data, String paramName, String value, bool first = false);
	 static String addParam(String data, String paramName, float value, bool first = false);

};

extern WiFiLogicClass WiFiLogic;

#endif

