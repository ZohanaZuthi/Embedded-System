#include "stm32f10x.h"

// === SysTick for 1ms delay ===
void SysTick_Init(void) {
SysTick->LOAD = 72000 - 1; // 1ms delay at 72MHz
SysTick->VAL = 0;
SysTick->CTRL = 5; // Enable SysTick, processor clock
}

void delay_ms(uint32_t ms) {
while (ms--) {
while (!(SysTick->CTRL & (1 << 16)));
}
}

// === GPIO Configuration ===
void GPIO_Config(void) {
RCC->APB2ENR |= (1UL << 4) | (1UL << 3) | (1UL << 0); // Enable GPIOC, GPIOB, AFIO

    // --- PC15 as input with pull-down (button) ---
    GPIOC->CRH &= ~(15UL << 28);  // Clear PC15 config (bits 31:28)
    GPIOC->CRH |= (8UL << 28);    // CNF=10 (input pull-up/pull-down), MODE=00 (input)
    GPIOC->ODR &= ~(1UL << 15);   // Select pull-down (clear ODR bit for pull-down)

    // --- PB13 as output push-pull (LED) ---
    GPIOB->CRH &= ~(15UL << 20);  // Clear PB13 config
    GPIOB->CRH |= (2UL << 20);    // MODE=10 (2MHz), CNF=00 (push-pull output)
    GPIOB->ODR &= ~(1UL << 13);   // LED OFF initially

}

// === EXTI15 Configuration ===
void EXTI15_Config(void) {
// Map EXTI15 to PC15
AFIO->EXTICR[3] &= ~(15UL << 12); // Clear EXTI15 bits
AFIO->EXTICR[3] |= (2UL << 12); // Set EXTI15 to Port C

    EXTI->IMR |= (1UL << 15);          // Unmask EXTI15 interrupt

    // Choose trigger edge (for example, falling edge for button press)
    EXTI->RTSR &= ~(1UL << 15);        // Disable rising edge
    EXTI->FTSR |= (1UL << 15);         // Enable falling edge trigger

}

// === NVIC Configuration ===
void NVIC_Config(void) {
NVIC_EnableIRQ(EXTI15_10_IRQn); // Enable EXTI15_10 interrupt
}

// === Interrupt Handler ===
void EXTI15_10_IRQHandler(void) {
if (EXTI->PR & (1UL << 15)) {
EXTI->PR |= (1UL << 15); // Clear pending bit

        // Toggle LED on interrupt
        GPIOB->ODR ^= (1UL << 13);
    }

}

int main(void) {
SysTick_Init();
GPIO_Config();
EXTI15_Config();
NVIC_Config();

    while (1) {
        // You can do other stuff here
        // No busy wait on button press, handled by interrupt
    }

}
