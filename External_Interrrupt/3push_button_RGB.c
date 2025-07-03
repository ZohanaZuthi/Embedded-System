#include "stm32f10x.h"

void GPIO_Config(void)
{
    // Enable clocks for GPIOA and AFIO
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    // PA0, PA1, PA2 as input pull-down
    GPIOA->CRL &= ~((0xF << 0) | (0xF << 4) | (0xF << 8)); // Clear CNF & MODE
    GPIOA->CRL |= ((0x8 << 0) | (0x8 << 4) | (0x8 << 8));  // CNF=10 (input pull), MODE=00
    GPIOA->ODR &= ~((1 << 0) | (1 << 1) | (1 << 2));       // Pull-downs enabled

    // PA5, PA6, PA7 as output push-pull (RGB)
    GPIOA->CRL &= ~((0xF << 20) | (0xF << 24) | (0xF << 28));
    GPIOA->CRL |= ((0x3 << 20) | (0x3 << 24) | (0x3 << 28)); // 50MHz push-pull
}

void EXTI_Config(void)
{
    // Map EXTI0, EXTI1, EXTI2 to PA0, PA1, PA2
    AFIO->EXTICR[0] &= ~((0xF << 0) | (0xF << 4) | (0xF << 8)); // PA is 0

    // Enable EXTI0, EXTI1, EXTI2
    EXTI->IMR |= (1 << 0) | (1 << 1) | (1 << 2);

    // Trigger on rising edge
    EXTI->RTSR |= (1 << 0) | (1 << 1) | (1 << 2);

    // Disable falling edge
    EXTI->FTSR &= ~((1 << 0) | (1 << 1) | (1 << 2));
}

void NVIC_Config(void)
{
    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void turn_off_all_colors(void)
{
    GPIOA->ODR &= ~((1 << 5) | (1 << 6) | (1 << 7));
}

// ------------ Interrupt Handlers ------------
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1 << 0))
    {
        EXTI->PR |= (1 << 0); // Clear flag
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 5); // Red
    }
}

void EXTI1_IRQHandler(void)
{
    if (EXTI->PR & (1 << 1))
    {
        EXTI->PR |= (1 << 1); // Clear flag
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 6); // Green
    }
}

void EXTI2_IRQHandler(void)
{
    if (EXTI->PR & (1 << 2))
    {
        EXTI->PR |= (1 << 2); // Clear flag
        turn_off_all_colors();
        GPIOA->ODR |= (1 << 7); // Blue
    }
}

// ------------ Main Function ------------
int main(void)
{
    GPIO_Config();
    EXTI_Config();
    NVIC_Config();

    while (1)
    {
        // Main loop idle — everything handled by interrupts
    }
}