/*************************************************************
*
*** драйвер датчика температуры и влажности SHT3x (FOST02)
*
**************************************************************/

// *** команды управления шиной
#define H_SCL() { DDRA &= ~(1 << SHT3x_SCL); PORTA |= (1 << SHT3x_SCL);}					//шина SCL в лог. 1
#define L_SCL() { DDRA |= (1 << SHT3x_SCL); PORTA &= ~(1 << SHT3x_SCL);}					//шина SCL в лог. 0
#define H_SDA() { DDRA &= ~(1 << SHT3x_SDA); PORTA |= (1 << SHT3x_SDA);}					//шина SDA в лог. 1
#define L_SDA() { DDRA |= (1 << SHT3x_SDA); PORTA &= ~(1 << SHT3x_SDA);}					//шина SDA в лог. 0

#define DELAY() _delay_us(5);									//delay 5мкс

// *** setting the sensor address for the bus I2C (Адрес 0х45 - high level on ADDR (SHT3x_ADDR_LOGIC_HIGH)
// 												 Адрес 0х44 - low level on ADDR) 
#ifdef SHT3x_ADDR_LOGIC_HIGH
#define ADDR_SHT3X 0x45
#else
#define ADDR_SHT3X 0x44
#endif

// *** Sensor command SHT3x

//All measurements command return T (CRC) and HM (CRC). Clock stretching is disabled
#define SHT3X_CMD_MEASURE_HPM 0x2400		//Measure T and HM with high repeatability (max. 15 ms)
#define SHT3X_CMD_MEASURE_MPM 0x240B		//Mesuare T and HM with medium repeatability (max. 6 ms)
#define SHT3X_CMD_MEASURE_LPM 0x2416		//Mesuare T and HM with low repeatability (max. 4.5 ms)

#define PERIODIC_MODE	0x2126
#define FETCH_DATA		0xE000
#define BREAK			0x3093
#define GENERAL_CALL	0x0006

#define RSR	0xF32D			//Read Status Register
#define CLR	0x3041			//Clear Status Register
#define SR	0x30A2			//Soft reset, resets the interface, clears the status register to default values. Wait minimum
//11 ms before next command

// *** Initialization HST3x
void SHT3x_ini (void);

// *** Turning off HST3x
void SHT3x_off (void);

// *** Measuring temperature and relative humanity (MSB of 2 bytes - Temp, LSB of 2 bytes - RH), return -1 on error
int32_t meas_th(int16_t COMMAND);

// *** resetting the sensor interface
void conres(void);

// ***  Status Register Read
int16_t srrd(void);

// *** Soft reset, resets the interface, clears the
// status register to default values. Wait minimum
// 11 ms before next command
void softreset (void);

//I2C general call for SHT3x
void SHT3x_gen_call(void);
