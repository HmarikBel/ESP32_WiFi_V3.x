#define OVERFLOW_CHECK					1000000

#define INTERVAL_REPEAT_SYNC_TIME 1000*1
#define INTERVAL_TO_SYNC_TIME 1000*60*60*3

#define READ_DATA_MSEC 5000


#define COUNTER_ID_CHARGER_ENERGY_TOTAL	4000
#define COUNTER_ID_CHARGER_ENERGY		4001
#define COUNTER_ID_CHARGER_POWER		4002
#define COUNTER_ID_CHARGER_VOLTAGE		4003
#define COUNTER_ID_CHARGER_CURRENT		4004
#define COUNTER_ID_CHARGER_TEMPERATURE	4005 // + 3


#define SEND_DATA_MIN					60000
#define SEND_DATA_MAX					600000

#define SCREEN_TIMEOUT		    	30000
#define INDEX_NO_SCREEN	    	    0
#define INDEX_ENERGY_LIMIT_SCREEN	1
#define INDEX_SCREEN_MAX		    1

/*
connecting Rotary encoder
CLK (A pin) - to any microcontroler intput pin with interrupt -> in this example pin 32
DT (B pin) - to any microcontroler intput pin with interrupt -> in this example pin 21
SW (button pin) - to any microcontroler intput pin -> in this example pin 25
VCC - to microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) or in this example pin 25
GND - to microcontroler GND
*/

#define ROTARY_ENCODER_A_PIN 21
#define ROTARY_ENCODER_B_PIN 19
//#define ROTARY_ENCODER_BUTTON_PIN 25
#define ROTARY_ENCODER_VCC_PIN -1//27 /*put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */

#define KEYBOARD_PIN                22
