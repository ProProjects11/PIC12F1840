/*
 *	Copyright (c) 2011-2014, http://www.proprojects.wordpress.com
 *	All rights reserved.
 *
 * 	Redistribution and use in source and binary forms, with or without modification,
 *	are permitted provided that the following conditions are met:
 *
 *	1.- Redistributions of source code must retain the above copyright notice,
 *          this list of conditions and the following disclaimer.
 *	2.- Redistributions in binary form must reproduce the above copyright notice,
 *          this list of conditions and the following disclaimer in the documentation
 *          and/or other materials provided with the distribution.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 *	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *	IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS
 *	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *	WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*********************************************************************
 * By:              Omar Gurrola
 * Company:         http://proprojects.wordpress.com
 * Processor:       PIC12
 * Compiler:        XC8 v1.32
 * File Name:       main.c
 * Created On:      July 27, 2014, 10:38 AM
 * Description:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/27/14    Initial version
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "pic12f1840_cbits.h"  // Apropiate configuration bits header
#include "main.h"
#include "pic12f1840_i2c.h"
#include "mcp23017.h"
//#include "at24c32.h"
#include "ds1307.h"
#include "HD44780-IOE.h"
#include "keypad.h"
#include <stdio.h>

/** DEFINES ********************************************************/
#define TMR1_PS_1_1  0
#define TMR1_PS_1_2  1
#define TMR1_PS_1_4  2
#define TMR1_PS_1_8  3

/** DECLARATIONS ***************************************************/
typedef enum {
    CLK_POS_HR = 0x01,
    CLK_POS_MIN,
    CLK_POS_SEC,
    CLK_POS_MON,
    CLK_POS_DAY,
    CLK_POS_YR,
    CLK_POS_HR_MOD,
} clk_pos_t;

/** GLOBAL VARIABLES ***********************************************/
static uint16_t g_reload_value;
static uint8_t g_counter1;
static clk_pos_t g_clk_pos;
static bool g_bl;

/** PROTOTYPES *****************************************************/
#define SetClockTo32Mhz()  OSCCONbits.IRCF = 0b1110; OSCCONbits.SPLLEN = 1
void init(uint16_t, uint8_t);
void write_t1(uint16_t);
void update_clock(bool);
uint8_t adjust_val(uint8_t, uint8_t, uint8_t, bool);

/** CODE DECLARATIONS ****************************************/
void main(void) {
    SetClockTo32Mhz();

    i2c_init(I2C_SPEED_STANDARD_100KHZ);
    mcp23017_init(0b000); // Init MCP23017 with the address xxx
    keypad_init();
    //at24c32_init(0b000);
    ds1307_init();
    lcd_initialize();
    init(50000, TMR1_PS_1_8); // 50ms Ticks

    for (;;) {
        ;
    }
}

void init(uint16_t load_value, uint8_t prescaler) {
    // Interrupts Configuration (Only 1 interrupt for this operating system)
    INTCONbits.GIE = 0; // Disable global interrupt
    PIR1bits.TMR1IF = 0; // Clear interrupt flag
    PIE1bits.TMR1IE = 1; // Enable interrupt
    INTCONbits.PEIE = 1; // Timmers are peripherials
    INTCONbits.GIE = 1; // Enable global interrupt

    // Timer1 configuration
    T1CONbits.TMR1ON = 0; // Stop Timmer
    T1CONbits.TMR1CS = 00; // Fosc/4
    T1CONbits.T1CKPS = prescaler; // 1,2,4,8
    g_reload_value = 65536 - load_value;
    write_t1(g_reload_value); // Load value
    T1CONbits.TMR1ON = 1; // Start timer

    g_bl = 1; // Turn off bl
    g_clk_pos = CLK_POS_HR; // Initial position
}

void write_t1(uint16_t value) {
    T1CONbits.TMR1ON = 0; // Stop timer

    // Upload data
    TMR1H = (uint8_t) (value >> 8);
    TMR1L = (uint8_t) value;

    T1CONbits.TMR1ON = 1; // Start timer
}

uint8_t adjust_val(uint8_t value, uint8_t lim_dwn, uint8_t lim_up, bool up_down) {
    if ((value == lim_up) && (up_down)) {
        value = lim_dwn;
    } else if ((value == lim_dwn) && (!up_down)) {
        value = lim_up;
    } else if (up_down) {
        value++;
    } else {
        value--;
    }
    return value;
}

void update_clock(bool up_down) {
    uint8_t val;
    bool pm;

    ds1307_stop_clock(); // Stop clock
    ds1307_get_clock(); // Sync
    switch (g_clk_pos) {
        case CLK_POS_HR:
            // Check if 12hr or 24hr then update accordly
            if (ds1307_is_12hr_mode()) {
                val = ds1307_get_hours();
                pm = ds1307_is_pm();
                if (((val == 12) && (!up_down)) || ((val == 11) && up_down)) {
                    pm = !pm; // Change pm/am
                }
                if ((val == 12) && (up_down)) {
                    val = 1; // Change from 12am/pm to 1am/pm
                } else if ((val == 1) && (!up_down)) {
                    val = 12; // Change from 1am/pm to 12am/pm
                } else if (up_down) {
                    val++;
                } else {
                    val--;
                }
                ds1307_set_pm_am(pm); // Set pm/am
                ds1307_set_hours(val); // Set new hr
            } else { // 24hr mode
                val = ds1307_get_hours();
                val = adjust_val(val, 0, 23, up_down);
                ds1307_set_hours(val); // Set new hr
            }
            break;
        case CLK_POS_MIN:
            val = ds1307_get_minutes();
            val = adjust_val(val, 0, 59, up_down);
            ds1307_set_minutes(val);
            break;
        case CLK_POS_SEC:
            val = ds1307_get_seconds();
            val = adjust_val(val, 0, 59, up_down);
            ds1307_set_seconds(val);
            break;
        case CLK_POS_MON:
            val = ds1307_get_month();
            val = adjust_val(val, 1, 12, up_down);
            ds1307_set_month(val);
            break;
        case CLK_POS_DAY:
            val = ds1307_get_day_of_month();
            val = adjust_val(val, 1, 31, up_down);
            ds1307_set_day_of_month(val);
            break;
        case CLK_POS_YR:
            val = ds1307_get_year();
            val = adjust_val(val, 0, 99, up_down);
            ds1307_set_year(val);
            break;
        case CLK_POS_HR_MOD:
            if (ds1307_is_12hr_mode() == true) {
                ds1307_switch_to_24hr();
            } else {
                ds1307_switch_to_12hr();
            }
            break;
    }
    ds1307_set_clock(); // Sync and start clock
    //ds1307_start_clock(); // Start clock
}

void interrupt tmr_isr(void) {
    write_t1(g_reload_value); // Manual reload timer value
    g_counter1++;

    // Call tasks here every 50ms
    switch (keypad_read_key()) {
        case KEYPAD_KEY_UP:
            update_clock(true);
            break;
        case KEYPAD_KEY_DN:
            update_clock(false);
            break;
        case KEYPAD_KEY_LEFT:
            if (g_clk_pos == CLK_POS_HR) { // First position?
                g_clk_pos = CLK_POS_HR_MOD; // Go to last position
            } else {
                g_clk_pos--; // Decrement position
            }
            break;
        case KEYPAD_KEY_RIGHT:
            if (g_clk_pos == CLK_POS_HR_MOD) { // Last position?
                g_clk_pos = CLK_POS_HR; // Go to first position
            } else {
                g_clk_pos++; // Increment position
            }
            break;
        case KEYPAD_KEY_BL:
            g_bl = !g_bl;
            break;
    }

    // Call tasks here every x
    if (g_counter1 == 10) { // 500ms
        uint8_t time[10], date[10], pos[2];

        ds1307_get_clock();
        ds1307_time_formatted(time);
        ds1307_date_formatted(date);
        lcd_goto(1, 1);
        lcd_write((uint8_t *) "Time: ");
        lcd_goto(1, 7);
        lcd_write(time);
        lcd_goto(2, 1);
        lcd_write((uint8_t *) "Date: ");
        lcd_goto(2, 7);
        lcd_write(date);
        lcd_goto(2, 16);
        sprintf(pos, "%u", g_clk_pos);
        lcd_write(pos);

        lcd_backlight(g_bl);
        g_counter1 = 0;
    }

    PIR1bits.TMR1IF = 0; // Clear interrupt flag
} //This return will be a "retfie fast"