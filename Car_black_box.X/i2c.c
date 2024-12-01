#include <xc.h>
#include "i2c.h"

void init_i2c(unsigned long baud)
{
    SSPM3 = 1; //set I2C master mode , sspm2,1,0 already = 0
    SSPADD = (unsigned char)(FOSC/ (4 * baud)) - 1;//char => for typecasting
    
    SSPEN = 1;//enable SSP
 }

static void i2c_wait_for_idle(void)
{
    while(R_nW || (SSPCON2 & 0x1F));// check if bus is busy or not , R_nW = 1 => reading by i2c
}
void i2c_start(void)
{
    i2c_wait_for_idle();
    SEN = 1;// INITATE START COND
}
void i2c_rep_start(void)
{
    i2c_stop();
    i2c_start();
}
void i2c_stop(void)
{
    i2c_wait_for_idle();
    PEN = 1;//INITATE STOP COND
}

unsigned char i2c_read(unsigned char ack)
{
    unsigned char data;
    i2c_wait_for_idle();
    RCEN = 1;//receive enabled
    
    i2c_wait_for_idle();
    data = SSPBUF;// read data
    
    if(ack == 1)
    {
        ACKDT = 1;// NOt acknoledged
        
    }
    else {
        ACKDT = 0; // Achnowledged 
    }
    ACKEN = 1; // MASETER ACKNO. enabled(optional)
    return data;
}
int i2c_write(unsigned char data)
{
    i2c_wait_for_idle();
    SSPBUF = data;
    return !ACKSTAT; // !1
}
