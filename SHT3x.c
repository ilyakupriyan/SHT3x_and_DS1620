/*************************************************************
*
*** Driver for sensor family SHT3x (FOST02)
*** Realized on program I2C 
*
**************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "define_IO_PORT.h"
#include <util/delay.h>
#include "SHT1x.h"

// *** Сonfiguring the configuration of pins connected to the sensor  HST3x
#define GLUE(a, b)     	a##b
#define PORT(x)			GLUE(PORT, x)
#define PIN(x)         	GLUE(PIN, x)
#define DDR(x)         	GLUE(DDR, x)

#define SHT3x_PORTOUT	PORT(SHT3x_PORT)
#define SHT3x_PORTIN	PIN(SHT3x_PORT)
#define SHT3x_DDR		DDR(SHT3x_PORT)
//status register to default values. Wait minimum
//11 ms before next command

//Start command bit for I2C
static void i2c_start() {
  	L_SDA();
  	DELAY();
  	L_SCL();
	DELAY();
}
//Stop command bit for  I2C
static void i2c_stop() {
	L_SCL();
	DELAY();
	L_SDA();
	DELAY();

	H_SCL();
	DELAY();
	H_SDA();
	DELAY();
}

//Command for receaving byte data from sensor
static uint8_t i2c_read(uint8_t ack) {
  	uint8_t byte = 0, i=8;
		
	while(i--) {
		H_SCL();
		DELAY();
		if(SHT3x_PORTIN & (1 << SHT3x_SDA)) byte |= (1 << i);
		L_SCL();
		DELAY();
	}		
	if(ack) {
		L_SDA();
	} else {
		H_SDA();
	}
	H_SCL();
	DELAY();
	L_SCL();
	DELAY();
	H_SDA();
			
	return byte;
}

//Command for transmiting byte data to sensor
static uint8_t i2c_send(uint8_t data) {
	uint8_t i=8, ask;
	
	while(i--) {
		if (data & (1 << i)) { 
			H_SDA(); 
		}
		else { 
			L_SDA(); 
		}
		DELAY();
		H_SCL();
		DELAY();
		L_SCL();
	}
	H_SDA();
	DELAY();
	H_SCL();
	DELAY();
	ask = (SHT3x_PORTIN & (1 << SHT3x_SDA));
	L_SCL();
		
	return ask;
}

// *** Initialization HST3x
void SHT3x_ini (void) {
	SHT3x_DDR |= _BV(SHT3x_VDD);	
	SHT3x_PORTOUT |= _BV(SHT3x_VDD);
	H_SCL();
	H_SDA();

	_delay_ms(10);									//required delay 10 mS
}

// *** Turning off HST3x
void SHT3x_off (void) {
	SHT3x_DDR &= ~_BV(SHT3x_VDD);
	SHT3x_PORTOUT &= ~_BV(SHT3x_VDD);
}

// *** Measuring temperature and relative humanity.
int32_t meas_th(int16_t COMMAND) {
	uint8_t data[6] = {0, 0, 0, 0, 0, 0};		//[0] - T MSB, [1] - T LSB, [2] - T CRC, [3] - RH MSB, [1] - RH LSB, [2] - RH CRC
	uint8_t CRC = 0xFF;		//Check sum
	uint32_t res = 0;
	uint8_t I, K;   

	i2c_start();
	i2c_send(ADDR_SHT3X << 1);
	i2c_send(COMMAND >> 8);
	i2c_send(COMMAND & 0xFF);
	i2c_stop();

	_delay_ms(30);

	i2c_start();
	i2c_send((ADDR_SHT3X << 1) | 1);
	data[0] = i2c_read(1);
	data[1] = i2c_read(1);
	data[2] = i2c_read(1);
	data[3] = i2c_read(1);
	data[4] = i2c_read(1);
	data[5] = i2c_read(0);
	i2c_stop();
	
	// *** Check CRC of T
	for (I = 0; I < 2; I++) {
		CRC ^= data[I];
		for (K = 0; K <= 7; K++) {
			CRC = (CRC & 0x80) ? (CRC << 1) ^ 0x31 : (CRC << 1);
		}
	}
	if (CRC != data[2]) return -5;
	
	CRC = 0xFF;
	
	// *** Check CRC of RH
	for (I = 3; I < 5; I++) {
		CRC ^= data[I];
		for (K = 0; K <= 7; K++) {
			CRC = (CRC & 0x80) ? (CRC << 1) ^ 0x31 : (CRC << 1);
		}
	}
	if (CRC != data[5]) return -6;

	//Formation of the final result
	res = (res | data[0]) << 8;
	res = (res | data[1]) << 8;
	res = (res | data[3]) << 8;
	res |= data[4];
	
	return res;
}

// ***  Status Register Read
int16_t srrd(void) {
	uint8_t REG[3];													// 2 status register + CRC
	uint8_t I,K;
	uint8_t CRC=0xFF;												// CRC
	int16_t res = 0;

	i2c_start();
	i2c_send(ADDR_SHT3X << 1);
	i2c_send(RSR >> 8);			
	i2c_send(RSR & 0xFF);		
	i2c_stop();
	
	_delay_ms(5);

	i2c_start();
	i2c_send((ADDR_SHT3X << 1) | 1);
	REG[0] = i2c_read(1);							
	REG[1] = i2c_read(1);							
	REG[2] = i2c_read(0);
	i2c_stop();

	// *** Check CRC
	for (I = 0; I < 2; I++) {
		CRC ^= REG[I];
		for (K = 0; K <= 7; K++) {
			CRC = (CRC & 0x80) ? ((CRC << 1) ^ 0x31) : (CRC << 1);
		}
	}
	if (CRC != REG[2]) return -1;

	res = (res | REG[0]) << 8;
	res |= REG[1];

	return res;
}

// *** Forced reset
void conres(void) {
	uint16_t I;
	
	H_SDA();
	L_SCL();
	for(I = 0; I <= 9; I++) {
		DELAY();
		H_SCL();
		DELAY();
		L_SCL();
	}
}

// *** Soft reset, resets the interface, clears the
// status register to default values.
void softreset (void) {
	uint8_t COMM[2] = { ((SR & (0xFF << 8)) >> 8), (SR & 0xFF) }; //Formation of 2 bytes command soft reset for transmiting

	i2c_start();
	i2c_send(ADDR_SHT3X << 1);
	i2c_send(COMM[0]);
	i2c_send(COMM[1]);

	i2c_stop();

	_delay_ms(15);
}

//I2C general call for SHT3x
void SHT3x_gen_call(void) {

	uint8_t COMM[2] = { ((GENERAL_CALL & (0xFF << 8)) >> 8), (GENERAL_CALL & 0xFF) };

	i2c_start();
	i2c_send(COMM[0]);
	i2c_send(COMM[1]);

	_delay_ms(10);
}
