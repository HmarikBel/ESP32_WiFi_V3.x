// 
// 
// 

#include "Keyboard.h"
#include "defines.h"

#define DEBOUNCE_COUNT 5 
#define SHORT_KEY_INTERVAL 100
#define LONG_KEY_INTERVAL 2000


uint8_t KeyboardClass::init()
{
	pinMode(KEYBOARD_PIN, INPUT);

	m_prevCheck = millis();
}

uint8_t KeyboardClass::getKey()
{
	uint8_t key = 0;
	if (isPressed() && (millis() - m_timePressed <= LONG_KEY_INTERVAL))
	{
	//	Serial.println("key pressed");
		if (m_timePressed == 0)
		{
			m_timePressed = millis();
		}
		return KEY_PRESSED;
	}
	else
	{
		if (m_timePressed > 0)
		{
			if (millis() - m_timePressed > LONG_KEY_INTERVAL)
			{
				key = LONG_KEY;
			}
			else if (millis() - m_timePressed > SHORT_KEY_INTERVAL)
			{
				key = SHORT_KEY;
			}
		}

		m_timePressed = 0;
	}

	return key;
}

bool KeyboardClass::isPressed()
{
	// ���� �� ������� � ��������� ������������
	if (millis() != m_prevCheck)
	{
		int reading = digitalRead(KEYBOARD_PIN);

//		Serial.print("reading ");
//		Serial.println(reading);

		if (reading == m_prevState && m_counter > 0)
		{
			m_counter--;
		}
		if (reading != m_prevState)
		{
			m_counter++;
		}
		// ���� ���� ���������� ���� �������� ���������� �����, ������� ���������� ���
		if (m_counter >= DEBOUNCE_COUNT)
		{
			m_counter = 0;
			m_prevState = reading;
		}
		m_prevCheck = millis();
	}

	return m_prevState == 0;
}


KeyboardClass Keyboard;

