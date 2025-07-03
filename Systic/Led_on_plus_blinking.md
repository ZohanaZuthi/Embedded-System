#include "stm32f10x.h"
// === SysTick Initialization for 1ms delay ===
void systick_init(void) {
SysTick->LOAD = 72000 - 1; // Assuming 72MHz, this gives 1ms delay
SysTick->VAL = 0;
SysTick->CTRL = 5; // Enable SysTick, use processor clock
}
// 1ms delay
void delay_ms(void) {
while (!(SysTick->CTRL & (1 << 16))); // Wait until COUNTFLAG is set
}
// Delay t milliseconds
void delay_t(unsigned long t) {
while (t--) delay_ms();
}
// === GPIO Configuration ===
void GPIO_config(void) {
RCC->APB2ENR |= (1 << 4); // Enable GPIOC clock
RCC->APB2ENR |= (1 << 3); // Enable GPIOB clock
// PC14 as input pull-down
GPIOC->CRH &= ~(0xF << 24); // Clear bits 27:24
GPIOC->CRH |= (0x8 << 24); // CNF = 10 (input pull-up/down), MODE = 00
// PB13 as output push-pull
GPIOB->CRH &= ~(0xF << 20); // Clear bits 23:20
GPIOB->CRH |= (0x2 << 20); // MODE = 10 (output 2MHz), CNF = 00 (push-pull)
GPIOB->ODR &= ~(1 << 13); // LED OFF initially
}
// === Button Reading Function ===
// Returns 1 if pressed, 0 otherwise (assuming active-high)
uint8_t is_button_pressed(void) {
return (GPIOC->IDR & (1 << 14)) ? 1 : 0;
}
int main(void) {
systick_init();
GPIO_config();
while (1) {
if (is_button_pressed()) {
// Measure how long it's held
unsigned int press_time = 0;
while (is_button_pressed()) {
delay_t(1); // Delay 1ms
press_time++;
if (press_time > 2000) break; // If held more than 2s, stop measuring
}
if (press_time > 2000) {
// Long press: blink
while (is_button_pressed()) {
GPIOB->ODR ^= (1 << 13); // Toggle LED
delay_t(500);
}
GPIOB->ODR &= ~(1 << 13); // Turn off after release
} else {
// Short press: LED ON
GPIOB->ODR |= (1 << 13);
delay_t(1000); // Keep on for a while
GPIOB->ODR &= ~(1 << 13);
}
}
}
}
