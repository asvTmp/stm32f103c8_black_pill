#ifndef __MAIN_H
#define __MAIN_H

// Определение пинов (Port B)
#define LCD_RS_PIN  6  // PB6
#define LCD_E_PIN   7  // PB7
#define LCD_D4_PIN  8  // PB8
#define LCD_D5_PIN  9  // PB9
#define LCD_D6_PIN  10 // PB10
#define LCD_D7_PIN  11 // PB11
#define LED_PIN     12 // PB12

// Макросы для работы с GPIO
#define LCD_RS_HIGH()   (GPIOB->BSRR = (1 << LCD_RS_PIN))
#define LCD_RS_LOW()    (GPIOB->BRR  = (1 << LCD_RS_PIN))
#define LCD_E_HIGH()    (GPIOB->BSRR = (1 << LCD_E_PIN))
#define LCD_E_LOW()     (GPIOB->BRR  = (1 << LCD_E_PIN))

#define SYSCLK_VALUE 48000

#endif
