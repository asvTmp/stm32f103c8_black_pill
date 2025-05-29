#include "stm32f1xx.h"
#include "main.h"

void PortInit(void) {
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Включаем тактирование порта GPIOB
	GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); //для начала все сбрасываем в ноль
	//MODE: выход с максимальной частотой 2 МГц
	//CNF: режим push-pull
	GPIOB->CRH |= (0x02 << GPIO_CRH_MODE12_Pos) | (0x00 << GPIO_CRH_CNF12_Pos);
}

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

void PortBSetHi(int pin_num) {
	GPIOB->BSRR = (1<<pin_num);
}

void PortBSetLow(int pin_num) {
	GPIOB->BRR = (1<<pin_num);
}

// Точная задержка на SysTick (72 МГц)
void delay_ms(uint32_t ms) {
    SysTick->LOAD = 72000;  // 72MHz / 1000
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;

    for(uint32_t i = 0; i < ms; i++) {
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
    SysTick->CTRL = 0;
}

int main() {
	int status;
	int t_delay = 0x200000;

	//status = ClockInit();

    // Настройка тактирования (72 МГц)
    RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL x9 (8MHz * 9 = 72MHz)
    RCC->CFGR |= RCC_CFGR_PPRE1_2;  // APB1 = 36MHz
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    // Настройка SysTick
    SysTick->CTRL = 0;

	PortInit();

	while(1) {
		PortBSetHi(LED_PIN);
		//for(int i=0; i<t_delay; i++);
		delay_ms(1000);
		PortBSetLow(LED_PIN);
		//for(int i=0; i<t_delay; i++);
		delay_ms(1000);

	}
}
