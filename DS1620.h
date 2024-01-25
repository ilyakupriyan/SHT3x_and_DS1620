
#define RST0	DS1620_PORTOUT &= ~_BV(DS1620_RST);
#define RST1	DS1620_PORTOUT |= _BV(DS1620_RST);
#define CLK0	DS1620_PORTOUT &= ~_BV(DS1620_CLK);	//setting CLK to 0
#define CLK1	DS1620_PORTOUT |= _BV(DS1620_CLK);	//setting CLK in 1

#define RTEMP_	0xAA
#define RCONT_	0xA0
#define RSLOP_	0xA9

//initiating the DS1620 temperature sensor
void DS1620_ini (void);

//transfer of a byte of the exchange protocol
void snd_prot(uint8_t);

//reading the status byte from the temperature sensor
uint8_t DS1620_rstat(void);

//clock output
void DS1620_clk (void);

//loading the configuration register
void DS1620_wconfig(uint8_t);

//starting the temperature conversion
void DS1620_start(void);

//receiving a byte from the 1620 sensor
uint8_t DS1620_rbyte(void);

//reading the temperature value
int16_t DS1620_rtemp(void);

//reading the value of the nreg register data
int16_t DS1620_rdata(uint8_t nreg);

//disconnecting the DS1620 sensor port
void DS1620_off (void);




