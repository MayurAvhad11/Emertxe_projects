#include <xc.h>


extern unsigned int block;
__interrupt() isr() {
    static unsigned int count = 0;
    if (TMR0IF) // if overflow occured 
    {
        TMR0 = TMR0 + 8;
        if (++count == 20000) 
        {
            count = 0;
            block++;
        }
        TMR0IF = 0;
    }

}
