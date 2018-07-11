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
        - Added #if DATA_SHIFT > 0 to shift
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

#include "HD44780-IOE.h"
#include "mcp23017.h"

// private function prototypes
static void _send_nibble(unsigned char);
static void _send_byte(unsigned char);
static void _set_4bit_interface();

// global variables
unsigned char display_config[6];

// Declaration of STPByte (will be the LCD Register)

union IOAByte {
    uint8_t LCD_DATA;

    struct {
        unsigned A0 : 1; //A0-LSB
        unsigned A1 : 1; //A1
        unsigned A2 : 1; //A2
        unsigned A3 : 1; //A3
        unsigned A4 : 1; //A4
        unsigned A5 : 1; //A5
        unsigned A6 : 1; //A6
        unsigned A7 : 1; //A7-MSB
    };
} LCDPort;

static void mcp23017_ioa_send_byte(void) {
    mcp23017_write_reg(MCP23017_REG_GPIOA, LCDPort.LCD_DATA);
}

// private utility functions

static void _send_nibble(unsigned char data) {
#if DATA_SHIFT > 0
    data <<= DATA_SHIFT; // shift the data as required
#endif
    LCDPort.LCD_DATA &= ~DATA_MASK; // clear old data bits
    LCDPort.LCD_DATA |= DATA_MASK & data; // put in new data bits
    mcp23017_ioa_send_byte();

    LCDPort.LCD_EN = 1;
    mcp23017_ioa_send_byte();
    LCD_ENABLE_PULSE_DELAY();
    LCDPort.LCD_EN = 0;
    mcp23017_ioa_send_byte();
    LCD_ENABLE_PULSE_DELAY();
}

static void _send_byte(unsigned char data) {
    _send_nibble(data >> 4);
    _send_nibble(data & 0x0F);
}

static void _set_4bit_interface() {
    LCDPort.LCD_RS = 0;
    mcp23017_ioa_send_byte();
    LCD_ADDRESS_SETUP_DELAY();
    _send_nibble(0b0010);
    LCD_EXECUTION_DELAY();
}

/**
 * Display string stored in RAM
 *
 * Usage:
 *     char message[10] = "Hello";
 *     lcd_write(message);
 */
void lcd_write(char *str) {
    unsigned char i = 0;

    while (str[i] != '\0')
        lcd_data(str[i++]);
}

/**
 * Move cursor to a given location
 */
void lcd_goto(unsigned char row, unsigned char col) {
    unsigned char addr;

    switch (row) {
        case 1: addr = LCD_LINE1_DIR;
            break;
        case 2: addr = LCD_LINE2_DIR;
            break;
        case 3: addr = LCD_LINE3_DIR;
            break;
        case 4: addr = LCD_LINE4_DIR;
            break;
        default:addr = LCD_LINEDEF_DIR;
            break;
    }
    addr += col - 1;
    lcd_command(SET_DDRAM_ADDR | addr);
}

/**
 * Add a custom character
 */
void lcd_add_character(unsigned char addr, unsigned char * pattern) {
    unsigned char i;

    lcd_command(SET_CGRAM_ADDR | addr << 3);
    for (i = 0; i < 8; i++)
        lcd_data(pattern[i]);
}

/**
 * Lousy function for automatic LCD initialization
 */
void lcd_initialize(void) {
    unsigned char i;

    // Wait some time
    LCD_START_DELAY();

    // Initialize MCP23017
    mcp23017_write_reg(MCP23017_REG_IODIRA, 0x00); // IOA as output
    mcp23017_write_reg(MCP23017_REG_GPIOA, 0x00); // Clear

    // initialize the display_config
    for (i = 0; i < 6; i++) {
        display_config[i] = 0x00;
    }

    _set_4bit_interface();

    // function set
    lcd_flags_set(FUNCTION_SET, DATA_LENGTH | CHAR_FONT, 0);
    lcd_flags_set(FUNCTION_SET, DISPLAY_LINES, 1);

    lcd_backlight_on();
    lcd_display_on();
    lcd_cursor_off();
    lcd_blinking_off();

    lcd_flags_set(ENTRY_MODE, CURSOR_INCREMENT, 1);
    lcd_flags_set(ENTRY_MODE, ENABLE_SHIFTING, 0);

    lcd_clear();
    lcd_return_home();
}

void lcd_command(unsigned char command) {
    LCDPort.LCD_RS = 0;
    mcp23017_ioa_send_byte();
    LCD_ADDRESS_SETUP_DELAY();
    _send_byte(command);
    LCD_EXECUTION_DELAY();
}

void lcd_data(unsigned char data) {
    LCDPort.LCD_RS = 1;
    mcp23017_ioa_send_byte();
    LCD_ADDRESS_SETUP_DELAY();
    _send_byte(data);
    LCD_EXECUTION_DELAY();
}

void lcd_flags_set(unsigned char instruction,
        unsigned char flags,
        unsigned char value) {
    unsigned char index;

    switch (instruction) {
        case ENTRY_MODE:
            index = 0;
            break;
        case DISPLAY_CONTROL:
            index = 1;
            break;
        case CURSOR_DISPLAY_SHIFT:
            index = 2;
            break;
        case FUNCTION_SET:
            index = 3;
            break;
        case SET_CGRAM_ADDR:
            index = 4;
            break;
        case SET_DDRAM_ADDR:
            index = 5;
            break;
    }

    if (value == 0)
        display_config[index] &= ~flags; // reset flags
    else
        display_config[index] |= flags; // set flags

    lcd_command(instruction | display_config[index]);
}

void lcd_backlight(bool state) {
    LCDPort.LCD_BL = state;
    mcp23017_ioa_send_byte();
}