//if pressed green else red (toggle)
#include "stm32f10x.h"

// === SysTick Initialization for 1ms delay ===
void systick_init(void) {
SysTick->LOAD = 72000 - 1; // Assuming 72MHz clock, gives 1ms
SysTick->VAL = 0; // Clear current value
SysTick->CTRL = 5; // Enable SysTick, use processor clock (no interrupt)
}

// 1ms delay
void delay_ms(void) {
SysTick->VAL = 0; // Reset current value
while (!(SysTick->CTRL & (1 << 16))); // Wait for COUNTFLAG
}

// Delay for t milliseconds
void delay_t(unsigned long t) {
while (t--) delay_ms();
}

// === GPIO Configuration ===
void GPIO_config(void) {
// Enable clocks for GPIOC and GPIOB
RCC->APB2ENR |= (1 << 4); // GPIOC clock
RCC->APB2ENR |= (1 << 3); // GPIOB clock

    // --- Configure PC14 as input pull-down ---
    GPIOC->CRH &= ~(0xF << 24); // Clear bits for PC14
    GPIOC->CRH |=  (0x8 << 24); // CNF = 10 (input with pull-up/pull-down), MODE = 00
    GPIOC->ODR &= ~(1 << 14);   // Enable pull-down by writing 0 to ODR14

    // --- Configure PB13 as output push-pull (LED1) ---
    GPIOB->CRH &= ~(0xF << 20); // Clear bits for PB13
    GPIOB->CRH |=  (0x2 << 20); // MODE = 10 (2MHz output), CNF = 00 (push-pull)

    // --- Configure PB14 as output push-pull (LED2) ---
    GPIOB->CRH &= ~(0xF << 24); // Clear bits for PB14
    GPIOB->CRH |=  (0x2 << 24); // MODE = 10 (2MHz output), CNF = 00 (push-pull)

    // --- Turn off both LEDs initially ---
    GPIOB->ODR &= ~(1 << 13);
    GPIOB->ODR &= ~(1 << 14);

}

// === Button Reading Function ===
// Returns 1 if pressed (active-high), 0 otherwise
uint8_t is_button_pressed(void) {
return (GPIOC->IDR & (1 << 14)) ? 1 : 0;
}

// === Main Program ===
int main(void) {
systick_init();
GPIO_config();

    while (1) {
        if (is_button_pressed()) {
            // Button pressed: LED1 ON, LED2 OFF
            GPIOB->ODR |=  (1 << 13);
            GPIOB->ODR &= ~(1 << 14);
        } else {
            // Button released: LED1 OFF, LED2 ON
            GPIOB->ODR &= ~(1 << 13);
            GPIOB->ODR |=  (1 << 14);
        }

        delay_ms(); // Basic debounce
    }

}
