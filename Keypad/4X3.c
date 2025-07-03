#include "stm32f10x.h"

// Keypad map
char key_map[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

int row, col;

// Enable GPIOA Clock
void enable_clock()
{
    RCC->APB2ENR |= (1 << 2); // GPIOA
}

// Setup PA0–PA3 (rows) as output, PA4–PA6 (columns) as input
void keypad_init()
{
    // Rows: PA0–PA3 as push-pull outputs
    GPIOA->CRL &= ~(0xFFFF << 0); // Clear config
    GPIOA->CRL |= (0x3333 << 0);  // MODE=11, CNF=00

    // Columns: PA4–PA6 as input floating
    GPIOA->CRL &= ~(0xFFF << 16); // Clear PA4–PA6
    GPIOA->CRL |= (0x444 << 16);  // MODE=00, CNF=01
}

// Setup PA8–PA10 as RGB output
void rgb_init()
{
    GPIOA->CRH &= ~(0xFFF << 0); // Clear PA8–PA10
    GPIOA->CRH |= (0x333 << 0);  // MODE=11, CNF=00
}

// Simple delay
void delay(volatile unsigned int t)
{
    while (t--)
    {
        for (volatile int i = 0; i < 1000; i++)
            __NOP();
    }
}

// Scan 4x3 Keypad
char get_key()
{
    for (row = 0; row < 4; row++)
    {
        GPIOA->ODR |= 0x0F;        // Set all rows HIGH
        GPIOA->ODR &= ~(1 << row); // Pull current row LOW
        delay(1);                  // Allow signal to settle

        for (col = 0; col < 3; col++)
        {
            if (!(GPIOA->IDR & (1 << (col + 4))))
            { // Column is LOW?
                while (!(GPIOA->IDR & (1 << (col + 4))))
                    ;      // Wait for release
                delay(10); // Debounce
                return key_map[row][col];
            }
        }
    }
    return 0; // No key
}

// Control RGB LED
void set_rgb(char key)
{
    GPIOA->ODR &= ~((1 << 8) | (1 << 9) | (1 << 10)); // All OFF

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
        break; // All others: LED OFF
    }
}

// Main
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
