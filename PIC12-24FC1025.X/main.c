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
#include "m24fc1025.h"

/** DEFINES ********************************************************/

/** PROTOTYPES *****************************************************/
#define SetClockTo32Mhz()  OSCCONbits.IRCF = 0b1110; OSCCONbits.SPLLEN = 1
void delay_ms(uint16_t);

/** GLOBAL VARIABLES ***********************************************/

/** CODE DECLARATIONS ****************************************/
void main(void) {
    SetClockTo32Mhz();

    delay_ms(1000); // Wait for proteus to load simulation

    i2c_init(I2C_SPEED_STANDARD_100KHZ);
    m24fc1025_init(0b00);
    mcp23017_init(0b001); // Init MCP23017 with the address 001
    mcp23017_write_reg(MCP23017_REG_IODIRA, 0x00); // IOA configure as output
    mcp23017_write_reg(MCP23017_REG_IODIRB, 0x00); // IOB configure as output

    // Write first 16 Bytes of memory
    uint32_t addr = 0;
    for (uint8_t c = 0x01; addr < 8; c = c << 1, addr++) { // Rotate bit
        m24fc1025_write_byte(addr, c);
        while (m24fc1025_is_write_busy()); // Wait for write to finish
    }
    for (uint8_t c = 0x80; addr < 16; c = c >> 1, addr++) { // Rotate bit to the other side
        m24fc1025_write_byte(addr, c);
        while (m24fc1025_is_write_busy()); // Wait for write to finish
    }

    // Write on the address 0x1000+ 16 Bytes of memory
    uint32_t addr = 0;
    for (uint8_t c = 0x80; addr < 8; c = c >> 1, addr++) { // Rotate bit
        m24fc1025_write_byte(addr + 0x1000, c);
        while (m24fc1025_is_write_busy()); // Wait for write to finish
    }
    for (uint8_t c = 0x01; addr < 16; c = c << 1, addr++) { // Rotate bit to the other side
        m24fc1025_write_byte(addr + 0x1000, c);
        while (m24fc1025_is_write_busy()); // Wait for write to finish
    }

    for (;;) {
        // Read data and write it to IOA
        for (uint32_t addr = 0; addr < 16; addr++) {
            mcp23017_write_reg(MCP23017_REG_GPIOA, m24fc1025_read_byte(addr)); // Write to IOA
            mcp23017_write_reg(MCP23017_REG_GPIOB, m24fc1025_read_byte(addr + 0x1000)); // Write to IOB
            delay_ms(250); // Wait some ms between cycles
        }
    }
}

void delay_ms(uint16_t t_ms) {
    for (t_ms; t_ms > 0; t_ms--) {
        for (uint16_t c = 886; c > 0; c--);
    }
}