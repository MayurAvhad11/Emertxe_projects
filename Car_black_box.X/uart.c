#include <xc.h>
#include "main.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    SPEN = 1;
    
    /* Continuous Reception Enable Bit */
    CREN = 1;
    
    /* Baud Rate Setting Register */
    SPBRG = (FOSC / (16 * baud)) - 1;
}

unsigned char getchar(void)
{
    /* Wait for the byte to be received */
    while (RCIF != 1)
    {
        continue;
    }
    
    /* Clear the interrupt flag */
    RCIF = 0;
    
    /* Return the data to the caller */
    return RCREG;
}

void putchar(unsigned char data)
{
    while(TXIF == 0)
    {
        continue;
        
    }
    TXREG = data;
    TXIF = 0;
}

void puts(const char *s)
{
    while (*s)
    {
        putchar(*s++);
    }
}
