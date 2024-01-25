/* CPU frequency */
#define F_CPU 1000000UL

/*********************************************
*
** purpose of the temperature sensor port DS1620
*
**********************************************/

#define DS1620_PORT B
#define DS1620_RST PORT0
#define DS1620_CLK PORT1
#define DS1620_DQ  PORT2

/********************************
*
** sensor port assignment SHT3x
*
*********************************/

#define SHT3x_PORT A
#define SHT3x_VDD PORTA3
#define SHT3x_SDA PORTA5
#define SHT3x_SCL PORTA4
