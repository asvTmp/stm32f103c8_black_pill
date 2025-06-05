#ifndef __MAIN_H
#define __MAIN_H

	#define F_CPU 		72000000UL	// Тактовая у нас 72МГЦ
	#define TimerTick  	F_CPU/1000-1	// Нам нужен килогерц

	int ClockInit(void);
	void BlinkLed(int);
	void delay_ms(int);
	void delay_t_delay(int);
	int SysTickInit(void);

	// Определение пинов
	#define LCD_RS_PIN  14  // PB14
	#define LCD_RW_PIN  13  // PB13
	#define LCD_E_PIN   15  // PB15
	#define LCD_D4_PIN  2  // PA2
	#define LCD_D5_PIN  3  // PA3
	#define LCD_D6_PIN  4  // PA4
	#define LCD_D7_PIN  5  // PA5

	// Макросы для работы с GPIO
	#define LCD_RS_HIGH()   (GPIOB->BSRR = (1 << LCD_RS_PIN))
	#define LCD_RS_LOW()    (GPIOB->BRR  = (1 << LCD_RS_PIN))
	#define LCD_RW_HIGH()   (GPIOB->BSRR = (1 << LCD_RW_PIN))
	#define LCD_RW_LOW()    (GPIOB->BRR  = (1 << LCD_RW_PIN))
	#define LCD_E_HIGH()    (GPIOB->BSRR = (1 << LCD_E_PIN))
	#define LCD_E_LOW()     (GPIOB->BRR  = (1 << LCD_E_PIN))

#endif
