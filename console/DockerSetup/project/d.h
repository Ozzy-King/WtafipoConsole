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
	return NULL;
}


void newer(){
	uint8_t cmd = RAMWR;

	uint8_t* ch;
	uint8_t* backgroundSprite;
	uint8_t fully;
	uint8_t alongX;
	uint8_t colIndex = 240;

    gpio_put(CS_screen, 0); // Select the screen
    gpio_put(DC_screen, command_sel);
    spi_write_blocking(spi0, &cmd, 1); // Send command
    gpio_put(DC_screen, data_sel);

	for(int gh = 0; gh < TTY_HEIGHT; gh++){//looks the gridheight
		for(int celly = 0; celly < GRID_HEIGHT; celly++){
			fully = gh*GRID_HEIGHT+celly;

			for(int gw = 0; gw < TTY_WIDTH; gw++){
				alongX = gw*GRID_WIDTH;
				//get the tty character
				ch = character[ SCREEN_TTY_ACCESS(gw, gh) ];//get char from tty and point at it
				//get the background
				backgroundSprite = SCREEN_BACKGROUND_ACCESS(gw, gh);
			
				for(int cellx = 0; cellx < GRID_WIDTH; cellx++){
					//get a sprite
					struct _sprite* dynamicSprite = canSpriteBeDrawn(alongX+cellx, fully);//check to see if sprite can be drawn, returns index else -1
					if(ch[targety] & targetx){//if the charater is bigger than 0
						colIndex = 255;
					}
					else if(dynamicSprite != NULL){
						colIndex = getSpritesColour(x, y, dynamicSprite);
					}
					else if(backgroundSprite != NULL){//if sprite is set access  if not keep black
						colIndex = SPRITE_ACCESS( backgroundSprite, xmod, ymod );
					}else {
						colIndex = 240;
					}
					spi_write_blocking(spi0, (uint8_t*)&COLOUR_ACCESS(colIndex), 2);
				}
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