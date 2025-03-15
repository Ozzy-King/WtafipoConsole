#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "hardware/pwm.h"

#include "characterFont.h"
#include "DPRAMmemory.h"


//spi chip select pin for screen
#define CS_screen 6 //active low
#define DC_screen 7 //1 for data, 0 for command
#define BL_screen 8 //pwm but impilment later
#define RST_screen 9 //active low

#define command_sel 0
#define data_sel 1

#define SCREEN_HEIGHT 170
#define SCREEN_WIDTH 320

#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define TTY_WIDTH 32
#define TTY_HEIGHT 17


//dpram
//the address to tty get place in flash as it shouldnt change the actual data will be stored else where
//uint8_t screen_tty[TTY_HEIGHT][TTY_WIDTH] __attribute__((section(".dpram")));
//volatile uint8_t* screen_tty = (volatile uint8_t *)0x50100000;


//#define CSsdcard 7
//	gpio_init(CSsdcard); 
//	gpio_put(CSsdcard, 1);

void setPWMForBackLight(uint16_t level){
	uint slice_num = pwm_gpio_to_slice_num(BL_screen);
	uint chan_num =  pwm_gpio_to_channel(BL_screen);
    // Set period of 4 cycles (0 to 3 inclusive)
    // Set channel A output high for one cycle before dropping
    pwm_set_chan_level(slice_num, chan_num, level);

}

//initilise pins for chip selecting
void initScreenPins(){
	//initilize required pins
	gpio_init(CS_screen); 
	gpio_init(DC_screen); 
	gpio_init(BL_screen); 
	gpio_init(RST_screen); 
	//set chip select high to be inactive
	gpio_set_dir(CS_screen, GPIO_OUT);
	gpio_set_dir(DC_screen, GPIO_OUT);
	gpio_set_dir(RST_screen, GPIO_OUT);

	//set BL to pwm and set it the 1/2 duty
	gpio_set_function(BL_screen, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(BL_screen);
	pwm_set_wrap(slice_num, 16);
	setPWMForBackLight(8);
    pwm_set_enabled(slice_num, true);

	gpio_put(RST_screen, 1);
	gpio_put(CS_screen, 1);
	gpio_put(DC_screen, command_sel);
	//gpio_put(BL_screen, 1);
}

void sendCommand_Data(uint8_t cmd, uint8_t* data, size_t len){ //send command then data
	gpio_put(CS_screen, 0);//select the screen
	gpio_put(DC_screen, command_sel);
	spi_write_blocking(spi0, &cmd, 1);//send command
	gpio_put(DC_screen, data_sel);
	spi_write_blocking(spi0, data, len); //send data
	gpio_put(CS_screen, 1);
}


//init screen
//colmod
//caset
//raset
//madctl

//sleep out
// display on

//maybe inversion set
#define SLPOUT 0x11 //sleep out
#define DISPON 0x29 //display on
#define SWRESET 0x01 //software reset
#define COLMOD 0x3A //colour mod
#define CASET 0x2A //
#define RASET 0x2B //
#define MADCTL 0x36 //
#define RAMWR 0x2C //ram write
#define INVON 0x21 //display inversion on
#define INVOFF 0x20

void InitScreen(){
	uint8_t RASETDATA[] = {0x00, 0x23, 0x00, 0xCC};//offseted by 34
	uint8_t CASETDATA[] = {0x00, 0x00, 0x01, 0x40};
	uint8_t data = 0x55;
	sendCommand_Data(SWRESET, NULL, 0);
	sleep_ms(150);
	sendCommand_Data(SLPOUT, NULL, 0);
	sleep_ms(150);

	//display inversion
	sendCommand_Data(INVON, NULL, 0);
	sleep_ms(2);

	//set colour mode
	sendCommand_Data(COLMOD, &data, 1);
	sleep_ms(2);

	//set left and right columns
	sendCommand_Data(CASET, (uint8_t*)CASETDATA, 4);
	sleep_ms(2);
	
	sendCommand_Data(RASET, (uint8_t*)RASETDATA, 4);
	sleep_ms(2);

	data = 0b01100000;
	sendCommand_Data(MADCTL, &data, 1);
	sleep_ms(2);
	
	sendCommand_Data(DISPON, NULL, 0);
	sleep_ms(2);
}
// void DrawBackground() {
//     uint8_t cmd = RAMWR;
//     gpio_put(CS_screen, 0); // Select the screen
//     gpio_put(DC_screen, command_sel);
//     spi_write_blocking(spi0, &cmd, 1); // Send command
//     gpio_put(DC_screen, data_sel);

//     for (int j = 0; j < 170; j++) {
//         for (int i = 0; i < 320; i++) {

//             spi_write_blocking(spi0, color, 2);
//         }
//     }

//     gpio_put(CS_screen, 1); // Deselect the screen
// }

void newDraw(){
	uint8_t cmd = RAMWR;

    gpio_put(CS_screen, 0); // Select the screen
    gpio_put(DC_screen, command_sel);
    spi_write_blocking(spi0, &cmd, 1); // Send command
    gpio_put(DC_screen, data_sel);
	for(uint16_t y = 0; y < SCREEN_HEIGHT;y++){
		uint8_t targety = ((y % GRID_HEIGHT) * CHARACTER_HEIGHT) / GRID_HEIGHT;//get the y target in the tty font character

		for(uint16_t x = 0; x < SCREEN_WIDTH; x++){
			//target x y for tty character
			uint8_t targetx = 0x80 >> ((x% GRID_WIDTH) * CHARACTER_WIDTH )/GRID_WIDTH; //get x target in tty font character

			uint8_t* ch = character[ SCREEN_TTY_ACCESS(x/GRID_WIDTH, y/GRID_HEIGHT) ];//get char from tty and point at it

			uint8_t* sprite = SCREEN_BACKGROUND_ACCESS(x/GRID_WIDTH, y/GRID_HEIGHT);
			uint8_t colIndex = 240;
			if(sprite != NULL){//if sprite is set access  if not keep black
				colIndex = SPRITE_ACCESS( sprite, x%GRID_WIDTH, y%GRID_HEIGHT );
			}
			if(ch[targety] & targetx){//if the charater is bigger than 0
				colIndex = 240;
			}
			spi_write_blocking(spi0, (uint8_t*)&COLOUR_ACCESS(colIndex), 2);
			
		}
	}
	gpio_put(CS_screen, 1); // deSelect the screen

}


// void drawTTY(){
// 	uint8_t cmd = RAMWR;
// 	uint8_t colour[2];
//     gpio_put(CS_screen, 0); // Select the screen
//     gpio_put(DC_screen, command_sel);
//     spi_write_blocking(spi0, &cmd, 1); // Send command
//     gpio_put(DC_screen, data_sel);
// 	//screen_tty
// 	for (uint8_t j = 0; j < SCREEN_HEIGHT; j++) {//height 
//         for (uint8_t i = 0; i < SCREEN_WIDTH/GRID_WIDTH; i++) { //width tty
// 			//each square is 10x10
			
// 			uint8_t* ch = character[SCREEN_TTY_ACCESS(j/GRID_HEIGHT,i)];//get char from tty and point at it
// 			uint8_t targety = ((j % GRID_HEIGHT) * CHARACTER_HEIGHT) / GRID_HEIGHT;

// 			for(int x = 0; x < GRID_WIDTH; x++){ //dras just the row
// 				uint8_t targetx = 0x80 >> (x * CHARACTER_WIDTH )/GRID_WIDTH;
// 				if(ch[targety] & targetx){
// 					colour[0] = 0xff; colour[1] = 0xff;
// 				}else{
// 					colour[0] = 0x00; colour[1] = 0x00;
// 				}
// 				spi_write_blocking(spi0, colour, 2);
// 			}
// 			//get with number - which row which column
// 		}
		
// 	}

// }


void DrawScreenSaver()	{
	uint8_t cmd = RAMWR;
	gpio_put(CS_screen, 0); // Select the screen
	gpio_put(DC_screen, command_sel);
	spi_write_blocking(spi0, &cmd, 1); // Send command
	gpio_put(DC_screen, data_sel);

	for (int j = 0; j < 320; j++) {
		for (int i = 0; i < 170; i++) {
			// Calculate gradient components
			uint32_t red   = (0x1f * i) / 170;   // Left (0) → Right (Max Red)
			uint32_t green = (0x3f * j) / 320;   // Top (0) → Bottom (Max Green)
			uint32_t blue  = (red/2)+(green/2); // Right (0) → Left (Max Blue)

			uint8_t color[] = { (red & 0xF8) | (green >> 5), (green << 3) | (blue & 0x1F) };
			spi_write_blocking(spi0, color, 2);
		}
	}

	gpio_put(CS_screen, 1); // Deselect the screen
}

