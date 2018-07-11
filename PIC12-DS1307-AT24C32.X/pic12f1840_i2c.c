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
 * File Name:       pic12f1840_i2c.c
 * Created On:      July 27, 2014, 5:12 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/27/14    Initial version
 * 1.1      07/29/14    Updated i2c_read_data() and added i2c_send_ack()
 * 1.2      07/30/14    Added i2c_read_ack() and Updated some functions
 *********************************************************************/

/** INCLUDES *******************************************************/
#include "pic12f1840_i2c.h"
#include "main.h"

/** PRIVATE VARIABLES **********************************************/

/** PRIVATE FUNCTION PROTOTYPES ************************************/

/** PRIVATE FUNCTION DEFINITIONS ***********************************/

/** PUBLIC FUNCTION DEFINITIONS ************************************/
void i2c_init(uint32_t clock_hz) {
    // Configure SCL and SDA pins
    I2C_SCL_INIT(); // SCL digital and input
    I2C_SDA_INIT(); // SDA digital and input

    // Configure MSSP for I2C Master mode
    SSPCON1 = 0b00101000; // I2C Master mode, SSP1 Enable
    SSPCON2 = 0x00; // Reset all states

    // Configure I2C Clock (Speed)
    // Fclock = Fosc / [(SSPxADD + 1)*(4)]
    // .^. SSPxADD<7> = [Fosc / (4*Fclock)] - 1
    // SSPxADD must be between 0x03 -> 0x7F (3 -> 127)
    SSPADD = (uint8_t) (_XTAL_FREQ / (4 * clock_hz)) - 1;

    // In I2 C Master or Slave mode:
    // 1 = Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)
    // 0 = Slew rate control enabled for high speed mode (400 kHz)
    if (clock_hz == I2C_SPEED_FAST_400KHZ) {
        SSPSTATbits.SMP = 0; // 400KHz
    } else {
        SSPSTATbits.SMP = 1; // Rest speeds
    }
}

void i2c_wait(void) {
    // Wait for flags go idle (Start, Repeat Start, Stop, Receive, Acknowledge)
    // and wait for transmit to finish
    while ((SSP1CON2 & 0x1F) || (SSPSTATbits.R_nW));
}

void i2c_start(void) {
    i2c_wait();
    SSP1CON2bits.SEN = 1; // Send Start sequence
}

void i2c_restart(void) {
    i2c_wait();
    SSP1CON2bits.RSEN = 1; // Send ReStart sequence
}

void i2c_stop(void) {
    i2c_wait();
    SSP1CON2bits.PEN = 1; // Send Stop sequence
}

void i2c_send_ack(bool ack) {
    i2c_wait();
    SSP1CON2bits.ACKDT = ack; // Load acknowledge value
    SSP1CON2bits.ACKEN = 1; // Send acknowledge
}

bool i2c_read_ack(void) {
    i2c_wait();
    return (SSP1CON2bits.ACKSTAT); // Return ACK
}

void i2c_send_address(uint8_t address, bool mode) {
    i2c_wait();
    address <<= 1; // Shift 1 bit to the left
    address += mode; // Add the R/W bit
    SSPBUF = address; // Send new address
}

void i2c_send_data(uint8_t data) {
    i2c_wait();
    SSPBUF = data; // Send data
}

uint8_t i2c_read_data(void) {
    uint8_t read_data;

    i2c_wait();
    SSP1CON2bits.RCEN = 1; // Start reception
    i2c_wait(); // Wait for reception to complete
    read_data = SSPBUF; // Read received data

    return read_data; // Return read data
}
