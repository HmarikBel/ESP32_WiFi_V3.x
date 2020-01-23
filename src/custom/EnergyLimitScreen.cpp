#include "EnergyLimitScreen.h"
#include "lcd.h"
#include <Time.h>
#include "RapiSender.h"
#include <AiEsp32RotaryEncoder.h>  // https://github.com/igorantolic/ai-esp32-rotary-encoder.git
#include "Keyboard.h"

extern RapiSender rapiSender;
extern AiEsp32RotaryEncoder gRotaryEncoder;

uint8_t EnergyLimitScreen::Process(uint8_t key)
{
	if(m_limit != gRotaryEncoder.readEncoder())
	{
		m_limit = gRotaryEncoder.readEncoder();
		Show();
	}

	if(key == LONG_KEY)
	{
		lcd_display(F("Write"), 0, 1, 5 * 1000, LCD_CLEAR_LINE);
		rapiSender.sendCmd("$SH", [this](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				lcd_display(F("Write done"), 0, 1, 5 * 1000, LCD_CLEAR_LINE);
				this->m_writeDone = true;
			}
			else 
			{
				lcd_display(F("Write error"), 0, 1, 5 * 1000, LCD_CLEAR_LINE);
				this->m_limit = -1;
				gRotaryEncoder.reset(0);
				this->Show();
			}		
		});
	}

	if(m_writeDone)
	{
		return INDEX_NO_SCREEN;
	}

	return BaseScreen::Process(key);
}

void EnergyLimitScreen::Init()
{
	gRotaryEncoder.setBoundaries(1, 20, false); //minValue, maxValue, cycle values (when max go to min and vice versa)

	lcd_display(F("Set energy limit"), 0, 0, 0, LCD_CLEAR_LINE);

	rapiSender.sendCmd("$GH", [this](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 2)
				{
					this->m_limit = strtol(rapiSender.getToken(1), NULL, 10);
					gRotaryEncoder.reset(this->m_limit);	
					this->Show();		
				} 
			}
			else 
			{
				this->m_limit = -1;
				gRotaryEncoder.reset(0);
				this->Show();
			}		
		});

    Show();
}

void EnergyLimitScreen::Show()
{
	String msg = String(m_limit);
	lcd_display(msg, 0, 1, 30 * 1000, LCD_CLEAR_LINE);
}
