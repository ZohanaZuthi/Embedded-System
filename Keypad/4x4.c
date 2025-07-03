#include "stm32f10x.h"

char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

int row, col;

// ------------ Enable GPIOA Clock ------------
void enable_clock()
{
    RCC->APB2ENR |= (1 << 2); // Enable GPIOA
}

// ------------ Setup Rows (PA0–PA3) as Output, Columns (PA4–PA7) as Input ------------
void keypad_init()
{
    // Rows: PA0–PA3 as output push-pull, 50 MHz
    GPIOA->CRL &= ~(0xFFFF << 0); // Clear bits for PA0–PA3
    GPIOA->CRL |= (0x3333 << 0);  // MODE = 11 (50 MHz), CNF = 00

    // Columns: PA4–PA7 as input floating
    GPIOA->CRL &= ~(0xFFFF << 16); // Clear bits for PA4–PA7
    GPIOA->CRL |= (0x4444 << 16);  // MODE = 00, CNF = 01 (input floating)
}

// ------------ Setup PA8–PA10 as Output (RGB LED) ------------
void rgb_init()
{
    GPIOA->CRH &= ~(0xFFF << 0); // Clear PA8–PA10
    GPIOA->CRH |= (0x333 << 0);  // Output push-pull, 50 MHz
}

// ------------ Simple Delay Function ------------
void delay(volatile unsigned int t)
{
    while (t--)
    {
        for (volatile int i = 0; i < 1000; i++)
            __NOP();
    }
}

// ------------ Scan Keypad ------------
char get_key()
{
    for (row = 0; row < 4; row++)
    {
        // Set current row LOW, others HIGH
        GPIOA->ODR |= 0x0F;        // Set PA0–PA3 HIGH
        GPIOA->ODR &= ~(1 << row); // Set current row LOW

        delay(1); // Short delay for line to settle

        for (col = 0; col < 4; col++)
        {
            if (!(GPIOA->IDR & (1 << (col + 4))))
            { // Check if column is LOW
                while (!(GPIOA->IDR & (1 << (col + 4))))
                    ;      // Wait for release
                delay(10); // Debounce
                return key_map[row][col];
            }
        }
    }
    return 0;
}

// ------------ Set RGB Color ------------
void set_rgb(char key)
{
    // Turn all colors OFF
    GPIOA->ODR &= ~((1 << 8) | (1 << 9) | (1 << 10));

    switch (key)
    {
    case '1':
        GPIOA->ODR |= (1 << 8);
        break; // Red
    case '2':
        GPIOA->ODR |= (1 << 9);
        break; // Green
    case '3':
        GPIOA->ODR |= (1 << 10);
        break; // Blue
    case '4':
        GPIOA->ODR |= (1 << 8) | (1 << 9);
        break; // Yellow
    case '5':
        GPIOA->ODR |= (1 << 8) | (1 << 10);
        break; // Magenta
    case '6':
        GPIOA->ODR |= (1 << 9) | (1 << 10);
        break; // Cyan
    case '7':
        GPIOA->ODR |= (1 << 8) | (1 << 9) | (1 << 10);
        break; // White
    default:
        break; // Others: OFF
    }
}

// ------------ Main Function ------------
int main(void)
{
    enable_clock();
    keypad_init();
    rgb_init();

    while (1)
    {
        char key = get_key();
        if (key)
        {
            set_rgb(key);
        }
    }
}
