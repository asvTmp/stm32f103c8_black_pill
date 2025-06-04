#ifndef __MAIN_H
#define __MAIN_H

	#define F_CPU 		72000000UL	// Тактовая у нас 72МГЦ
	#define TimerTick  	F_CPU/1000-1	// Нам нужен килогерц

	int ClockInit(void);
	void BlinkLed(int);
	void delay_ms(int);
	void delay_t_delay(int);
	int SysTickInit(void);

#endif
