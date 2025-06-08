/*
 * lcd_1602a.h
 *
 *  Created on: Jun 8, 2025
 *      Author: asv
 */

#include "stm32f1xx.h"

#ifndef INC_LCD_1602A_H_
#define INC_LCD_1602A_H_

	#define F_CPU 			72000000UL	// Тактовая у нас 72МГЦ
	#define TimerTick  		F_CPU/1000-1	// Нам нужен килогерц
	#define TimerTickUs  	F_CPU/1000000-1	// Нам нужен килогерц

	#define START_DELAY 500
	#define STEP_DELAY 0
	#define MAX_DELAY 1000
	#define MIN_DELAY 100

	typedef struct {
	    int value;
	    int step;
	} BlinkDelay;

	int ClockInit(void);
	void PortInit(void);
	void PortSetHi(void);
	void PortSetLow(void);
	void BlinkLed(int);
	void delay_us(int);
	void delay_ms(int);
	void SysTickInit(void);

	void LCD_Write4Bit(uint8_t data);
	void LCD_PulseEnable();
	void LCD_Send(uint8_t value, uint8_t mode);
	void LCD_Send_CMD(uint8_t cmd);
	void LCD_Send_DATA(uint8_t value);
	void LCD_Send_DATA(uint8_t value);
	void LCD_init();
	void LCD_Print(const char *str);
	void LCD_clear();
	void blink_led_one(int t);



	BlinkDelay blink_led(BlinkDelay);
	int main_lcd_1602a();

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

#endif /* INC_LCD_1602A_H_ */
