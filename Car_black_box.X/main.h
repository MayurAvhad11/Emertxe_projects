/* 
 * File:   main.h
 * Author: Acer
 *
 * Created on 7 August, 2024, 6:11 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include "clcd.h"
#include "i2c.h"
#include "external_eeprom.h"
#include  "ds1307.h"
#include "adc.h"
#include "digital_keypad.h"
#include "uart.h"


void init_timer0(void);
void display_logs();
void handle_lockout();
void handle_menu_navigation(unsigned char *down_press, unsigned int *long_press_counter);
void update_dashboard_display();
unsigned char detect_single_press();
void enter_menu_option(unsigned char down_press);
int enter_password(char *pass, unsigned int *attempt);
void change_pass(void);


#endif	/* MAIN_H */

