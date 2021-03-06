/**
 * Driver library for HD44780-compatible LCD displays.
 * https://github.com/armandas/Yet-Another-LCD-Library
 *
 * Author: Armandas Jarusauskas (http://projects.armandas.lt)
 *
 * Contributors:
 *    Sylvain Prat (https://github.com/sprat)
 *
 * This work is licensed under a
 * Creative Commons Attribution 3.0 Unported License.
 * http://creativecommons.org/licenses/by/3.0/
 *
 */
/**
        Mod. By: Omar Gurrola
        www.proprojects.wordpress.com
        2/26/13
        - Added #if DATA_SHIFT > 0 to shift (To remove warning)
        - Added LCD_RW_MODE if you use RW pin otherwise connecto to 0 (GND)
        - Added StartUpCode in lcd_init()
        - Added LCD_START_DELAY() to stablize when power on.
        - Changed algorith for lcd_gotoxy(), because don't work right for 20x4 LCD
        03/04/13
        - Adjusted to be used with 74HC595 IC (STP)
        08/04/2014
        - Updated to XC8
        - Changed to be use with MCP23017 I/O Expander on IOA instead of STP
 **/

/******************************* CONFIG ***************************************/
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

// LCD CONFIGURATION ------------------------------------------------------------
// number of lines on the LCD
#define LCD_LINES           2
#define LCD_CHARACTERS      16

// data mask
// this must match the data pins on the port
// this program uses 4 lower bits of PORT
#define DATA_MASK 0b00001111
// if you don't use the lower four pins
// for your data port, your data will have
// to be shifted.
// number of left shifts to do:
#define DATA_SHIFT 0

// LCD pins on IOA (IOA0-IOA7)
#define LCD_RS		A4
#define LCD_EN		A5
#define LCD_BL		A6

// timings: depend on the instruction clock speed
#define LCD_ADDRESS_SETUP_DELAY()   NOP()           	// > 40 ns
#define LCD_EXECUTION_DELAY()       __delay_us(50)      // > 50 us
#define LCD_ENABLE_PULSE_DELAY()    __delay_us(250)	// > 250 ns
#define LCD_HOME_CLEAR_DELAY()      __delay_ms(2)	// > 1.6 ms
#define LCD_START_DELAY()           __delay_ms(10)      // ~ 10 ms

// Lines directions for gotoxy function
#define LCD_LINEDEF_DIR 0x00	// If other than 1-4 is selected
#define LCD_LINE1_DIR 	0x00
#define LCD_LINE2_DIR 	0x40
#define LCD_LINE3_DIR 	0x14
#define LCD_LINE4_DIR 	0x54
/******************************* END OF CONFIG ********************************/

// commands
#define CLEAR_DISPLAY           0x01
#define RETURN_HOME             0x02
#define ENTRY_MODE              0x04
#define DISPLAY_CONTROL         0x08
#define CURSOR_DISPLAY_SHIFT    0x10
#define FUNCTION_SET            0x20
#define SET_CGRAM_ADDR          0x40
#define SET_DDRAM_ADDR          0x80

// ENTRY_MODE flags
#define CURSOR_INCREMENT        0x02
#define ENABLE_SHIFTING         0x01

// DISPLAY_CONTROL flags
#define DISPLAY_ON              0x04
#define CURSOR_ON               0x02
#define BLINKING_ON             0x01

// CURSOR_DISPLAY_SHIFT flags
#define DISPLAY_SHIFT           0x08
#define SHIFT_RIGHT             0x04

// FUNCTION_SET flags
#define DATA_LENGTH             0x10
#define DISPLAY_LINES           0x08
#define CHAR_FONT               0x04

// function prototypes
void lcd_flags_set(unsigned char, unsigned char, unsigned char);
void lcd_initialize(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_write(char *);
void lcd_goto(unsigned char, unsigned char);
void lcd_add_character(unsigned char, unsigned char *);
void lcd_backlight(bool);

// inline functions
#define lcd_clear()         lcd_command(CLEAR_DISPLAY); LCD_HOME_CLEAR_DELAY()
#define lcd_return_home()   lcd_command(RETURN_HOME); LCD_HOME_CLEAR_DELAY()

#define lcd_display_on()    lcd_flags_set(DISPLAY_CONTROL, DISPLAY_ON, 1)
#define lcd_display_off()   lcd_flags_set(DISPLAY_CONTROL, DISPLAY_ON, 0)
#define lcd_cursor_on()     lcd_flags_set(DISPLAY_CONTROL, CURSOR_ON, 1)
#define lcd_cursor_off()    lcd_flags_set(DISPLAY_CONTROL, CURSOR_ON, 0)
#define lcd_blinking_on()   lcd_flags_set(DISPLAY_CONTROL, BLINKING_ON, 1)
#define lcd_blinking_off()  lcd_flags_set(DISPLAY_CONTROL, BLINKING_ON, 0)

#define lcd_backlight_on()  lcd_backlight(1)
#define lcd_backlight_off() lcd_backlight(0)
