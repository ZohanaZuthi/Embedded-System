#include "stm32f10x.h" // Adjust this include to your MCU header

// ------------ Initialize SysTick for 1 ms delay ------------
void systick_init(void) {
SysTick->LOAD = 72000 - 1; // 72 MHz / 1000 = 1 ms
SysTick->VAL = 0;
SysTick->CTRL = 5; // Enable SysTick with processor clock
}

// ------------ Delay for 1 ms ------------
void delay_ms(void) {
while (!(SysTick->CTRL & (1 << 16))); // Wait for COUNTFLAG
}

// ------------ Delay for t ms ------------
void delay_t(uint32_t t) {
while (t--) {
delay_ms();
}
}

// ------------ Configure GPIOs ------------
void GPIO_config(void) {
RCC->APB2ENR |= (1 << 2); // Enable GPIOA clock

    // PA9 as output push-pull (LED)
    GPIOA->CRH &= ~(0xF << 4); // Clear PA9
    GPIOA->CRH |= (0x2 << 4);  // MODE = 10 (2 MHz), CNF = 00 (Push-Pull)

    // PA0 as input with pull-up (Button)
    GPIOA->CRL &= ~(0xF << 0); // Clear PA0
    GPIOA->CRL |= (0x8 << 0);  // MODE = 00, CNF = 10 (Input pull-up)
    GPIOA->ODR |= (1 << 0);    // Enable pull-up resistor on PA0

}

// ------------ Main Function ------------
int main(void) {
systick_init(); // Initialize SysTick
GPIO_config(); // Configure LED and button

    uint8_t led_state = 0;          // LED off initially
    uint8_t last_button_state = 1;  // Button not pressed (pull-up = 1)

    while (1) {
        uint8_t current_button_state = (GPIOA->IDR & (1 << 0)) ? 1 : 0;

        // Detect button press (falling edge)
        if (last_button_state == 1 && current_button_state == 0) {
            led_state = !led_state;  // Toggle LED state

            if (led_state) {
                GPIOA->ODR |= (1 << 9);   // Turn ON LED
            } else {
                GPIOA->ODR &= ~(1 << 9);  // Turn OFF LED
            }
        }

        last_button_state = current_button_state;

        delay_t(20);  // Simple debounce delay
    }

}
