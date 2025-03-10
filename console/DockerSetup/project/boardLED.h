#ifndef __ErrorSig__
#define __ErrorSig__

#include "hardware/gpio.h"

void initBoardLED(){
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN,0);
}
void showLED(){
	gpio_put(PICO_DEFAULT_LED_PIN, 1);
}
void hideLED(){
	gpio_put(PICO_DEFAULT_LED_PIN, 0);
}


#endif