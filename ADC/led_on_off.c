// Control LED (PC15) ON/OFF based on potentiometer level on PA0 (ADC channel 0)

#include "stm32f10x.h"

// === GPIO Init ===
void gpio_init(void)
{
    RCC->APB2ENR |= (1 << 4) | (1 << 2); // Enable GPIOC and GPIOA

    // --- PC15 as output push-pull (LED) ---
    GPIOC->CRH &= ~(0xF << 28); // Clear PC15 config
    GPIOC->CRH |= (0x1 << 28);  // Output 10 MHz, push-pull

    // --- PA0 as analog input (potentiometer) ---
    GPIOA->CRL &= ~(0xF << 0); // Clear PA0 config
    GPIOA->CRL |= (0x0 << 0);  // Analog input mode
}

// === ADC Init ===
void adc_init(void)
{
    RCC->APB2ENR |= (1 << 9); // Enable ADC1 clock
    ADC1->CR2 |= (1 << 0);    // Turn on ADC1
    for (volatile int i = 0; i < 1000; i++)
        ;                      // Short delay for stability
    ADC1->SMPR2 |= (0x7 << 0); // Sampling time for channel 0
}

// === Read ADC from PA0 ===
uint16_t adc_read(void)
{
    ADC1->SQR1 = 0;        // 1 conversion
    ADC1->SQR3 = 0;        // Channel 0 (PA0)
    ADC1->CR2 |= (1 << 0); // Start conversion
    while (!(ADC1->SR & (1 << 1)))
        ;            // Wait for EOC
    return ADC1->DR; // Return result
}

// === Simple delay ===
void delay_ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms * 800; i++)
    {
        __NOP();
    }
}

// === Main Function ===
int main(void)
{
    gpio_init();
    adc_init();

    while (1)
    {
        uint16_t adc_value = adc_read(); // 0 to 4095

        // Threshold logic: if ADC value > 90% of max, turn ON LED
        if (adc_value > 3800)
        {
            GPIOC->ODR |= (1 << 15); // LED ON
        }
        else
        {
            GPIOC->ODR &= ~(1 << 15); // LED OFF
        }

        delay_ms(100); // Slow polling
    }
}