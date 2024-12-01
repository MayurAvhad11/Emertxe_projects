#include <xc.h>
#include "main.h"
#include <string.h>

#include "variables.h"

unsigned char detect_single_press() {
    static unsigned char prev_key = ALL_RELEASED;
    __delay_ms(50);
    unsigned char key = read_digital_keypad(STATE);

    if (key != ALL_RELEASED && prev_key == ALL_RELEASED) {
        prev_key = key;
        return key; // Return the detected key
    }
    prev_key = key; // Update previous key state
    return ALL_RELEASED; // No new key press detected
}

void update_dashboard_display() {
   // clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    clcd_print("  TIME    EV  SP", LINE1(0));
    get_time(clock_reg);
    display_time(clock_reg);
    clcd_print(event[i], LINE2(10));
    unsigned short adc_val = read_adc(CHANNEL0) / 10.23;
    // speed[0] = (adc_val / 100) + 48;
    speed[0] = ((adc_val / 10) % 10) + 48;
    speed[1] = (adc_val % 10) + 48;
    speed[2] = '\0';
    clcd_print(speed, LINE2(14));
}

void handle_menu_navigation(unsigned char *down_press, unsigned int *long_press_counter) {
    unsigned char key = detect_single_press();
    static unsigned char sw5_pressed = 0;
    static unsigned char sw6_pressed = 0;
    static unsigned int sw5_timer = 0;
    static unsigned int sw6_timer = 0;

    unsigned int long_press_threshold = 20;  // Threshold for long press

    // Detect SW5 press and handle it
    if (read_digital_keypad(LEVEL) == SW5) {
        sw5_pressed = 1;
        sw5_timer++;  // Increment timer to track how long SW5 is pressed
    } else {
        if (sw5_pressed) { // SW5 was pressed, now it's released
            if (sw5_timer >= long_press_threshold) {
                // Long press detected, enter the selected menu option
                enter_menu_option(*down_press);
            } else {
                // Short press detected, scroll up
                if (*down_press > 0) {
                    (*down_press)--;
                } else {
                    *down_press = 7;  // Wrap to the bottom after the first option
                }
            }
            sw5_pressed = 0;
            sw5_timer = 0;  // Reset the timer
        }
    }

    // Detect SW6 press and handle it
    if (read_digital_keypad(LEVEL) == SW6) {
        sw6_pressed = 1;
        sw6_timer++;  // Increment timer to track how long SW6 is pressed
    } else {
        if (sw6_pressed) { // SW6 was pressed, now it's released
            if (sw6_timer >= long_press_threshold) {
                // Long press detected, return to the dashboard
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE); // Clear screen
                dash_mode = 1; // Return to dashboard mode
            } else {
                // Short press detected, scroll down
                (*down_press)++;
                if (*down_press > 7) {
                    *down_press = 0;  // Wrap to the top after the last option
                }
            }
            sw6_pressed = 0;
            sw6_timer = 0;  // Reset the timer
        }
    }

    // Display the current menu based on down_press value
    if (*down_press == 0) {
        clcd_print("View Logs    ", LINE1(3));
        clcd_print("Clear Logs   ", LINE2(3));
    } else if (*down_press == 2) {
        clcd_print("Clear Logs   ", LINE1(3));
        clcd_print("Download log ", LINE2(3));
    } else if (*down_press == 4) {
        clcd_print("Download log ", LINE1(3));
        clcd_print("Rest Password", LINE2(3));
    } else if (*down_press == 6) {
        clcd_print("Rest Password", LINE1(3));
        clcd_print("Reset Time   ", LINE2(3));
    }

    // Display the star ('*') in the correct position
    if (*down_press % 2 == 0) {
        clcd_print("*", LINE1(1));
        clcd_print(" ", LINE2(1));
    } else {
        clcd_print("*", LINE2(1));
        clcd_print(" ", LINE1(1));
    }
}





void display_logs() {
    unsigned char addr = 0; // Start from the most recent block (index 0)
    unsigned char eeprom_data[12];
    unsigned char key;
    unsigned int index = 0;
    unsigned int long_press_counter = 0;
    unsigned char down_press = 0;

    while (1) {
        if (clear_flag == 0) {
            clcd_print("# TIME     EV SP", LINE1(0));
            clcd_putch(index + '0', LINE2(0));

            for (int j = 0; j < 12; j++) {//fetch the data from eeprom]
                eeprom_data[j] = read_ext_eeprom(addr + j);
            }

            // For printing time 
            for (int j = 0; j < 8; j++) {
                clcd_putch(eeprom_data[j], LINE2(2 + j));
            }
            // For printing events
            for (int j = 0; j < 2; j++) {
                clcd_putch(eeprom_data[8 + j], LINE2(11 + j));
            }
            // For printing speed
            for (int j = 0; j < 2; j++) {
                clcd_putch(eeprom_data[10 + j], LINE2(14 + j));
            }

            key = detect_single_press();
            if (key == SW5) { // Scroll up
                if (index == 0) {
                    index = log_counter; // Wrap around to the last block
                } else {
                    index--;
                }
                addr = index * 12;
            }
            if (key == SW6 && index < log_counter) { // Scroll down 
                index++;
                if (index == log_counter)
                    index = 0;
                addr = index * 12;
            }
            //for live log capture and store
            if (key == SW1) {
                i = 7;
                store_eeprom();

            } else if (key == SW2) {
                if (i != 6) {
                    i++;
                    store_eeprom();

                }
            } else if (key == SW3) {
                if (i != 1) {
                    i--;
                    store_eeprom();

                }
            }

            // Check for long press of SW6 to return to menu mode
            if (read_digital_keypad(LEVEL) == SW6) {
                long_press_counter++;
                if (long_press_counter > 10) { // Adjust the threshold as needed
                    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                    // handle_menu_navigation(&down_press, &long_press_counter);

                    break; // Exit the while loop to return to dashboard
                }
            }// Check for long press of SW5 to return to dashboard

            else {
                long_press_counter = 0; // Reset the counter if SW6 is not pressed
            }
        } else if (clear_flag == 1) {

            clcd_print(" No logs ", LINE1(0));
            clcd_print("are found!",LINE2(4));
            
            __delay_ms(1000);
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            dash_mode = 1;
            if (dash_mode) {
                break;
            }
        }
    }
}

void change_pass(void) {
    char new_pass[5], re_pass[5]; // 4+1 for null 
    unsigned int pos = 0;
    unsigned int i = 5;
    unsigned char key;

    clcd_print(" Enter Password ", LINE1(0));
    clcd_print("                ", LINE2(0));

    // Loop to enter 4-digit password
    while (pos < 4) {
        key = detect_single_press();
        __delay_ms(20);
        if (key == SW5 || key == SW6) {
            new_pass[pos] = (key == SW5) ? '1' : '0';
            clcd_putch('*', LINE2(i)); // Display asterisk at the current position
            i++;
            pos++;
        }
    }
    new_pass[4] = '\0'; //  null-terminate 

    pos = 0;
    i = 5;

    clcd_print("ReEnter Password", LINE1(0));
    clcd_print("                ", LINE2(0));

    // Loop to enter 4-digit password
    while (pos < 4) {
        key = detect_single_press();
        __delay_ms(20);
        if (key == SW5 || key == SW6) {
            re_pass[pos] = (key == SW5) ? '1' : '0';
            clcd_putch('*', LINE2(i)); // Display asterisk at the current position
            i++;
            pos++;
        }
    }
    re_pass[4] = '\0'; //null-terminate 

    // Check the entered password
    if (strcmp(new_pass, re_pass) == 0) {
        strcpy(pass, new_pass);
        for (int addr = 125; addr < 129; addr++) { // to store the new pass to eeprom(to avoid reset pass change)
            write_ext_eeprom(addr, new_pass[addr - 125]);
        }
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        clcd_print("Password is", LINE1(2));
        clcd_print("Changed", LINE2(4));
    } else {
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        clcd_print("Password", LINE1(3));
        clcd_print("Mismatch", LINE2(3));
    }
    __delay_ms(1000);
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    dash_mode = 1;//go back to dashboard
    
}

void download_log() {
    unsigned char eeprom_data[13]; // 12 bytes + 1 for null-termination
    unsigned char addr = 0;
    unsigned int log_index = 0;

    // Print the header with properly aligned columns
    puts("SR.no.  Time      Gear  Speed\n\r");

    // Loop to read logs one by one until all logs are downloaded
    while (log_index < log_counter) {
        // Read one log entry (12 bytes) from EEPROM
        for (int j = 0; j < 12; j++) {
            eeprom_data[j] = read_ext_eeprom(addr + j);
        }

        // Properly null-terminate the string
        eeprom_data[12] = '\0';

       
        // Split it into time, gear, and speed
        char time[9], gear[3], speed[3];
        strncpy(time, eeprom_data, 8);  // First 8 characters are time
        time[8] = '\0'; // Null-terminate time
        strncpy(gear, eeprom_data + 8, 2);  // Next 2 characters are gear
        gear[2] = '\0'; 
        strncpy(speed, eeprom_data + 10, 2); // Last 2 characters are speed
        speed[2] = '\0';

        // Print the formatted log entry with aligned columns
        if (log_index < 10) {  // If the SR.no. is a single digit, add extra space
            putchar(log_index + '0');   // Print SR.no.
            puts(".      ");            // Adjust spacing
        }
        puts(time); // Print the time
        puts("   "); // Add spaces between columns
        puts(gear); // Print the gear
        puts("     ");// Add spaces between columns
        puts(speed);// Print the speed
        putchar('\r');// Return cursor to start of the line (optional)
        putchar('\n');   
        // Move to the next log entry
        addr += 12;  // Increment EEPROM address for the next log
        log_index++; // Keep track of how many logs have been read
    }

    // Indicate the completion of the log download
    puts("Log download complete.\r\n");
}

//for changing time
void change_time(void) {
    unsigned char key;
    unsigned char new_time[3]; // Stores hour, minute, second
    unsigned char blink_pos = 0; // 0: hour, 1: minute, 2: second
    unsigned char blink = 0;
    unsigned int wait = 0;
    unsigned char one_time = 1;
    unsigned char long_press_counter = 0;

    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);

    clcd_print("Time (HH:MM:SS)", LINE1(0));

    while (1) {
        key = detect_single_press();

        if (one_time) {
            one_time = 0;
            get_time(clock_reg); // Fetch current time
            // Convert BCD to decimal
            new_time[0] = ((clock_reg[0] >> 4) & 0x03) * 10 + (clock_reg[0] & 0x0F); // Hours
            new_time[1] = ((clock_reg[1] >> 4) & 0x07) * 10 + (clock_reg[1] & 0x0F); // Minutes
            new_time[2] = ((clock_reg[2] >> 4) & 0x07) * 10 + (clock_reg[2] & 0x0F); // Seconds
        }

        // Handle key presses
        if (key == SW5) { // Increase value
            new_time[blink_pos]++;
            if (blink_pos == 0 && new_time[blink_pos] > 23) // Hours
                new_time[blink_pos] = 0;
            else if ((blink_pos == 1 || blink_pos == 2) && new_time[blink_pos] > 59) // Minutes or Seconds
                new_time[blink_pos] = 0;
        } else if (key == SW6) { // Move to next field
            blink_pos = (blink_pos + 1) % 3;
        }

        // Check for long press of SW6 to save time and exit
        if (read_digital_keypad(LEVEL) == SW6) {
            long_press_counter++;
            __delay_ms(10);
            if (long_press_counter > 100) { // Adjust the threshold as needed
                // Save the new time
                unsigned char dummy;
                // Convert decimal to BCD and write to DS1307
                dummy = ((new_time[0] / 10) << 4) | (new_time[0] % 10);
                write_ds1307(HOUR_ADDR, dummy);

                dummy = ((new_time[1] / 10) << 4) | (new_time[1] % 10);
                write_ds1307(MIN_ADDR, dummy);

                dummy = ((new_time[2] / 10) << 4) | (new_time[2] % 10);
                write_ds1307(SEC_ADDR, dummy);

                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                clcd_print("Time changed", LINE1(2));
                clcd_print("Successfully", LINE2(2));
                __delay_ms(1000);
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
                dash_mode = 1;
                return;
            }
        } else {
            long_press_counter = 0;
        }

        // Blinking logic
        if (wait++ == 15) {//blink current position 
            wait = 0;
            blink = !blink;

            // Display time
            clcd_putch((new_time[0] / 10) + '0', LINE2(0));
            clcd_putch((new_time[0] % 10) + '0', LINE2(1));
            clcd_putch(':', LINE2(2));
            clcd_putch((new_time[1] / 10) + '0', LINE2(3));
            clcd_putch((new_time[1] % 10) + '0', LINE2(4));
            clcd_putch(':', LINE2(5));
            clcd_putch((new_time[2] / 10) + '0', LINE2(6));
            clcd_putch((new_time[2] % 10) + '0', LINE2(7));

            if (blink) {
                switch (blink_pos) {
                    case 0:
                        clcd_print("  ", LINE2(0)); // Blink hours
                        break;
                    case 1:
                        clcd_print("  ", LINE2(3)); // Blink minutes
                        break;
                    case 2:
                        clcd_print("  ", LINE2(6)); // Blink seconds
                        break;
                }
            }
        }
    }
}






//for enter in to menu option
void enter_menu_option(unsigned char down_press) {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);

    if (down_press == 0) {//Enter into view logs
        // Read and display logs

        display_logs();
        // dash_mode = 0; // Stay in menu mode


    } else if (down_press == 1 || down_press == 2) { //Enter into log clear
        // Clear logs functionality
        clear_flag = 1;
        log_counter = 0;
        //clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        clcd_print("logs cleared ", LINE1(2));
        __delay_ms(1000);
        dash_mode = 1;
    }
    else if (down_press == 3 || down_press == 4) {// Enter into download log
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        download_log();
        clcd_print("logs downloaded", LINE1(0));
        clcd_print("successfully", LINE2(0));
        __delay_ms(1000);
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
        dash_mode = 1;

    } else if (down_press == 5 || down_press == 6) {// Enter into change password
        change_pass();
    } else if (down_press == 7) {
         change_time();
}
}

int enter_password(char *pass, unsigned int *attempt) {
    unsigned char usr_pass[5] = {0}; // Buffer for user input
    unsigned int pos = 0;
    unsigned int i = 5;
    unsigned char key;

    clcd_print(" Enter Password ", LINE1(0));
    clcd_print("                ", LINE2(0));

    // Loop to enter 4-digit password
    while (pos < 4) {
        key = detect_single_press();
        __delay_us(50);
        if (key == SW5 || key == SW6) {
            usr_pass[pos] = (key == SW5) ? '1' : '0';
            clcd_putch('*', LINE2(i)); // Display asterisk at the current position
            i++;
            pos++;
        }
    }

    usr_pass[4] = '\0'; // Null-terminate the string

    // Check the entered password
    if (strcmp(usr_pass, pass) == 0) {
        clcd_print("Correct password", LINE1(0));
        return 1; // Correct password
    } else {
        (*attempt)--;
        clcd_print("Wrong password ", LINE1(0));
        __delay_ms(1000); // Delay for feedback
        clcd_print("                ", LINE1(0));
        if (*attempt > 0) {
            clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            //clcd_print(" ", LINE1(0)); // Clear the line
            clcd_putch(*attempt + '0', LINE1(0));
            clcd_print(" attempts left", LINE1(1));
            __delay_ms(1000);
        }
        return 0; // Incorrect password
    }
}


