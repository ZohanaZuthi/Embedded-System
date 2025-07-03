#include "stm32f10x.h"

// ------------ Delay Function ------------
void delay(unsigned int t) {
for (unsigned int i = 0; i < t \* 8000; i++) {
\_\_NOP();
}
}

// ------------ GPIO Setup ------------
void setup(void) {
RCC->APB2ENR |= (1 << 2); // Enable clock for GPIOA

    // Configure PA4, PA5, PA6 (LEDs) as output push-pull, 50 MHz
    GPIOA->CRL &= ~((0xF << 16) | (0xF << 20) | (0xF << 24)); // Clear bits for PA4, PA5, PA6
    GPIOA->CRL |=  ((0x3 << 16) | (0x3 << 20) | (0x3 << 24)); // MODE = 0b11 (50 MHz), CNF = 0b00 (push-pull)

    // Configure PA0, PA1, PA2 (Buttons) as input floating
    GPIOA->CRL &= ~((0xF << 0) | (0xF << 4) | (0xF << 8)); // Clear bits for PA0, PA1, PA2
    GPIOA->CRL |=  ((0x4 << 0) | (0x4 << 4) | (0x4 << 8)); // MODE = 0b00, CNF = 0b01 (input floating)

}

// ------------ Main Function ------------
int main(void) {
setup();

    while (1) {
        // Read button states (active-low logic)
        unsigned char btn0 = (GPIOA->IDR & (1 << 0)); // PA0
        unsigned char btn1 = (GPIOA->IDR & (1 << 1)); // PA1
        unsigned char btn2 = (GPIOA->IDR & (1 << 2)); // PA2

        if (btn0 && !btn1 && !btn2) {
            GPIOA->ODR |= (1 << 4);  // RED ON
            GPIOA->ODR &= ~((1 << 5) | (1 << 6)); // GREEN, BLUE OFF
        } else if (!btn0 && btn1 && !btn2) {
            GPIOA->ODR |= (1 << 5);  // GREEN ON
            GPIOA->ODR &= ~((1 << 4) | (1 << 6)); // RED, BLUE OFF
        } else if (!btn0 && !btn1 && btn2) {
            GPIOA->ODR |= (1 << 6);  // BLUE ON
            GPIOA->ODR &= ~((1 << 4) | (1 << 5)); // RED, GREEN OFF
        } else {
            // No button or multiple buttons pressed ? All OFF
            GPIOA->ODR &= ~((1 << 4) | (1 << 5) | (1 << 6));
        }

        delay(10); // Simple debounce delay
    }

}
