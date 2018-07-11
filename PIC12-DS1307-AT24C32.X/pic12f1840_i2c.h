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
 * File Name:       pic12f1840_i2c.h
 * Created On:      July 27, 2014, 5:12 PM
 * Description:     
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rev.     Date        Comment
 * 1.0      07/27/14    Initial version
 * 1.1      07/29/14    Updated i2c_read_data() and added i2c_send_ack()
 * 1.2      07/30/14    Added i2c_read_ack() and Updated some functions
 **********************************************************************/

#ifndef __PIC12F1840_I2C_H
#define	__PIC12F1840_I2C_H

/** INCLUDES *******************************************************/
#include <stdint.h>
#include <stdbool.h>

/** INTERFACE CONFIGURATION ****************************************/
#define I2C_SPEED_STANDARD_100KHZ   100000
#define I2C_SPEED_FAST_400KHZ       400000
#define I2C_SPEED_FAST_1MHZ         1000000

#define I2C_SCL_INIT()  ANSELAbits.ANSA1 = 0; TRISAbits.TRISA1 = 1 // SCL Digital + Input
#define I2C_SDA_INIT()  ANSELAbits.ANSA2 = 0; TRISAbits.TRISA2 = 1 // SDA Digital + Input

#define I2C_ADDRESS_MODE_READ 1
#define I2C_ADDRESS_MODE_WRITE 0
#define I2C_ACK 0
#define I2C_NACK 1

/** PUBLIC FUNCTIONS ***********************************************/
/**
 * Initialize I2C.
 * @param clock_hz Define I2C clock speed (I2C_SPEED_STANDARD_100KHZ, I2C_SPEED_FAST_400KHZ, I2C_SPEED_FAST_1MHZ).
 */
void i2c_init(uint32_t clock_hz);
/**
 * Wait for I2C transfer to finish
 */
void i2c_wait(void);
/**
 * Send I2C Start sequence
 */
void i2c_start(void);
/**
 * Send I2C ReStart sequence
 */
void i2c_restart(void);
/**
 * Send I2C Stop sequence
 */
void i2c_stop(void);
/**
 * Send Ack.
 * @param ack 0 = ACK, 1 = NACK (I2C_ACK, I2C_NACK).
 */
void i2c_send_ack(bool ack);
/**
 * Read the ACK bit
 * @return 0 = ACK, 1 = NACK (I2C_ACK, I2C_NACK).
 */
bool i2c_read_ack(void);
/**
 * Send I2C slave address and select R/W mode.
 * @param address 7-bit slave address
 * @param mode Read = 1, /Write = 0 (I2C_ADDRESS_MODE_READ, I2C_ADDRESS_MODE_WRITE)
 */
void i2c_send_address(uint8_t address, bool mode);
/**
 * Send I2C byte data.
 * @param data Byte data to send.
 */
void i2c_send_data(uint8_t data);
/**
 * Read I2C byte data.
 * @return Read data
 */
uint8_t i2c_read_data(void);

#endif	/* __PIC12F1840_I2C_H */

