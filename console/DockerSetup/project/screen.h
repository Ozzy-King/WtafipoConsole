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
void printNumber(uint32_t num, uint8_t row, uint8_t col){
    uint8_t c= col;
    while(num != 0){
        uint t = num - ((num/10)*10);

        SCREEN_TTY_ACCESS(c, row) = (t+48);

        num /= 10;
        c++;
    }
}
void printString(uint8_t* str, uint8_t row, uint8_t col){
    uint8_t c = 0;
    while(str[c] != '\0'){
        SCREEN_TTY_ACCESS(col, row) = (str[c]);
        c++;
        col++;
    }
}

//\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\NEED TO CHECK ALL THIS WORKS PROPERLY 
//get the the colour at the right position in the sprite
uint8_t getSpritesColour(uint16_t x, uint16_t y, struct _sprite* spr){
	x = x - spr->x;
	y = y - spr->y;
	return SPRITE_ACCESS(spr->sprite, x, y);	
}

////--------------------------------------------------------------this sprite find is really slow--------------------------------------
////--------------------------------------------------------------before the render find all the grids the sprites are in
//finds the first valid sprite. this stops z fighting and order is linear
struct _sprite* canSpriteBeDrawn(uint16_t x, uint16_t y){
	struct _sprite* spr = DYNAMIC_SPRITE_LIST_START;
	while(spr != NULL){
		if (spr->sprite != NULL &&
			y >= spr->y && y < spr->y+GRID_HEIGHT &&
			x >= spr->x && x < spr->x+GRID_WIDTH){
			
			return spr;
		}
		spr = spr->nextSpr;
	}

	// for(uint8_t i = 0; i < DYNAMIC_SPRITE_LEN; i++){//go thruhg e
	// 	struct _sprite* spr = &(DYNAMIC_SPRITE_ACCESS(i));
	// 	if (spr->sprite != NULL &&
	// 		y >= spr->y && y < spr->y+GRID_HEIGHT &&
	// 		x >= spr->x && x < spr->x+GRID_WIDTH){
			
	// 		return i;
	// 	}
	// }
	return NULL;
}

static const uint8_t LUT [10] = {0, 1, 2, 3, 3, 4, 4, 5, 6, 7};
void newerDraw(){
	uint8_t cmd = RAMWR;

	uint8_t* ch;
	uint8_t* backgroundSprite;
	uint8_t fully;
	uint16_t alongX;
	
	uint8_t colIndex = 240;
	
	struct _sprite* dynamicSprite = NULL;
	
	uint8_t targetx;
	uint8_t targety;
    
	gpio_put(CS_screen, 0); // Select the screen
    gpio_put(DC_screen, command_sel);
    spi_write_blocking(spi0, &cmd, 1); // Send command
    gpio_put(DC_screen, data_sel);


	for(uint8_t gh = 0; gh < TTY_HEIGHT; gh++){//looks the gridheight
		for(uint8_t celly = 0; celly < GRID_HEIGHT; celly++){
			fully = gh*GRID_HEIGHT+celly;
			targety = LUT[celly];//get the y target in the tty font character

			for(uint8_t gw = 0; gw < TTY_WIDTH; gw++){
				alongX = gw*GRID_WIDTH;
				//get the tty character
				ch = character[ SCREEN_TTY_ACCESS(gw, gh) ];//get char from tty and point at it
				//get the background
				backgroundSprite = SCREEN_BACKGROUND_ACCESS(gw, gh);
			
				for(uint8_t cellx = 0; cellx < GRID_WIDTH; cellx++){
					//get a sprite
					targetx = 0x80 >> LUT[cellx];
					dynamicSprite = canSpriteBeDrawn(alongX+cellx, fully);//check to see if sprite can be drawn, returns index else -1
					if(ch[targety] & targetx){//if the charater is bigger than 0
						colIndex = 255;
					}
					else if(dynamicSprite != NULL){
						colIndex = getSpritesColour(alongX+cellx, fully, dynamicSprite);
					}
					else if(backgroundSprite != NULL){//if sprite is set access  if not keep black
						colIndex = SPRITE_ACCESS( backgroundSprite, cellx, celly );
					}else {
						colIndex = 240;
					}
					CELLROWCOLOURBUFFER_ACCESS(cellx) = COLOUR_ACCESS(colIndex);
					//spi_write_blocking(spi0, (uint8_t*)&COLOUR_ACCESS(colIndex), 2);
				}
				spi_write_blocking(spi0, (uint8_t*)CELLROWCOLOURBUFFER, 20);
			}
		}
	}
	gpio_put(CS_screen, 1); // deSelect the screen
}

void newDraw(){
	uint8_t cmd = RAMWR;

	uint8_t colIndex = 240;
	uint8_t ymod;
	uint8_t xmod;
	uint8_t ydiv;
	uint8_t xdiv;

	//static uint8_t flipper;
	//flipper = !flipper;

    gpio_put(CS_screen, 0); // Select the screen
    gpio_put(DC_screen, command_sel);
    spi_write_blocking(spi0, &cmd, 1); // Send command
    gpio_put(DC_screen, data_sel);
	for(uint16_t y = 0; y < SCREEN_HEIGHT;y++){
		 ymod = y % GRID_HEIGHT;
		 ydiv = y / GRID_HEIGHT;

		uint8_t targety = ((ymod) * CHARACTER_HEIGHT) / GRID_HEIGHT;//get the y target in the tty font character

		for(uint16_t x = 0; x < SCREEN_WIDTH; x++){
			 colIndex = 240;
			//if(flipper){
				xmod = x % GRID_WIDTH;
				xdiv = x/GRID_WIDTH;
				//target x y for tty character
				uint8_t targetx = 0x80 >> (((xmod) * CHARACTER_WIDTH )/GRID_WIDTH); //get x target in tty font character

				uint8_t* ch = character[ SCREEN_TTY_ACCESS(xdiv, ydiv) ];//get char from tty and point at it

				uint8_t* backgroundSprite = SCREEN_BACKGROUND_ACCESS(xdiv, ydiv);
				struct _sprite* dynamicSprite = canSpriteBeDrawn(x, y);//check to see if sprite can be drawn, returns index else -1

				//uint8_t colIndex = 240;
				
				if(backgroundSprite != NULL){//if sprite is set access  if not keep black
					colIndex = SPRITE_ACCESS( backgroundSprite, xmod, ymod );
				}
				if(dynamicSprite != NULL){
					colIndex = getSpritesColour(x, y, dynamicSprite);
				}
				if(ch[targety] & targetx){//if the charater is bigger than 0
					colIndex = 255;
				}
			//}
			spi_write_blocking(spi0, (uint8_t*)&COLOUR_ACCESS(colIndex), 2);
		}
		//flipper = !flipper;
	}
	gpio_put(CS_screen, 1); // deSelect the screen

}


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

