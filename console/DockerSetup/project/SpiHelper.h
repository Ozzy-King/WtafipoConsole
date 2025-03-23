#include "hardware/spi.h"
#include "hardware/gpio.h"

#ifdef DEBUG
	#include "boardLED.h"
#endif

//useing SPI0
//gp2 clk  sck
//gp3 mosi tx
//gp4 miso rx

#define spiClk 2
#define spiOut 3
#define spiIn 4

//#define Baudrate 40000000
#define Baudrate 60000000
uint initSpi0(){
	uint ret = 0;
	//initilize required pins
	gpio_init(spiClk); 
	gpio_init(spiOut); 
	gpio_init(spiIn); 

	gpio_set_dir(spiClk, GPIO_OUT); 
    gpio_set_dir(spiOut, GPIO_OUT); 
    gpio_set_dir(spiIn, GPIO_IN); 
	
	//gpio_set_drive_strength(spiClk, GPIO_DRIVE_STRENGTH_12MA);

	//init and set baudrate of spi
#ifdef DEBUG
	initBoardLED();//if debug turn on led it succesful
	ret = spi_init(spi0, Baudrate);
	if(ret >= Baudrate){
		showLED();
	}
#else
	spi_init(spi0, Baudrate); //sets the master by default
#endif
	//setup spi format 
	spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);//can set 16bits with spi_write16
	
	//set the pin functions for spi funcion
	gpio_set_function(spiClk, GPIO_FUNC_SPI);
	gpio_set_function(spiOut, GPIO_FUNC_SPI);
	gpio_set_function(spiIn, GPIO_FUNC_SPI);
	return ret;
}