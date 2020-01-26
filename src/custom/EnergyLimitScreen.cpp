#include "EnergyLimitScreen.h"
#include "lcd.h"
#include <Time.h>
#include "RapiSender.h"
#include <AiEsp32RotaryEncoder.h>  // https://github.com/igorantolic/ai-esp32-rotary-encoder.git
#include "Keyboard.h"
#include "debug.h"

extern RapiSender rapiSender;
extern AiEsp32RotaryEncoder gRotaryEncoder;

uint8_t EnergyLimitScreen::Process(uint8_t key)
{
	if(m_limit != gRotaryEncoder.readEncoder() / 2)
	{
		m_limit = gRotaryEncoder.readEncoder() / 2;
		ResetEnterTime();
		Show(true);
		delay(300);	
	}

	if(key == LONG_KEY)
	{
		DBUGLN("Long Key write Limit");

		lcd_display(F("Writing..."), 0, 1, 10*1000, LCD_DISPLAY_NOW);
		rapiSender.sendCmd(String(F("$SH ")) + String(m_limit), [this](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				lcd_display(F("Write done"), 0, 1, 1000, LCD_DISPLAY_NOW);
				this->m_writeDone = true;
			}
			else 
			{
				lcd_display(F("Write error"), 0, 1, 1000, LCD_DISPLAY_NOW);
				this->Show(false);
			}		
		});
	}

	if(m_writeDone || key == SHORT_KEY)
	{
		lcd_display("Exit", 0, 0, 0, LCD_DISPLAY_NOW);
		lcd_display("Exit", 0, 1, 0, LCD_DISPLAY_NOW);
		return INDEX_NO_SCREEN;
	}

	return BaseScreen::Process(key);
}

void EnergyLimitScreen::Init()
{
	DBUGLN("Init EnergyLimitScreen");

	gRotaryEncoder.setBoundaries(0, 20 * 2, false); //minValue, maxValue, cycle values (when max go to min and vice versa)

	lcd_display(F("Energy limit "), 0, 0, 30*1000, LCD_CLEAR_LINE | LCD_DISPLAY_NOW);
	delay(100);

	rapiSender.sendCmd("$GH", [this](int ret) {
			if (RAPI_RESPONSE_OK == ret)
			{
				if(rapiSender.getTokenCnt() >= 2)
				{
					this->m_limit = strtol(rapiSender.getToken(1), NULL, 10);
					if(this->m_limit == 0)
					{
						this->m_limit = 6;
					}
					gRotaryEncoder.reset(this->m_limit * 2);
					delay(100);	
					this->Show(true);		
				} 
			}
			else 
			{
				this->m_limit = 0;
				gRotaryEncoder.reset(0);
				lcd_display(F("Read error"), 0, 1, 1000, LCD_DISPLAY_NOW);
				this->Show(false);
			}		
		});

    Show(true);
}

void EnergyLimitScreen::Show(bool displayNow)
{
	DBUG("Show limit ");
	DBUGLN(m_limit);
	String msg = String(m_limit) + "   ";
	lcd_display(msg, 0, 1, 30*1000, displayNow ? LCD_DISPLAY_NOW : 0);
}
