# Функция задержки delay_ms, с использованием SysTick, без прерывания, по опросу регистра   

```c
int SysTickInit(void) {
	SysTick->CTRL = 0;
}

void delay_ms(int ms) {
    SysTick->LOAD = 72000;  // 72MHz / 1000
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;

    for(uint32_t i = 0; i < ms; i++) {
        while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }

    SysTick->CTRL = 0;
}

```
