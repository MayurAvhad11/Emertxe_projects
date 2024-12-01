#include <xc.h>
#include "external_eeprom.h"
#include "i2c.h"

extern unsigned char time[];
extern unsigned char clock_reg[] , speed[]; 
 extern unsigned int i;
extern unsigned char *event[];
extern unsigned log_counter;
extern unsigned int clear_flag;
unsigned char read_ext_eeprom(unsigned char addr)
{
    unsigned char data;
    
    i2c_start();
    i2c_write(E_WRITE);
    i2c_write(addr);
    i2c_rep_start();
    i2c_write(E_READ);
    data = i2c_read(0);
    i2c_stop();
    
    return data;
}

void write_ext_eeprom(unsigned char addr, unsigned char data) // SEc_ADDR, data
{
    i2c_start();
    i2c_write(E_WRITE);
    i2c_write(addr);
    i2c_write(data);
    i2c_stop();
}

void store_eeprom(void)
{
    unsigned char addr;
    unsigned char eeprom_data[12];

    if(clear_flag)
    {
        clear_flag = 0;
    }
    // Shift old logs to the next index
    for (int index = log_counter; index > 0; index--)
    {
        // Read the existing log from the previous index
        for (int j = 0; j < 12; j++)
        {
            eeprom_data[j] = read_ext_eeprom((index - 1) * 12 + j);
        }
        // Write it to the current index
        for (int j = 0; j < 12; j++)
        {
            write_ext_eeprom(index * 12 + j, eeprom_data[j]);
        }
    }

    // Store the most recent log at index 0
    addr = 0;
    for (int j = 0; j < 8; j++)
    {
        eeprom_data[j] = time[j];
    }
    eeprom_data[8] = event[i][0];
    eeprom_data[9] = event[i][1];
    for (int j = 0; j < 2; j++)
    {
        eeprom_data[10 + j] = speed[j];
    }
    for (int j = 0; j < 12; j++)
    {
        write_ext_eeprom(addr + j, eeprom_data[j]);
    }

    // Update log_counter
    if (log_counter < 10)
    {
        log_counter++;
    }
}

