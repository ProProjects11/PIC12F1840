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
 * File Name:       mcp23017.h
 * Created On:      July 29, 2014, 5:37 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/29/14    Initial version
 *********************************************************************/

#ifndef __MCP23017_H
#define	__MCP23017_H

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

/** INTERFACE CONFIGURATION ****************************************/
#define MCP23017_STATIC_ADDRESS 0b0100 // Static address (4 MSB)

typedef enum {
    MCP23017_REG_IODIRA = 0x00,
    MCP23017_REG_IODIRB,
    MCP23017_REG_IPOLA,
    MCP23017_REG_IPOLB,
    MCP23017_REG_GPINTENA,
    MCP23017_REG_GPINTENB,
    MCP23017_REG_DEFVALA,
    MCP23017_REG_DEFVALB,
    MCP23017_REG_INTCONA,
    MCP23017_REG_INTCONB,
    MCP23017_REG_IOCON1,
    MCP23017_REG_IOCON2,
    MCP23017_REG_GPPUA,
    MCP23017_REG_GPPUB,
    MCP23017_REG_INTFA,
    MCP23017_REG_INTFB,
    MCP23017_REG_INTCAPA,
    MCP23017_REG_INTCAPB,
    MCP23017_REG_GPIOA,
    MCP23017_REG_GPIOB,
    MCP23017_REG_OLATA,
    MCP23017_REG_OLATB
} mcp23017_registers_t;


/** PUBLIC FUNCTIONS ***********************************************/
/**
 * Initialize the MCP23017 device
 * @param address_3b Physical 3b address designed in A2,A1 and A0
 */
void mcp23017_init(uint8_t address_3b);
/**
 * Set slave address
 * @param address_3b Physical 3b address designed in A2,A1 and A0
 */
void mcp23017_set_slave_addr(uint8_t address_3b);
/**
 * Write a byte to a specific register address
 * @param reg_address Address of the register
 * @param value Value to write to the register
 */
void mcp23017_write_reg(uint8_t reg_address, uint8_t value);
/**
 * Read a byte from a specific register address
 * @param reg_address Address of the register
 * @return The value read from the register
 */
uint8_t mcp23017_read_reg(uint8_t reg_address);

#endif	/* __MCP23017_H */

