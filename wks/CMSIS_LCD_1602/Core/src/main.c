#include "main.h"
#include "stm32f1xx.h"
#include "lcd_1602a.h"

int main(void) {
	main_lcd_1602a();

	int status;
	BlinkDelay delay_t = {START_DELAY, STEP_DELAY};

	while(1) {
		delay_t = blink_led(delay_t);
	}
}
