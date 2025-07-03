#include "stm32f10x.h" // Device header

// ------------ GPIO Configuration ------------
void GPIO_Config(void)
{
// Enable clocks for GPIOA and AFIO
RCC->APB2ENR |= (1UL << 2) | (1UL << 0);

    // ---- Buttons: PA0 and PA1 as input floating ----
    // PA0
    GPIOA->CRL &= ~(0xF << 0);  // Clear mode & CNF for PA0
    GPIOA->CRL |= (0x4 << 0);   // Input floating

    // PA1
    GPIOA->CRL &= ~(0xF << 4);  // Clear mode & CNF for PA1
    GPIOA->CRL |= (0x4 << 4);   // Input floating

    // ---- LEDs: PA5 and PA6 as output push-pull ----
    // PA5
    GPIOA->CRL &= ~(0xF << 20); // Clear mode & CNF for PA5
    GPIOA->CRL |= (0x3 << 20);  // Output push-pull, 50 MHz

    // PA6
    GPIOA->CRL &= ~(0xF << 24); // Clear mode & CNF for PA6
    GPIOA->CRL |= (0x3 << 24);  // Output push-pull, 50 MHz

}

// ------------ External Interrupt Configuration (PA0 & PA1) ------------
void EXTI_Config(void)
{
// ---- Map EXTI0 to PA0 ----
AFIO->EXTICR[0] &= ~(0xF << 0); // EXTI0 -> PA0

    // ---- Map EXTI1 to PA1 ----
    AFIO->EXTICR[0] &= ~(0xF << 4); // EXTI1 -> PA1

    // ---- Unmask EXTI lines ----
    EXTI->IMR |= (1 << 0); // Unmask EXTI0
    EXTI->IMR |= (1 << 1); // Unmask EXTI1

    // ---- Enable Rising and Falling edge triggers ----
    EXTI->RTSR |= (1 << 0) | (1 << 1);
    EXTI->FTSR |= (1 << 0) | (1 << 1);

}

// ------------ NVIC Configuration ------------
void NVIC_Config(void)
{
NVIC_EnableIRQ(EXTI0_IRQn); // Enable EXTI0 interrupt
NVIC_EnableIRQ(EXTI1_IRQn); // Enable EXTI1 interrupt
}

// ------------ Interrupt Handlers ------------

// PA0 ? LED1
void EXTI0_IRQHandler(void)
{
if (EXTI->PR & (1 << 0))
{
EXTI->PR |= (1 << 0); // Clear pending bit

        if (GPIOA->IDR & (1 << 0))
            GPIOA->ODR |= (1 << 5); // LED1 ON
        else
            GPIOA->ODR &= ~(1 << 5); // LED1 OFF
    }

}

// PA1 ? LED2
void EXTI1_IRQHandler(void)
{
if (EXTI->PR & (1 << 1))
{
EXTI->PR |= (1 << 1); // Clear pending bit

        if (GPIOA->IDR & (1 << 1))
            GPIOA->ODR |= (1 << 6); // LED2 ON
        else
            GPIOA->ODR &= ~(1 << 6); // LED2 OFF
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
        // Wait for EXTI interrupts
    }

}
