#include "main.h"

#define START_DELAY 100
#define STEP_DELAY 10
#define MAX_DELAY 200
#define MIN_DELAY 10

typedef struct {
    int value;
    int step;
} BlinkDelay;

void blink_led_one(int t) {
	delay_ms(t);
	PortSetHi();
	delay_ms(t);
	PortSetLow();
}

BlinkDelay blink_led(BlinkDelay delay_x) {
	blink_led_one(delay_x.value);

	if (delay_x.value > MAX_DELAY) delay_x.step = -STEP_DELAY;
	if (delay_x.value < MIN_DELAY) delay_x.step = STEP_DELAY;

	delay_x.value += delay_x.step;

	return delay_x;
}

int main(void) {
	main_lcd_1602a();

	int status;
	BlinkDelay delay_t = {START_DELAY, STEP_DELAY};

	while(1) {
		delay_t = blink_led(delay_t);
	}
}
