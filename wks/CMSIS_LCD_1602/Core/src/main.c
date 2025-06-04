#include "stm32f1xx.h"
#include "main.h"

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
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Включаем тактирование порта GPIOB
	GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_CNF12); //для начала все сбрасываем в ноль
	//MODE: выход с максимальной частотой 2 МГц
	//CNF: режим push-pull
	GPIOB->CRH |= (0x02 << GPIO_CRH_MODE12_Pos) | (0x00 << GPIO_CRH_CNF12_Pos);
}

void PortSetHi(void) {
	GPIOB->BSRR = (1<<12);
}

void PortSetLow(void) {
	GPIOB->BRR = (1<<12);
}

void delay_ms(int ms) {
    SysTick->LOAD = TimerTick;  // 72MHz / 1000
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

    for(uint32_t i = 0; i < ms; i++) {
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }

    SysTick->CTRL = 0;
}

void delay_t_delay(int t_delay) {
	for(int i=0; i<t_delay; i++);
}

int SysTickInit(void) {
	SysTick->CTRL = 0;
}


int main() {
	int status;
	int i;

	status = ClockInit();
	SysTickInit();
	PortInit();

	while(1) {
		delay_ms(500);
		PortSetHi();
		delay_ms(500);
		PortSetLow();
	}
}
