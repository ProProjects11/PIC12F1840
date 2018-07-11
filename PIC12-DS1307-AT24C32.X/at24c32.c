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
 * File Name:       at24c32.c
 * Created On:      August 1, 2014, 3:55 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      08/01/14    Initial version
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "at24c32.h"
#include "main.h"
#include "pic12f1840_i2c.h"

/** PRIVATE VARIABLES **********************************************/
uint8_t g_slave_addr_7b;

/** PRIVATE FUNCTION PROTOTYPES ************************************/

/** PRIVATE FUNCTION DEFINITIONS ***********************************/

/** PUBLIC FUNCTION DEFINITIONS ************************************/

void at24c32_init(uint8_t addr_3b) {
    // Set new address
    at24c32_set_slave_addr(addr_3b);
}

void at24c32_set_slave_addr(uint8_t addr_3b) {
    // New address 7b = STATIC[4]<6:3> + address[3]<2:0>
    g_slave_addr_7b = (AT24C32_STATIC_ADDRESS << 3) | (addr_3b & 0x03);
}

void at24c32_write_byte(uint16_t addr_12b, uint8_t value) {
    // Write byte = 
    // |S|1010+A2+A1+A0+W|ACK|0000<15:12>+AH<11:8>|ACK|AL<7:0>|ACK|DIN|ACK|P|
    i2c_start();
    i2c_send_address(g_slave_addr_7b, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data((uint8_t) ((addr_12b >> 8) & 0x0F));
    i2c_send_data((uint8_t) addr_12b);
    i2c_send_data(value);
    i2c_stop();
}

bool at24c32_is_write_busy(void) {
    bool read_ack;

    // To check if write is busy, write the same control byte and check ack
    // if ack is not received write is busy, otherwise write is done.
    i2c_start();
    i2c_send_address(g_slave_addr_7b, I2C_ADDRESS_MODE_WRITE);
    read_ack = i2c_read_ack();
    i2c_stop();
    return read_ack; // ACK = 0, so memory is not busy
}

uint8_t at24c32_read_byte(uint16_t addr_12b) {
    uint8_t read_data;

    // Read byte =
    // |S|1010+A2+A1+A0+W|ACK|0000<15:12>+AH<11:8>|ACK|AL<7:0>|ACK|RS|1010+A2+A1+A0+R|ACK|DOUT|NACK|S|
    i2c_start();
    i2c_send_address(g_slave_addr_7b, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data((uint8_t) ((addr_12b >> 8) & 0x0F));
    i2c_send_data((uint8_t) addr_12b);
    i2c_restart();
    i2c_send_address(g_slave_addr_7b, I2C_ADDRESS_MODE_READ);
    read_data = i2c_read_data();
    i2c_send_ack(I2C_NACK);
    i2c_stop();
    
    return read_data;
}
