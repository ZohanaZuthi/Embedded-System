#include "stm32f10x.h"

char key_map[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

int row, col;

void enable_clock()
{
    RCC->APB2ENR |= (1 << 2); // GPIOA
}

void keypad_init()
{
    GPIOA->CRL &= ~(0xFFFF << 0);
    GPIOA->CRL |= (0x3333 << 0); // PA0–PA3 output

    GPIOA->CRL &= ~(0xFFFF << 16);
    GPIOA->CRL |= (0x4444 << 16); // PA4–PA7 input floating
}

void rgb_init()
{
    GPIOA->CRH &= ~(0xFFF << 0);
    GPIOA->CRH |= (0x333 << 0); // PA8–PA10 output
}

void delay(volatile unsigned int t)
{
    while (t--)
    {
        for (volatile int i = 0; i < 1000; i++)
            __NOP();
    }
}

char get_key()
{
    for (row = 0; row < 4; row++)
    {
        GPIOA->ODR |= 0x0F;        // All rows HIGH
        GPIOA->ODR &= ~(1 << row); // One row LOW
        delay(1);

        for (col = 0; col < 4; col++)
        {
            if (!(GPIOA->IDR & (1 << (col + 4))))
            {
                while (!(GPIOA->IDR & (1 << (col + 4))))
                    ;
                delay(10);
                return key_map[row][col];
            }
        }
    }
    return 0;
}

static char last_key = 0;
static uint8_t press_count = 0;

void set_rgb_cycle(char key)
{
    GPIOA->ODR &= ~((1 << 8) | (1 << 9) | (1 << 10)); // All OFF

    if (key == last_key)
    {
        press_count++;
    }
    else
    {
        last_key = key;
        press_count = 1;
    }

    switch (press_count % 3)
    {
    case 1:
        GPIOA->ODR |= (1 << 8); // Red
        break;
    case 2:
        GPIOA->ODR |= (1 << 10); // Blue
        break;
    case 0:
        GPIOA->ODR |= (1 << 9); // Green
        break;
    }
}

// -------------------------
// ? MAIN FUNCTION HERE
// -------------------------
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
            set_rgb_cycle(key);
        }
    }
}
