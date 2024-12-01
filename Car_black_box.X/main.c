/*
 * name: Mayur Anil Avhad
 * date:11/09/2024
 * Description :
 * Car Black Box Project - Overview
 * The Car Black Box project is inspired by aviation black boxes, designed to record critical data related to a vehicle?s journey. 
 * This system captures key parameters such as speed, gear position, collisions, and timestamps.
 *  In the event of an accident, the data can be reviewed to analyze the driving pattern, helping in investigations and improving road safety.
 * The project includes an intuitive dashboard mode for real-time monitoring and a menu mode for configuration, secured with a password. 
 * The black box ensures reliability by storing essential data even during unexpected events, providing a valuable tool for both drivers and investigators.
 */
#include <xc.h>
#include "main.h"
#include <string.h>
#pragma config WDTE = OFF

unsigned char time[9];
unsigned char clock_reg[3], speed[3];
unsigned int i = 0;
unsigned int block = 0, clear_flag = 0;
unsigned char dash_mode = 1;
unsigned char *event[] = {"ON", "GN", "G1", "G2", "G3", "G4", "GR", "C "};
unsigned char prekey;
char pass[5];
unsigned int log_counter = 0;

unsigned char testing_flag = 0;

void init_config()
{
    init_clcd();
    init_adc();
    init_i2c(100000);
    init_ds1307();
    init_digital_keypad();
    init_timer0();
    GIE = 1;
    PEIE = 1;
    //if password is showing incorrect uncomment this part (once)
//        write_ext_eeprom(0x7D, '0'); // 125 (decimal) address, '1' (0x31)
//        write_ext_eeprom(0x7E, '0'); // 126 (decimal) address, '0' (0x30)
//        write_ext_eeprom(0x7F, '0'); // 127 (decimal) address, '1' (0x31)
//        write_ext_eeprom(0x80, '0'); // 128 (decimal) address, '0' (0x30)

    // for downloading the logs in pc
    init_uart(9600);

    puts("UART Test Code\n\r");
}

void handle_lockout()
{
    unsigned char hundreds, tens, ones;

    TMR0IE = 1;  // Enable Timer Interrupt for updating block
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    while (block < 120)
    {
        clcd_print("Wait For", LINE1(4));  // Display a message on the first line
        
        // Convert block to hundreds, tens, and ones
        hundreds = block / 100;
        tens = (block / 10) % 10;
        ones = block % 10;

        // Display the block value manually on the second line
        clcd_putch(hundreds + '0', LINE2(4)); 
        clcd_putch(tens + '0', LINE2(5)); 
        clcd_putch(ones + '0', LINE2(6));
        clcd_print("sec...", LINE2(8));
    }
    // Reset block counter and disable the timer interrupt
    block = 0;
    TMR0IE = 0;  // Disable Timer Interrupt
}

void main(void)
{
    init_config();

    unsigned int attempt = 3;
    unsigned char down_press = 0;
    unsigned int long_press_counter = 0;
    for (int i = 0; i < 4; i++)
    {                                       // reading pass from the eeprom
        pass[i] = read_ext_eeprom(125 + i); // Start at address 125
    }
    pass[4] = '\0'; // Null-terminate the string

    while (1)
    {
        if (dash_mode)
        {
            update_dashboard_display();

            unsigned char key = detect_single_press();

            if (key == SW1)
            {
                i = 7;
                store_eeprom();
            }
            else if (key == SW2)
            {
                if (i != 6)
                {
                    i++;
                    store_eeprom();
                }
            }
            else if (key == SW3)
            {
                if (i != 1)
                {
                    i--;
                    store_eeprom();
                }
            }
            else if (key == SW4)
            {
                dash_mode = 0;
            }
        }
        else
        {
            while (attempt > 0)
            {
                if (enter_password(pass, &attempt))
                {
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    __delay_us(500);

                    // Initial menu display
                    clcd_print("View Logs    ", LINE1(3));
                    clcd_print("Clear Logs   ", LINE2(3));

                    while (1)
                    {
                        handle_menu_navigation(&down_press, &long_press_counter);
                        if (dash_mode)
                        {
                            down_press = 0; // make menu starts from view again
                            break;
                        }
                    }
                }
                if (dash_mode)
                {
                    break;
                }

                // Lock the system after all attempts are used
                if (attempt == 0)
                {
                    handle_lockout();
                    attempt = 3;   // Reset attempt counter
                    dash_mode = 1; // Return to dashboard mode
                }
            }
        }
    }
}
