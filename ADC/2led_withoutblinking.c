// Blinking rate control using pot
#include "stm32f10x.h"
void gpio_init(void)
{
    RCC->APB2ENR |= (1 << 4) | (1 << 2);

    GPIOC->CRH &= ~(15UL << 24); // Clear PC14 config
    GPIOC->CRH |= (1 << 24);     // PC14 output push-pull 10MHz
    GPIOC->CRH &= ~(15UL << 28); // Clear PC15 config
    GPIOC->CRH |= (1 << 28);     // PC15 output push-pull 10MHz

    GPIOA->CRL &= ~(15UL << 0); // Clear PA0 config
    GPIOA->CRL |= (0 << 0);     // PA0 analog input
}
void adc_init(void)
{
    RCC->APB2ENR |= (1 << 9);
    ADC1->CR2 |= (1 << 0);
    for (volatile int i = 0; i < 1000; i++)
        ;
    ADC1->SMPR2 |= (0x7 << 0); // ?//////////////////////////////////////CHANGE WILL BE HERE for change input
}
uint16_t adc_read(void)
{
    ADC1->SQR1 = (1 - 1) << 20;
    ADC1->SQR3 = (0 << 0); // //////////////////////////////////////////CHANGE WILL BE HERE for change input
    ADC1->CR2 |= (1 << 0);
    while (!(ADC1->SR & (1 << 1)))
        ;
    return ADC1->DR; // Read result
}
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 800; i++)
    {
    }
}
int main(void)
{
    gpio_init();
    adc_init();
    while (1)
    {
        // Read ADC value from channel 0 (range: 0 to 4095)
        uint16_t adc_value = adc_read();
        // uint32_t delay = 100 + (adc_value * 900) / 4095; // 100ms–1000ms
        if (adc_value <= 2047)
        {
            GPIOC->ODR ^= (1 << 14);
            GPIOC->ODR &= ~(1 << 15);
            // delay_ms(delay);
        }
        else
        {
            GPIOC->ODR ^= (1 << 15);
            GPIOC->ODR &= ~(1 << 14);
            // delay_ms(delay);
        }
    }
}