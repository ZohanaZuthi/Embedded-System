#include "stm32f10x.h"

// === GPIO Initialization ===
void gpio_init(void)
{
    // Enable clocks for GPIOA and GPIOB
    RCC->APB2ENR |= (1 << 2) | (1 << 3); // IOPAEN | IOPBEN

    // --- PA0 as analog input (potentiometer) ---
    GPIOA->CRL &= ~(0xF << 0); // Clear config for PA0
    GPIOA->CRL |= (0x0 << 0);  // MODE = 00, CNF = 00 (analog input)

    // --- PB0 and PB1 as output push-pull (LEDs) ---
    GPIOB->CRL &= ~((0xF << 0) | (0xF << 4)); // Clear config for PB0 and PB1
    GPIOB->CRL |= ((0x1 << 0) | (0x1 << 4));  // MODE = 01 (10 MHz), CNF = 00 (push-pull)
}

// === ADC Initialization ===
void adc_init(void)
{
    RCC->APB2ENR |= (1 << 9); // Enable ADC1 clock

    ADC1->CR2 |= (1 << 0); // ADON: Power on ADC
    for (volatile int i = 0; i < 1000; i++)
        ; // Short delay for ADC stability

    ADC1->SMPR2 |= (0x7 << 0); // Set maximum sample time for channel 0
}

// === Read ADC value from PA0 ===
uint16_t adc_read(void)
{
    ADC1->SQR1 = 0;        // Regular sequence length = 1
    ADC1->SQR3 = 0;        // Channel 0 (PA0)
    ADC1->CR2 |= (1 << 0); // Start conversion (ADON reused)

    while (!(ADC1->SR & (1 << 1)))
        ;            // Wait for EOC (End of Conversion)
    return ADC1->DR; // Return ADC result
}

// === Simple delay function (approximate ms) ===
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
        uint16_t adc_value = adc_read(); // 12-bit value: 0 to 4095

        if (adc_value <= 2047)
        {
            GPIOB->ODR |= (1 << 0);  // Turn ON LED1 (PB0)
            GPIOB->ODR &= ~(1 << 1); // Turn OFF LED2 (PB1)
        }
        else
        {
            GPIOB->ODR &= ~(1 << 0); // Turn OFF LED1 (PB0)
            GPIOB->ODR |= (1 << 1);  // Turn ON LED2 (PB1)
        }

        delay_ms(100); // Polling delay
    }
}