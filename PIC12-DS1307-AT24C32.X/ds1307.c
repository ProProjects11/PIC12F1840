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
 *	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 *	WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*********************************************************************
 * By:              Omar Gurrola
 * Company:         http://proprojects.wordpress.com
 * Processor:       PIC12
 * Compiler:        XC8 v1.32
 * File Name:       ds1307.c
 * Created On:      August 1, 2014, 4:40 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      08/01/14    Initial version
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "ds1307.h"
#include "main.h"
#include "pic12f1840_i2c.h"
#include <stdio.h>

/** PRIVATE DEFINES ************************************************/

/** PRIVATE VARIABLES **********************************************/
typedef union {
    uint8_t byte; // |OUT<7>,00<6:5>,SQWE<4>,00<3:2>,RS1<1>,RS0<0>|

    struct {
        unsigned rate_sel : 2;
        unsigned dummy : 2;
        unsigned sqw_enable : 1;
        unsigned dummy2 : 2;
        unsigned out_lvl_when_off : 1;
    };
} reg_ctrl_t;

static uint8_t g_reg_data[7];
static reg_ctrl_t g_reg_ctrl;

/** PRIVATE FUNCTION PROTOTYPES ************************************/
uint8_t dec_to_bcd(uint8_t);
uint8_t bcd_to_dec(uint8_t);

/** PRIVATE FUNCTION DEFINITIONS ***********************************/
uint8_t dec_to_bcd(uint8_t dec) {
    // dec / 10 = high value, just move it to <7:4>
    // dec % 10 = rest, just insert it to <3:0>
    return (((uint8_t) (dec / 10) << 4) | (dec % 10));
}

uint8_t bcd_to_dec(uint8_t bcd) {
    // 10 value = (bcd >> 4) * 10
    // 1 value = clear high nibble
    // add both
    return (((bcd >> 4)*10) + (bcd & 0x0F));
}

/** PUBLIC FUNCTION DEFINITIONS ************************************/
void ds1307_init(void) {
    // Syncronize data from clock
    ds1307_get_clock();
}

uint8_t ds1307_read_addr(uint8_t addr) {
    uint8_t read_value;

    // Read Byte =
    // |S|1101000W|A|ADDR|A|SR|1101000R|A|DATA|NA|P|
    i2c_start();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(addr); // Send address location
    i2c_restart();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_READ);
    read_value = i2c_read_data();
    i2c_send_ack(I2C_NACK);
    i2c_stop();

    return read_value;
}

void ds1307_write_addr(uint8_t addr, uint8_t value) {
    // Write Byte =
    // |S|1101000W|A|ADDR|A|DATA|A|P|

    i2c_start();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(addr); // Send address location
    i2c_send_data(value); // Send register value
    i2c_stop();
}

void ds1307_stop_clock(void) {
    // Bit 7 of Register 0x00 is the clock halt (CH) bit.
    // CH = 1, the oscillator is disabled.
    // CH = 0, the oscillator is enabled.

    uint8_t tsec;
    tsec = ds1307_read_addr(DS1307_REG_SECONDS);
    ds1307_write_addr(DS1307_REG_SECONDS, (tsec | 0x80));
    g_reg_data[DS1307_REG_SECONDS] = g_reg_data[DS1307_REG_SECONDS] | 0x80; // CH = 1
}

void ds1307_start_clock(void) {
    // Bit 7 of Register 0x00 is the clock halt (CH) bit.
    // CH = 1, the oscillator is disabled.
    // CH = 0, the oscillator is enabled.

    uint8_t tsec;
    tsec = ds1307_read_addr(DS1307_REG_SECONDS);
    ds1307_write_addr(DS1307_REG_SECONDS, (tsec & 0x7F));
    g_reg_data[DS1307_REG_SECONDS] = g_reg_data[DS1307_REG_SECONDS] & 0x7F; // CH = 0
}

bool ds1307_is_stopped(void) {
    return ((g_reg_data[DS1307_REG_SECONDS] & 0x80) == 0x80);
}

void ds1307_get_clock(void) {
    // Read Continuos =
    // |S|1101000W|A|ADDR|A|SR|1101000R|A|DATA0|A|DATA1|A|....|DATA_N|NA|P|
    i2c_start();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(0x00); // First address location
    i2c_restart();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_READ);
    for (uint8_t r = 0x00; r < 0x07; r++) { // 0x00 -> 0x06

        g_reg_data[r] = i2c_read_data();
        i2c_send_ack(I2C_ACK);
    }
    g_reg_ctrl.byte = i2c_read_data(); // 0x07 = Control register
    i2c_send_ack(I2C_NACK);
    i2c_stop();
}

void ds1307_set_clock(void) {
    // Stop clock, then write all registers but with Seconds<7> CH = 1 (Clk off)
    // then start the clock again

    ds1307_stop_clock();
    g_reg_data[DS1307_REG_SECONDS] = g_reg_data[DS1307_REG_SECONDS] | 0x80;
    i2c_start();
    i2c_send_address(DS1307_SLAVE_ADDR, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(0x00); // Initial address location
    for (uint8_t r = 0x00; r < 0x07; r++) {
        i2c_send_data(g_reg_data[r]);
    }
    i2c_stop();
    ds1307_start_clock();
}

void ds1307_set_control(uint8_t out, uint8_t sqwe, uint8_t rs) {

    g_reg_ctrl.out_lvl_when_off = out;
    g_reg_ctrl.sqw_enable = sqwe;
    g_reg_ctrl.rate_sel = rs;
    ds1307_write_addr(DS1307_REG_CONTROL, g_reg_ctrl.byte);
}

bool ds1307_is_12hr_mode(void) {
    // Bit 6 of the hours register is defined as the 12-hour or 24-hour mode-select bit
    // Hours<6> = 1, 12 hour mode selected
    return ((g_reg_data[DS1307_REG_HOURS] & 0x40) == 0x40);
}

uint8_t ds1307_get_seconds() {
    return bcd_to_dec(g_reg_data[DS1307_REG_SECONDS] & 0x7F);
}

void ds1307_set_seconds(uint8_t sec) {
    // Update only if seconds is valid
    if (sec < 60) {
        // Keep the CH bit <7>
        g_reg_data[DS1307_REG_SECONDS] = dec_to_bcd(sec) | (g_reg_data[DS1307_REG_SECONDS] & 0x80);
    }
}

uint8_t ds1307_get_minutes() {
    return bcd_to_dec(g_reg_data[DS1307_REG_MINUTES]);
}

void ds1307_set_minutes(uint8_t min) {
    // Only if data is valid
    if (min < 60) {
        g_reg_data[DS1307_REG_MINUTES] = dec_to_bcd(min);
    }
}

uint8_t ds1307_get_hours() {
    uint8_t hours;
    if (ds1307_is_12hr_mode()) {
        // Hours 12hr mode = Hx10<4>,Hx1<3:0>
        hours = g_reg_data[DS1307_REG_HOURS] & 0x1F;
    } else {
        // Hours 24hr mode = Hx10<5:4>,Hx1<3:0>
        hours = g_reg_data[DS1307_REG_HOURS] & 0x3F;
    }
    return bcd_to_dec(hours);
}

void ds1307_set_hours(uint8_t hours) {
    if (ds1307_is_12hr_mode()) {
        // Check if hours is valid for 12h
        if ((hours > 0) && (hours < 13)) { // 1 -> 12 hrs
            // Save mode bit <6> and pm bit <5>
            g_reg_data[DS1307_REG_HOURS] = dec_to_bcd(hours) | (g_reg_data[DS1307_REG_HOURS] & 0x60);
        }
    } else {
        if (hours < 24) { // 0 -> 23
            // Save mode bit (24hr)
            g_reg_data[DS1307_REG_HOURS] = dec_to_bcd(hours) | (g_reg_data[DS1307_REG_HOURS] & 0x40);
        }
    }
}

uint8_t ds1307_get_day_of_week() {

    return (g_reg_data[DS1307_REG_DAY_OW] & 0x07);
}

void ds1307_set_day_of_week(uint8_t dow) {
    if (dow < 8) { // 1 -> 7
        g_reg_data[DS1307_REG_DAY_OW] = dow;
    }
}

uint8_t ds1307_get_day_of_month() {

    return bcd_to_dec(g_reg_data[DS1307_REG_DAY_OM] & 0x3F);
}

void ds1307_set_day_of_month(uint8_t dom) {
    if ((dom > 0) && (dom < 32)) { // 1 -> 31
        g_reg_data[DS1307_REG_DAY_OM] = dec_to_bcd(dom);
    }
}

uint8_t ds1307_get_month(void) {
    return bcd_to_dec(g_reg_data[DS1307_REG_MONTH]);
}

void ds1307_set_month(uint8_t month) {
    if ((month > 0) && (month < 13)) { // 1 -> 12
        g_reg_data[DS1307_REG_MONTH] = dec_to_bcd(month);
    }
}

uint8_t ds1307_get_year(void) {

    return bcd_to_dec(g_reg_data[DS1307_REG_YEAR]);
}

void ds1307_set_year(uint8_t year) {
    if (year < 100) { // 00 -> 99
        g_reg_data[DS1307_REG_YEAR] = dec_to_bcd(year);
    }
}

bool ds1307_is_pm(void) {
    bool is_pm;
    if (ds1307_is_12hr_mode()) {
        // in 12hr mode bit 5 of hours is the pm (1 = PM)
        is_pm = ((g_reg_data[DS1307_REG_HOURS] & 0x20) == 0x20);
    } else {
        // in 24hr mode more than 11hr is pm (12 -> 23)

        is_pm = (ds1307_get_hours() > 11);
    }
    return is_pm;
}

void ds1307_set_pm_am(bool pm_am) {
    if (ds1307_is_12hr_mode()) { // Only for 12hr mode
        if (pm_am == DS1307_SET_PM) {
            // Bit 5 of Hours = 1 = PM
            g_reg_data[DS1307_REG_HOURS] = g_reg_data[DS1307_REG_HOURS] | 0b00100000;
        } else { // SET_AM
            // Bit 5 of Hours = 0 = AM
            g_reg_data[DS1307_REG_HOURS] = g_reg_data[DS1307_REG_HOURS] & 0b11011111;
        }
    }
}

void ds1307_set_hr_mode(bool mode) {
    // Bit 6 of hours is hr mode
    // 1 =  12hr, 0 = 24hr
    if (mode == true) {
        g_reg_data[DS1307_REG_HOURS] = g_reg_data[DS1307_REG_HOURS] | 0b01000000;
    } else {
        g_reg_data[DS1307_REG_HOURS] = g_reg_data[DS1307_REG_HOURS] & 0b10111111;
    }
}

void ds1307_switch_to_12hr(void) {
    if (ds1307_is_12hr_mode() == false) { // Only if it's 24Hr mode
        uint8_t hrs;
        bool pm_am;
        hrs = ds1307_get_hours();
        if (hrs < 12) { // 0 -> 11 = AM, 12 -> 23 = PM
            pm_am = DS1307_SET_AM;
        } else {
            pm_am = DS1307_SET_PM;
        }
        if (hrs == 0) {
            // 0 (24Hr) = 12 am
            hrs = 12;
        } else if (hrs > 12) {
            // 13 -> 23 (24Hr) = hr - 12 (12Hr)
            hrs = hrs - 12;
        }
        // 1 -> 12, hr is equal for both

        ds1307_set_hr_mode(DS1307_HR_MODE_12HR); // Set 12hr mode
        ds1307_set_pm_am(pm_am); // Set pm or am
        ds1307_set_hours(hrs); // new hrs
    }
}

void ds1307_switch_to_24hr(void) {
    if (ds1307_is_12hr_mode() == true) { // Only if it's 12Hr mode
        uint8_t hrs;
        bool pm_am;
        pm_am = ds1307_is_pm();
        hrs = ds1307_get_hours();
        if ((!pm_am) && (hrs == 12)) {
            // 12am = 0 (24hrs)
            hrs = 0;
        } else if (pm_am && (hrs < 12)) {
            // 1pm -> 11pm = hr +12 (24hr)
            hrs = hrs + 12;
        }
        // 1am -> 11am, 12pm = equal hrs (24hr)
        ds1307_set_hr_mode(DS1307_HR_MODE_24HR); // 24Hrs mode
        ds1307_set_hours(hrs); // new hrs
    }
}

void ds1307_write_ram(uint8_t addr, uint8_t value) {
    // RAM start at 0x08 - > 0x3F = 56KB
    // so address = 0x08 + addr

    ds1307_write_addr(DS1307_REG_RAM_ADDR + addr, value);
}

uint8_t ds1307_read_ram(uint8_t addr) {
    uint8_t data;
    // RAM start at 0x08 - > 0x3F = 56KB
    // so address = 0x08 + addr
    data = ds1307_read_addr(DS1307_REG_RAM_ADDR + addr);
    return data;
}

void ds1307_time_formatted(char* p_str_time) {
    if (ds1307_is_12hr_mode()) {
        // 12h Mode[10] = HH:MM:SSP/A+NULL
        sprintf(p_str_time, "%.2u:%.2u:%.2u", ds1307_get_hours(), ds1307_get_minutes(), ds1307_get_seconds());
        if (ds1307_is_pm()) {
            p_str_time[8] = 'P';
        } else {
            p_str_time[8] = 'A';
        }
        p_str_time[9] = NULL;
    } else {
        // 24h Mode[10] = HH:MM:SS+ +NULL
        sprintf(p_str_time, "%.2u:%.2u:%.2u ", ds1307_get_hours(), ds1307_get_minutes(), ds1307_get_seconds());
    }
}

void ds1307_date_formatted(char* p_str_date) {
    // String[9] = MM/DD/YY+NULL
    sprintf(p_str_date, "%.2u/%.2u/%.2u", ds1307_get_month(), ds1307_get_day_of_month(), ds1307_get_year());
}
