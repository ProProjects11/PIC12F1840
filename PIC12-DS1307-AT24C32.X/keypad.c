/*
 *	Copyright (c) 2011-2014, http://www.proprojects.wordpress.com
 *	All rights reserved.
 *
 * 	Redistribution and use in source and binary forms, with or without modification, 
 *	are permitted provided that the following conditions are met:
 *
 *	1.- Redistributions of source code must retain the above copyright notice,
 *	    this list of conditions and the following disclaimer.
 *	2.- Redistributions in binary form must reproduce the above copyright notice, 
 *	    this list of conditions and the following disclaimer in the documentation
 *	    and/or other materials provided with the distribution.
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
 * File Name:       keypad.c
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      08/04/14    Initial version
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "keypad.h"
#include "mcp23017.h"

/** PRIVATE VARIABLES **********************************************/
static uint8_t g_old_key; // For debounce function
static uint8_t g_last_valid_key; // For no auto-repeat function

/** PRIVATE FUNCTION PROTOTYPES ************************************/

/** PRIVATE DEFINITIONS ********************************************/
uint8_t keypad_scan(void);

/** PRIVATE FUNCTION DEFINITIONS ***********************************/
uint8_t keypad_scan(void){
    // Read the keys from the keypad
    return mcp23017_read_reg(MCP23017_REG_GPIOB);
}

/** PUBLIC FUNCTION DEFINITIONS ************************************/
void keypad_init(void) {
    mcp23017_write_reg(MCP23017_REG_IODIRB, 0xFF); // IOB configure as input
    mcp23017_write_reg(MCP23017_REG_GPPUB,0xFF); // IOB pull ups on
    mcp23017_write_reg(MCP23017_REG_IPOLB,0xFF); // Invert logic
}

uint8_t keypad_read_key(void) {
    uint8_t key;

    key = keypad_scan(); // Read a key from keypad
    if (key == NULL) {
        // No key so reset debounce and no-repeat
        g_old_key = NULL;
        g_last_valid_key = NULL;
    } else if (key == g_old_key) { // Second time same key, debounce pass?
        // Debounce pass
        if (key != g_last_valid_key) { // No repeat key
            // A valid key
            g_last_valid_key = key; // Store new valid key
        }
        else{
            key = NULL;
        }
    } else { // A key, but first time
        g_old_key = key; // Store the key for debounce check
        key = NULL;
    }
    return key;
}
