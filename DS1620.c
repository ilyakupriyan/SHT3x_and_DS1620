//driver for temperature sensor DS1620
 
#include <inttypes.h>
#include <avr/io.h>
#include <stdint.h>
#include "define_IO_PORT.h"
#include <util/delay.h>
#include "DS1620.h"


/* definition of the DS1620 temperature sensor control bit */

#define GLUE(a, b)     a##b
#define PORT(x)        GLUE(PORT, x)
#define PIN(x)         GLUE(PIN, x)
#define DDR(x)         GLUE(DDR, x)

#define DS1620_PORTOUT	PORT(DS1620_PORT)
#define DS1620_PORTIN	PIN(DS1620_PORT)
#define DS1620_DDR		DDR(DS1620_PORT)

// *** initializing the DS1620 temperature sensor port
void DS1620_ini (void)
{
	DS1620_DDR |= _BV(DS1620_RST);			// setting the RS port to output
	DS1620_PORTOUT &= ~_BV(DS1620_RST);		// setting 0 on the RST pin
	DS1620_DDR |= _BV(DS1620_CLK);			// setting the CLK port to output
	DS1620_PORTOUT |= _BV(DS1620_CLK);		// 
	CLK = 1DS1620_DDR |= _BV(DS1620_DQ);		// setting the DQ port to output
}

// *** disabling the DS1620 sensor port
void DS1620_off (void)
{
	DS1620_DDR &= ~(_BV(DS1620_RST) | _BV(DS1620_CLK) | _BV(DS1620_DQ));		//setting the port to high impedance
	DS1620_PORTOUT &= ~(_BV(DS1620_RST) | _BV(DS1620_CLK) | _BV(DS1620_DQ));	//disabling the suspender 47 kOm
}


// *** transmitting a byte of the exchange protocol 
void snd_prot(uint8_t J)
{
	uint8_t I;

 	for(I = 1; I ; I <<=1) {
    		if(J & I) {
	   		DS1620_PORTOUT |= _BV(DS1620_DQ);
		}
    		else
     		{
	   		DS1620_PORTOUT &= ~_BV(DS1620_DQ);
     		}
    		DS1620_clk();
  	}
}

// *** reading the status byte from the temperature sensor
uint8_t DS1620_rstat(void)
{
	uint8_t J;
	DS1620_PORTOUT |= _BV(DS1620_RST);
	snd_prot(0xAC);						//output of the status register status query command
	J = DS1620_rbyte();
	DS1620_PORTOUT &= ~_BV(DS1620_RST);
 	return J;
}

// *** clock output
void DS1620_clk (void)
{
	DS1620_PORTOUT &= ~_BV(DS1620_CLK);	//setting CLK to 0
	_delay_us(1);
	DS1620_PORTOUT |= _BV(DS1620_CLK);	//installing CLK in 1
	_delay_us(1);
}

// *** loading the configuration register
void DS1620_wconfig(uint8_t I)
{
  	DS1620_PORTOUT |= _BV(DS1620_RST);	// RST to 1
  	snd_prot(0x0C);				// output of the configuration register entry command
  	snd_prot(I);
  	DS1620_PORTOUT &= ~_BV(DS1620_RST);	// RST to 0
}

// *** starting the temperature conversion
void DS1620_start(void)
{
  	DS1620_PORTOUT |= _BV(DS1620_RST);	// RST to 1
  	snd_prot(0xEE);				// output of the configuration register entry command
  	DS1620_PORTOUT &= ~_BV(DS1620_RST);	// RST to 0
}

// *** reading the temperature value
int16_t DS1620_rtemp(void)
{
  	int16_t I;
  	uint8_t J;

  	while(!(DS1620_rstat() & 0x80));	// waiting for the ready flag
  	DS1620_PORTOUT |= _BV(DS1620_RST);	// RST to 1
  	snd_prot(0xAA);				// temperature reading command

  	I = DS1620_rbyte();
  	J = DS1620_rbyte() & 0x01;

  	DS1620_PORTOUT &= ~_BV(DS1620_RST);	// RST to 0

  	if(J) I |= 0xff << 8;

  	return I;
}

// *** reading the value of the nreg register data
int16_t DS1620_rdata(uint8_t nreg)
{
  	int16_t I;
  	uint8_t J;

 	while(!(DS1620_rstat() & 0x80));		// waiting for the ready flag
  		DS1620_PORTOUT |= _BV(DS1620_RST);	// RST to 1
  	snd_prot(nreg);					// temperature reading command
  	I = DS1620_rbyte();
  	J = DS1620_rbyte() & 0x01;
  	DS1620_PORTOUT &= ~_BV(DS1620_RST);		// RST to 0
  	if(J) I |= 0xff << 8;

  	return I;
}

// *** receiving a byte from the 1620 sensor
uint8_t DS1620_rbyte(void)
{
  	uint8_t I, J = 0;

  	DS1620_DDR &= ~_BV(DS1620_DQ);			// setting the DQ port to input
  	for( I = 1; I ; I <<= 1) {
    		DS1620_PORTOUT &= ~_BV(DS1620_CLK);	//setting CLK to 0
   		 _delay_us(1);				//delay for the time of setting the level
	 	if(DS1620_PORTIN & _BV(DS1620_DQ)) 
			J += I;				//reading the next bit
    		DS1620_PORTOUT |= _BV(DS1620_CLK);	//installing CLK in 1
    		_delay_us(1);				//delay for the time of setting the level
   	}

  	DS1620_DDR |= _BV(DS1620_DQ);			//setting the DQ port to output
  	return J;
}

