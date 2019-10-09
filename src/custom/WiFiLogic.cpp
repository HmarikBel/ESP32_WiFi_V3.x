// 
// 
// 

#include "WiFiLogic.h"

#include <WebServer.h>
extern WebServer server;

#ifdef ESP32
#include <SPIFFS.h>
#endif

const char* serveraddress = "hmarikutility.somee.com";
//const char* serveraddress = "mysensors.azurewebsites.net";

#define DEBUG_SERIAL

bool WiFiLogicClass::SendData(int sensorId, float value)
{
#ifdef DEBUG_SERIAL
	Serial.println(F("Send data"));
#endif
	if (WiFi.status() != WL_CONNECTED)
	{
		return false;
	}

	WiFiClient client;

	if (!client.connect(serveraddress, 80))
	{
#ifdef DEBUG_SERIAL
		Serial.println("Connection failed");
#endif
		return false;
	}

	String url = "/api/sensors?id=";
	url += sensorId;
	url += "&value=";
	url += value;
#ifdef DEBUG_SERIAL
	Serial.print("Requesting URL: ");
	Serial.println(url);
#endif
	// This will send the request to the server
	client.print(String("GET ") + url + " HTTP/1.1\r\n" +
		"Host: " + serveraddress + "\r\n" +
		"Connection: close\r\n\r\n");
	delay(10);

	// Read all the lines of the reply from server and print them to Serial
	while (client.available())
	{
		String line = client.readStringUntil('\r');
#ifdef DEBUG_SERIAL
		Serial.print(line);
#endif
	}
#ifdef DEBUG_SERIAL
	Serial.println();
	Serial.println("closing connection");
#endif
	return true;
}

#define OVERFLOW_CHECK					10000000

bool WiFiLogicClass::SendIfNecessary(int sensorId, SendDataFloat* data, float diff, unsigned long minInterval, unsigned long maxInterval, bool checkResult)
{
	return SendIfNecessary(sensorId, data->m_currentValue, &(data->m_sentValue), diff, &(data->m_nextSend), minInterval, maxInterval, checkResult);
}

bool WiFiLogicClass::SendIfNecessary(int sensorId, float value, float* pSentValue, float diff, unsigned long* pNextSend,
	unsigned long minInterval, unsigned long maxInterval, bool checkResult)
{
//	Serial.println(F("SendIfNecessaryFloat"));
	unsigned long tmpNow = millis();

	if (tmpNow >= *pNextSend && tmpNow - *pNextSend < OVERFLOW_CHECK)
	{
		if (abs(*pSentValue - value) > diff || tmpNow - *pNextSend + minInterval >= maxInterval)
		{
			if (WiFiLogic.SendData(sensorId, value) || !checkResult)
			{
				*pSentValue = value;

				if (*pNextSend + minInterval > tmpNow)
				{
					*pNextSend = *pNextSend + minInterval;
				}
				else 
				{
					*pNextSend = tmpNow + minInterval;
				}
			}
		}
	}

	return true;
}

bool WiFiLogicClass::SendIfNecessary(int sensorId, SendDataInt* data, int diff, unsigned long minInterval, unsigned long maxInterval, bool checkResult)
{
	return SendIfNecessary(sensorId, data->m_currentValue, &(data->m_sentValue), diff, &(data->m_nextSend), minInterval, maxInterval, checkResult);
}

bool WiFiLogicClass::SendIfNecessary(int sensorId, int value, int* pSentValue, int diff, unsigned long* pNextSend,
	unsigned long minInterval, unsigned long maxInterval, bool checkResult)
{
//	Serial.println(F("SendIfNecessaryInt"));
	float sentValue = *pSentValue;
	SendIfNecessary(sensorId, (float)value, &sentValue, (float)diff, pNextSend, minInterval, maxInterval, checkResult);
	*pSentValue = sentValue;
	return sentValue;
}



bool WiFiLogicClass::formatCurrentDateTime(char* buffer)
{
#ifdef ESP8266
	sprintf_P(buffer, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"), year(), month(), day(), hour(), minute(), second());
#else
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo, 100)) {
		return false;
	}

	sprintf_P(buffer, PSTR("%04d-%02d-%02dT%02d:%02d:%02d"), timeinfo.tm_year, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
#endif

	return true;
}

bool WiFiLogicClass::formatCurrentDateTimeToDisplay(char* buffer)
{
#ifdef ESP8266
	sprintf_P(buffer, PSTR("%02d.%02d.%02d %02d:%02d:%02d"), day(), month(), year() - 2000, hour(), minute(), second());
#else
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo, 100)) {
		strcpy_P(buffer, PSTR("not set"));
		return false;
	}

	formatDateTimeToDisplay(buffer, timeinfo);
	//sprintf_P(buffer, PSTR("%02d.%02d.%02d %02d:%02d:%02d"), timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year - 2000, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
#endif
	return true;
}

#ifndef ESP8266
void WiFiLogicClass::formatDateTimeToDisplay(char* buffer, tm timeinfo)
{
	sprintf_P(buffer, PSTR("%02d.%02d.%02d %02d:%02d:%02d"), 
		timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year > 2000 ? timeinfo.tm_year - 2000 : timeinfo.tm_year - 100, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}
#endif

void WiFiLogicClass::formatUptime(char* s)
{
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;
	int d = hr / 24;

	snprintf(s, 15, "%d.%02d:%02d:%02d", d, hr % 24, min % 60, sec % 60);
}

void WiFiLogicClass::formatDateTimeToDisplay(char* buffer, unsigned long epoch)
{
#ifdef ESP8266
	tmElements_t tm;
	breakTime(epoch, tm);  // break time_t into elements

	sprintf_P(buffer, PSTR("%02d.%02d.%02d %02d:%02d:%02d"), tm.Day, tm.Month, 1970 + tm.Year, tm.Hour, tm.Minute, tm.Second);
#else
	struct tm timeinfo;
	time_t t = epoch;
	localtime_r(&t, &timeinfo);
	formatDateTimeToDisplay(buffer, timeinfo);
#endif
}

void WiFiLogicClass::sprintf_P_Float(char* s, float value)
{
	if (isnan(value))
	{
		sprintf_P(s, PSTR("\"NaN\""));
	}
	else if (abs(value) < 10)
	{
		sprintf_P(s, PSTR("%.02f"), value);
	}
	else if (abs(value) < 100)
	{
		sprintf_P(s, PSTR("%.1f"), value);
	}
	else
	{
		sprintf_P(s, PSTR("%d"), (int)value);
	}
}

String WiFiLogicClass::addParamInt(String data, String paramName, String value, bool first)
{
	return data += (first ? "\"" : ",\"") + paramName + "\":" + value;
}

String WiFiLogicClass::addParam(String data, String paramName, String value, bool first)
{
	return addParamInt(data, paramName, "\"" + value + "\"", first);
}

String WiFiLogicClass::addParam(String data, String paramName, float value, bool first)
{
	char t[7]; sprintf_P_Float(t, value);
	return addParamInt(data, paramName, String(t), first);
}

WiFiLogicClass WiFiLogic;

