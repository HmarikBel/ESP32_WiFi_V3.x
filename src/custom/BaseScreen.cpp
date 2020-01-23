#include "BaseScreen.h"
//#include "GlobalsObjects.h"
#include "Keyboard.h"
#include <Time.h>

uint8_t BaseScreen::Process(uint8_t key)
{
	if(key == SHORT_KEY)
	{
		return m_next;
	}

	if (millis() > m_timeToReturn)
	{
		return INDEX_NO_SCREEN;
	}

	return GetStateIndex();
}

uint8_t BaseScreen::GetStateIndex()
{
	return -1;
};