#include "stm32f10x.h"

void gpio_init(void)
{
    // Enable clock for GPIOC and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN;

    // PC13 as output push-pull (LED)
    GPIOC->CRH &= ~(0xF << 20); // Clear PC13 config ?
    GPIOC->CRH |= (0x1 << 20);  // Output, 10 MHz, push-pull?

    // PA0 as analog input (potentiometer)
    GPIOA->CRL &= ~(0xF << 0); // Clear PA0 config?
    // Analog input mode (MODE = 00, CNF = 00) ? already done by clearing
}

void adc_init(void)
{
    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Enable ADC1
    ADC1->CR2 |= ADC_CR2_ADON;

    // Small delay after ADC power-up
    for (volatile int i = 0; i < 1000; i++)
        ;

    // Set sample time for channel 0 (PA0) to max (239.5 cycles)
    ADC1->SMPR2 |= (0x7 << 0);
    //?
}

uint16_t adc_read(void)
{
    ADC1->SQR1 = (1 - 1) << 20; // 1 conversion
    ADC1->SQR3 = 0 << 0;        // Channel 0?

    ADC1->CR2 |= ADC_CR2_ADON; // Start conversion
    while (!(ADC1->SR & ADC_SR_EOC))
        ; // Wait for conversion complete

    return ADC1->DR; // Return ADC result
}

void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 800; i++)
    {
        __NOP();
    }
}

int main(void)
{
    gpio_init();
    adc_init();

    while (1)
    {
        uint16_t adc_val = adc_read();                      // 0–4095
        uint32_t full_delay = 100 + (adc_val * 900) / 4095; // 100–1000 ms
        uint32_t half_delay = full_delay / 2;

        GPIOC->ODR |= (1 << 13); // LED ON?
        delay_ms(half_delay);

        GPIOC->ODR &= ~(1 << 13); // LED OFF?
        delay_ms(half_delay);
    }
}
