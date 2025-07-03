#include "stm32f10x.h"

// ------------ Delay Function ------------
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 8000; i++)
        __NOP(); // Approximate delay (for 8 MHz)
}

// ------------ GPIO Configuration ------------
void GPIO_Config(void)
{
    // Enable clock for GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // PA5, PA6, PA7 as output push-pull (RGB)
    GPIOA->CRL &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOA->CRL |= ((0x3 << 20) | (0x3 << 24) | (0x3 << 28)); // Output 50MHz push-pull
}

// ------------ Turn Off All Colors ------------
void turn_off_all_colors(void)
{
    GPIOA->ODR &= ~((1 << 5) | (1 << 6) | (1 << 7));
}

// ------------ Main Function ------------
int main(void)
{
    GPIO_Config();

    while (1)
    {
        // Red
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 5);
        delay_ms(500);

        // Green
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 6);
        delay_ms(500);

        // Blue
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 7);
        delay_ms(500);
    }
}
