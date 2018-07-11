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
 * File Name:       ds1307.h
 * Created On:      August 1, 2014, 4:40 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      08/01/14    Initial version
 *********************************************************************/

#ifndef __DS1307_H
#define	__DS1307_H

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

/** INTERFACE CONFIGURATION ****************************************/
#define DS1307_SLAVE_ADDR   0b01101000 // Static address

#define DS1307_RAM_SIZE     56 // 56 Bytes of RAM (0x00 -> 0x37)

#define DS1307_CONF_RS_1HZ      0x00 // 1Hz
#define DS1307_CONF_RS_4096HZ   0x01 // 4.096 KHz
#define DS1307_CONF_RS_8192HZ   0x10 // 8.192 KHz
#define DS1307_CONF_RS_32768HZ  0x11 // 32.768 KHz
#define DS1307_CONF_SQWE_OFF    0x00
#define DS1307_CONF_SQWE_ON     0x01
#define DS1307_CONF_SQWE_OUT_0  0x00
#define DS1307_CONF_SQWE_OUT_1  0x01

#define DS1307_SET_AM           0x00
#define DS1307_SET_PM           0x01
#define DS1307_HR_MODE_24HR     0x00
#define DS1307_HR_MODE_12HR     0x01

typedef enum {
    DS1307_REG_SECONDS = 0x00,
    DS1307_REG_MINUTES,
    DS1307_REG_HOURS,
    DS1307_REG_DAY_OW,
    DS1307_REG_DAY_OM,
    DS1307_REG_MONTH,
    DS1307_REG_YEAR,
    DS1307_REG_CONTROL,
    DS1307_REG_RAM_ADDR = 0x08
};

/** PUBLIC FUNCTIONS ***********************************************/
/**
 * Initialize ds1307 getting time and date from it.
 */
void ds1307_init(void);
/**
 * Read from a specific address from the device
 * @param addr Address to read from
 * @return  Read value
 */
uint8_t ds1307_read_addr(uint8_t addr);
/**
 * Write to a specific address from the device
 * @param addr Address to write to
 * @param value Value to write
 */
void ds1307_write_addr(uint8_t addr, uint8_t value);
/**
 * Stop the clock of the device
 */
void ds1307_stop_clock(void);
/**
 * Start the clock of the device
 */
void ds1307_start_clock(void);
/**
 * Check if clock is stopped
 * @return 1 = true = clock is stopped, 0 = false = clock is not stopped
 */
bool ds1307_is_stopped(void);
/**
 * Get time, clock and ctrl from device (Syncronize)
 */
void ds1307_get_clock(void);
/**
 * Set time and clock to device (ctrl is not assigned)
 */
void ds1307_set_clock(void);
/**
 * Configure control byte of device (Direct, No set_clock required)
 * @param out DS1307_CONF_SQWE_OUT_0, DS1307_CONF_SQWE_OUT_1
 * @param sqwe DS1307_CONF_SQWE_ON, DS1307_CONF_SQWE_OFF
 * @param rs DS1307_CONF_RS_1HZ, DS1307_CONF_RS_4096HZ, DS1307_CONF_RS_8192HZ, DS1307_CONF_RS_32768HZ
 */
void ds1307_set_control(uint8_t out, uint8_t sqwe, uint8_t rs);
/**
 * Check if 12hr mode is used
 * @return 1 = true = 12hr mode, 0 = false = 24hr mode
 */
bool ds1307_is_12hr_mode(void);
/**
 * Get the seconds
 * @return Seconds (0-59)
 */
uint8_t ds1307_get_seconds();
/**
 * Set the seconds
 * @param sec Seconds (0-59)
 */
void ds1307_set_seconds(uint8_t sec);
/**
 * Get minutes
 * @return Minutes (0-59)
 */
uint8_t ds1307_get_minutes();
/**
 * Set minutes
 * @param min Minutes (0-59)
 */
void ds1307_set_minutes(uint8_t min);
/**
 * Get hours
 * @return Hours (12hr mode 1-12, 24hr mode 0-23)
 */
uint8_t ds1307_get_hours();
/**
 * Set hours
 * @param hours Hours (12hr mode 1-12, 24hr mode 0-23)
 */
void ds1307_set_hours(uint8_t hours);
/**
 * Get day of the week
 * @return Day of the week (1-7)
 */
uint8_t ds1307_get_day_of_week();
/**
 * Set day of the week
 * @param dow Day of the week (1-7)
 */
void ds1307_set_day_of_week(uint8_t dow);
/**
 * Get day of the month
 * @return Day of the month (1-31)
 */
uint8_t ds1307_get_day_of_month();
/**
 * Set day of the month
 * @param dom Day of the month (1-31)
 */
void ds1307_set_day_of_month(uint8_t dom);
/**
 * Get month
 * @return Month (1-12)
 */
uint8_t ds1307_get_month(void);
/**
 * Set month
 * @param month Month (1-12)
 */
void ds1307_set_month(uint8_t month);
/**
 * Get year
 * @return Year (0-99)
 */
uint8_t ds1307_get_year(void);
/**
 * Set year
 * @param year Year (0-99)
 */
void ds1307_set_year(uint8_t year);
/**
 * Check if its pm (Only for 12hr mode)
 * @return 1 = true = PM, 0 = false = AM
 */
bool ds1307_is_pm(void);
/**
 * Set pm or am (Only for 12hr mode)
 * @param pm_am DS1307_SET_AM, DS1307_SET_PM
 */
void ds1307_set_pm_am(bool pm_am);
/**
 * Set the hr mode
 * @param mode DS1307_HR_MODE_24HR, DS1307_HR_MODE_12HR
 */
void ds1307_set_hr_mode(bool mode);
/**
 * Switch to 12hr mode (Only for 24hr mode)
 */
void ds1307_switch_to_12hr(void);
/**
 * Switch to 24hr mode (Only for 12hr mode)
 */
void ds1307_switch_to_24hr(void);
/**
 * Write a byte to the DS1307 RAM (56 Bytes)
 * @param addr Address (0x00-0x37)
 * @param value Byte to write
 */
void ds1307_write_ram(uint8_t addr, uint8_t value);
/**
 * Read a byte from the DS1307 RAM (56 Bytes)
 * @param addr Address (0x00-0x37)
 * @return Read byte
 */
uint8_t ds1307_read_ram(uint8_t addr);
/**
 * Format time to HH:MM:SS[P/A]+NULL
 * @param p_str_time Pointer to buffer[10]
 */
void ds1307_time_formatted(char* p_str_time);
/**
 * Format date to MM/DD/YY+NULL
 * @param p_str_date Pointer to buffer[9]
 */
void ds1307_date_formatted(char* p_str_date);
#endif	/* __DS1307_H */

