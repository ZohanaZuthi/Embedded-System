//if one button is pressed start led on, if another
//start blinking
#include "stm32f10x.h"

// === SysTick Initialization ===
void systick_init(void) {
SysTick->LOAD = 72000 - 1; // 1ms delay at 72MHz
SysTick->VAL = 0;
SysTick->CTRL = 5; // Enable SysTick with processor clock
}

void delay_ms(void) {
while (!(SysTick->CTRL & (1 << 16))); // Wait for COUNTFLAG
}

void delay_t(unsigned long t) {
while (t--) delay_ms();
}

// === GPIO Setup ===
void GPIO_config(void) {
RCC->APB2ENR |= (1 << 4); // Enable GPIOC clock
RCC->APB2ENR |= (1 << 3); // Enable GPIOB clock

    // --- PC14 and PC15 as input pull-down ---
    GPIOC->CRH &= ~(0xF << 24); // Clear PC14 (bits 27:24)
    GPIOC->CRH |=  (0x8 << 24); // Input pull-down

    GPIOC->CRH &= ~(0xF << 28); // Clear PC15 (bits 31:28)
    GPIOC->CRH |=  (0x8 << 28); // Input pull-down

    GPIOC->ODR &= ~(1 << 14); // Enable pull-downs

    // --- PB13 as output push-pull ---
    GPIOB->CRH &= ~(0xF << 20); // Clear PB13 (bits 23:20)

GPIOB->CRH |= (0x2 << 20); // MODE=10 (2MHz), CNF=00 (push-pull)
GPIOB->ODR &= ~(1 << 13); // LED OFF initially
}
// === Read buttons ===
uint8_t is_button1_pressed(void) {
return (GPIOC->IDR & (1 << 14)) ? 1 : 0;
}
uint8_t is_button2_pressed(void) {
return (GPIOC->IDR & (1 << 15)) ? 1 : 0;
}
int main(void) {
systick_init();
GPIOC->ODR &= ~(1 << 15); // Pull-down PC15

GPIO_config();
while (1) {
if (is_button2_pressed()) {
// If button 2 (PC15) is pressed ? blink
while(!(is_button1_pressed()))
{ GPIOB->ODR ^= (1 << 13); // Toggle LED
delay_t(500);}
} else if (is_button1_pressed()) {
// If only button 1 (PC14) is pressed ? LED ON
while(!is_button2_pressed()){ GPIOB->ODR |= (1 << 13);}
} else {
// No button pressed ? LED OFF
GPIOB->ODR &= ~(1 << 13);
}
}
}
