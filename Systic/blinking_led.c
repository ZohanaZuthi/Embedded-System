#include "stm32f10x.h"

// ------------ SysTick Initialization ------------
void systick_init(void)
{
    SysTick->LOAD = 72000 - 1; // (72MHz / 1000) - 1 = 1 ms
    SysTick->VAL = 0;          // Clear current value register
    SysTick->CTRL = 5;         // Enable SysTick, use processor clock (no interrupt)
}

// ------------ Delay 1 ms ------------
void delay_ms(void)
{
    while (!(SysTick->CTRL & (1 << 16)))
        ; // Wait until COUNTFLAG is set
}

// ------------ Delay t ms ------------
void delay_t(uint32_t t)
{
    while (t--)
    {
        delay_ms();
    }
}

// ------------ GPIOA Pin 9 Configuration (PA9) ------------
void GPIO_config(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Enable GPIOA clock

    GPIOA->CRH &= ~(0xF << 4); // Clear config for PA9 (bits 7:4)
    GPIOA->CRH |= (0x2 << 4);  // MODE9 = 10 (Output, 2 MHz), CNF9 = 00 (Push-pull)
}

int main(void)
{
    systick_init(); // Initialize SysTick for delays
    GPIO_config();  // Set up PA9 as output

    while (1)
    {
        GPIOA->ODR |= (1 << 9); // Set PA9 HIGH (LED ON)
        delay_t(3000);          // Delay 3000 ms (3 seconds)

        GPIOA->ODR &= ~(1 << 9); // Set PA9 LOW (LED OFF)
        delay_t(3000);           // Delay 3000 ms (3 seconds)
    }
}
