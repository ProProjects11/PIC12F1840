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
 * File Name:       at24c32.h
 * Created On:      August 1, 2014, 3:55 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      08/01/14    Initial version
 *********************************************************************/

#ifndef __AT24C32_H
#define	__AT24C32_H

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

/** INTERFACE CONFIGURATION ****************************************/
#define AT24C32_STATIC_ADDRESS 0b1010 // Static address (4 MSB)

/** PUBLIC FUNCTIONS ***********************************************/
/**
 * Initialize the AT24C32 I2C memory
 * @param addr_3b Physical 3b address
 */
void at24c32_init(uint8_t addr_3b);
/**
 * Set the 3b physical address of the device
 * @param addr_3b Physical 3b address
 */
void at24c32_set_slave_addr(uint8_t addr_3b);
/**
 * Write a byte to the specified address
 * @param addr_12b 12b Address
 * @param value Value to write
 */
void at24c32_write_byte(uint16_t addr_12b, uint8_t value);
/**
 * This function checks if memory is busy writing
 * @return 1 = true = memory is busy, 0 = false = memory is not busy
 */
bool at24c32_is_write_busy(void);
/**
 * Read a byte from memory
 * @param addr_12b 12b address to read from
 * @return Read value
 */
uint8_t at24c32_read_byte(uint16_t addr_12b);

#endif	/* __AT24C32_H */

