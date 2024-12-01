/* 
 * File:   external_eeprom.h
 * Author: Acer
 *
 * Created on 8 August, 2024, 8:42 PM
 */

#ifndef EXTERNAL_EEPROM_H
#define	EXTERNAL_EEPROM_H

#define E_WRITE             0b10100000 // 0xA0
#define E_READ              0b10100001 // 0xA1




//void init_ds1307(void);
unsigned char read_ext_eeprom(unsigned char addr);
void write_ext_eeprom(unsigned char addr, unsigned char data);

 void store_eeprom(void);

#endif	/* EXTERNAL_EEPROM_H */

