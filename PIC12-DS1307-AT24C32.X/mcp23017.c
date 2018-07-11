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
 * File Name:       mcp23017.c
 * Created On:      July 29, 2014, 5:38 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/29/14    Initial version
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "mcp23017.h"
#include "pic12f1840_i2c.h"

/** PRIVATE VARIABLES **********************************************/
static uint8_t g_slave_address_7b;

/** PRIVATE FUNCTION PROTOTYPES ************************************/

/** PRIVATE FUNCTION DEFINITIONS ***********************************/

/** PUBLIC FUNCTION DEFINITIONS ************************************/
void mcp23017_init(uint8_t address_3b) {
    // Set new address
    mcp23017_set_slave_addr(address_3b);
}

void mcp23017_set_slave_addr(uint8_t address_3b) {
    // New address = X<7> + OPCODE<6:3> + address<2:0>
    g_slave_address_7b = (MCP23017_STATIC_ADDRESS << 3) | address_3b;
}

void mcp23017_write_reg(uint8_t reg_address, uint8_t value) {
    // Write Byte = |S|DIR+W|ADDR|DIN|P|
    i2c_start();
    i2c_send_address(g_slave_address_7b, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(reg_address);
    i2c_send_data(value);
    i2c_stop();
}

uint8_t mcp23017_read_reg(uint8_t reg_address) {
    uint8_t value;

    // Read Byte = |S|DIR+W|ADDR|SR|DIR+R|DOUT|P|
    i2c_start();
    i2c_send_address(g_slave_address_7b, I2C_ADDRESS_MODE_WRITE);
    i2c_send_data(reg_address);
    i2c_restart();
    i2c_send_address(g_slave_address_7b, I2C_ADDRESS_MODE_READ);
    value = i2c_read_data();
    i2c_stop();

    return value;
}
