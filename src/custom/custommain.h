
#ifndef _CUSTOMMAIN_h
#define _CUSTOMMAIN_h

#include <AiEsp32RotaryEncoder.h>  // https://github.com/igorantolic/ai-esp32-rotary-encoder.git
#include "Keyboard.h"

extern AiEsp32RotaryEncoder gRotaryEncoder;

void customLoop();
void customSetup();

#endif