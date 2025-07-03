//1 pushbutton (PC13)+1 led(PB3):(when first time /press led 1
then press led off)
#include "stm32f10x.h"
// Initialize SysTick for 1 ms delay
void systick_init(void) {
SysTick->LOAD = 72000 - 1; // 72 MHz / 1000 = 1ms
SysTick->VAL = 0;
SysTick->CTRL = 5;  
 // Enable SysTick with processor clock
}
// 1 ms delay
void delay_ms(void) {
while (!(SysTick->CTRL & (1 << 16))); // Wait for COUNTFLAG
}
// t ms delay
void delay_t(unsigned long t) {
while (t--) {
delay_ms();
}
}
// Configure PA9 as output push-pull
void GPIO_config(void) {
RCC->APB2ENR |= (1 << 2); // Enable GPIOA clock
GPIOA->CRH &= ~(0xF << 4); // Clear PA9 (bits 7:4)
GPIOA->CRH |= (0x2 << 4); // MODE9 = 10 (2 MHz output), CNF9 = 00 (push-pull)
}
int main(void) {
systick_init(); // Initialize SysTick
GPIO_config(); // Configure PA9 as output
while (1) {
GPIOA->ODR |= (1 << 9); // PA9 HIGH ? LED ON
delay_t(3000);  
// Delay 500 ms
GPIOA->ODR &= ~(1 << 9); // PA9 LOW ? LED OFF
delay_t(3000);  
// Delay 500 ms
}
}
