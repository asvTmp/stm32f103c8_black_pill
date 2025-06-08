#include "stm32f1xx.h"
#include "lcd_1602a.h"

int ClockInit(void) {

	RCC->CR |= (1<<RCC_CR_HSEON_Pos); //Запускаем генератор HSE

	__IO int StartUpCounter;
	//Ждем успешного запуска или окончания тайм-аута
	for(StartUpCounter=0; ; StartUpCounter++) {
		//Если успешно запустилось, то выходим из цикла
		if(RCC->CR & (1<<RCC_CR_HSERDY_Pos))
			break;
		//Если не запустилось, то отключаем все, что включили, возвращаем ошибку
		if(StartUpCounter > 0x1000)	{
			RCC->CR &= ~(1<<RCC_CR_HSEON_Pos); //Останавливаем HSE
			return 1;
		}
	}

	//Настраиваем PLL
	RCC->CFGR 	|= (0x07<<RCC_CFGR_PLLMULL_Pos) //PLL множитель равен 9
				| (0x01<<RCC_CFGR_PLLSRC_Pos); //Тактирование PLL от HSE

	RCC->CR |= (1<<RCC_CR_PLLON_Pos); //Запускаем PLL


	//Ждем успешного запуска или окончания тайм-аута
	for(StartUpCounter=0; ; StartUpCounter++) {
		//Если успешно запустилось, то выходим из цикла
		if(RCC->CR & (1<<RCC_CR_PLLRDY_Pos))
			break;

		//Если по каким-то причинам не запустился PLL, то отключаем все, что включили и возвращаем ошибку
		if(StartUpCounter > 0x1000) {
			RCC->CR &= ~(1<<RCC_CR_HSEON_Pos); //Останавливаем HSE
			RCC->CR &= ~(1<<RCC_CR_PLLON_Pos); //Останавливаем PLL
			return 2;
		}
	}

	//Устанавливаем 2 цикла ожидания для Flash
	//так как частота ядра у нас будет 48 MHz < SYSCLK <= 72 MHz
	FLASH->ACR |= (0x02<<FLASH_ACR_LATENCY_Pos);

	//Делители
	RCC->CFGR	|= (0x00<<RCC_CFGR_PPRE2_Pos) //Делитель шины APB2 отключен (оставляем 0 по умолчанию)
				|  (0x04<<RCC_CFGR_PPRE1_Pos) //Делитель нишы APB1 равен 2
				|  (0x00<<RCC_CFGR_HPRE_Pos); //Делитель AHB отключен (оставляем 0 по умолчанию)

	RCC->CFGR |= (0x02<<RCC_CFGR_SW_Pos); //Переключаемся на работу от PLL

	//Ждем, пока переключимся
	while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0x02<<RCC_CFGR_SWS_Pos)) {
	}

	//отключаем внутренний RC-генератор
	RCC->CR &= ~(1<<RCC_CR_HSION_Pos);

	//Настройка и переклбючение сисемы на внешний кварцевый генератор и PLL запершилось успехом.
	return 0;
}

void PortInit(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    // Настройка PB12 (LED) PB14 (RS), PB13 (RW), PB15 (E) как выходы push-pull, 2 MHz
    GPIOB->CRH &= ~(GPIO_CRH_CNF12| GPIO_CRH_CNF13 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15);
    GPIOB->CRH |= (GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE14 | GPIO_CRH_MODE15);

    // Настройка PA2-PA5 (D4-D7) как выходы push-pull, 2 MHz
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);
    GPIOA->CRL |= (GPIO_CRL_MODE2 | GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
}

void PortSetHi(void) {
	GPIOB->BSRR = (1<<12);
}

void PortSetLow(void) {
	GPIOB->BRR = (1<<12);
}

void delay_us_st(int us) {
    SysTick->LOAD = TimerTickUs;  // 72MHz / 1000000
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

    for(uint32_t i = 0; i < us; i++) {
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }

    SysTick->CTRL = 0;
}

// Задержка в микросекундах
void delay_us_tt(uint32_t us) {
    us *= 8; // Примерная калибровка для 72 MHz
    while(us--) {
        __NOP();
    }
}

void delay_us(int us) {
	delay_us_st(us);
}

void delay_ms(int ms) {
	for (int  i = 0; i < ms; i++) {
		delay_us(1000);
	}
}
void delay_t_delay(int t_delay) {
	for(int i=0; i<t_delay; i++);
}

void SysTickInit(void) {
	SysTick->CTRL = 0;
}

// Установка 4 бит данных
void LCD_Write4Bits(uint8_t data) {
    // Установка битов данных
    if(data & 0x01) GPIOA->BSRR = (1 << LCD_D4_PIN);
    else GPIOA->BRR = (1 << LCD_D4_PIN);

    if(data & 0x02) GPIOA->BSRR = (1 << LCD_D5_PIN);
    else GPIOA->BRR = (1 << LCD_D5_PIN);

    if(data & 0x04) GPIOA->BSRR = (1 << LCD_D6_PIN);
    else GPIOA->BRR = (1 << LCD_D6_PIN);

    if(data & 0x08) GPIOA->BSRR = (1 << LCD_D7_PIN);
    else GPIOA->BRR = (1 << LCD_D7_PIN);
}

// Импульс на Enable
void LCD_PulseEnable() {
    LCD_E_HIGH();
    delay_us(1);
    LCD_E_LOW();
    delay_us(100);
}

// Отправка команды/данных
void LCD_Send(uint8_t value, uint8_t mode) {
    if(mode) LCD_RS_HIGH();
    else LCD_RS_LOW();

    LCD_Write4Bits(value >> 4); // Старшие 4 бита
    LCD_PulseEnable();

    LCD_Write4Bits(value);      // Младшие 4 бита
    LCD_PulseEnable();

    delay_ms(1);
}

// Отправка команды
void LCD_Send_CMD(uint8_t cmd) {
    LCD_RS_LOW();
    LCD_RW_LOW();

    LCD_Write4Bits(cmd >> 4); // Старшие 4 бита
    LCD_PulseEnable();

    LCD_Write4Bits(cmd);      // Младшие 4 бита
    LCD_PulseEnable();

    if(cmd == 0x01 || cmd == 0x02) delay_ms(2); // Задержка для команд очистки и возврата
    else delay_us(100);

}

// Отправка данных
void LCD_Send_DATA(uint8_t value) {
	LCD_RS_HIGH();
	LCD_RW_LOW();

    LCD_Write4Bits(value >> 4); // Старшие 4 бита
    LCD_PulseEnable();

    LCD_Write4Bits(value);      // Младшие 4 бита
    LCD_PulseEnable();

    delay_us(100);
}

void LCD_init() {
	PortInit();
	delay_ms(50);

	LCD_Write4Bits(0x03);
	LCD_PulseEnable();
	delay_ms(5);
	LCD_Write4Bits(0x03);
	LCD_PulseEnable();
    delay_us(100);
    LCD_Write4Bits(0x03);
    LCD_PulseEnable();

    LCD_Write4Bits(0x02);
    LCD_PulseEnable();

    // Конфигурация дисплея
    LCD_Send_CMD(0x28); // 4 бита, 2 строки, шрифт 5x8
    LCD_Send_CMD(0x0c); // Дисплей включен, курсор выключен
    LCD_Send_CMD(0x06); // Автоинкремент курсора
    LCD_Send_CMD(0x01); // Очистка экрана
    delay_ms(2);
}

// Вывод строки
void LCD_Print(const char *str) {
    while(*str) {
        LCD_Send(*str++, 1);
    }
}

void LCD_clear() {
	LCD_Send_CMD(0x01);
}

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

// Установка позиции курсора
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;
    if(row == 0) address = 0x80 + col;
    else address = 0xC0 + col;
    LCD_Send_CMD(address);
}

int main_lcd_1602a() {
	int status;
	status = ClockInit();
	SysTickInit();
	LCD_init();

	LCD_SetCursor(0, 2); // Первая строка
	LCD_Print("Black Pill");
	LCD_SetCursor(1, 4); // Вторая строка
	LCD_Print("STM32F103C8");

	return 0;
}
