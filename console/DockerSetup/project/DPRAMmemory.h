#ifndef __DPRAM_MEMORY__
#define __DPRAM_MEMORY__

const uint16_t colour_palette[256] = {
    0x6186, 0x79A6, 0x9185, 0xA964, 0xC122, 0xDBED, 0x6184, 0x79A4,
    0x91A3, 0xA9A2, 0xC3EC, 0xDC0B, 0x6183, 0x79C3, 0x91E1, 0xABCA,
    0xC409, 0xDC28, 0x61C2, 0x7A01, 0x9389, 0xABE8, 0xC447, 0xDCA5,
    0x6201, 0x7B27, 0x93A7, 0xAC46, 0xC4C4, 0xDD42, 0x62C6, 0x7B66,
    0x9405, 0xACA4, 0xC562, 0xDEAD, 0x62E4, 0x7BA4, 0x9483, 0xAD62,
    0xC64C, 0xDF0B, 0x5B23, 0x73E3, 0x84A1, 0xA58A, 0xB649, 0xC708,
    0x5322, 0x63E1, 0x84A9, 0x9588, 0x9E47, 0xA705, 0x4321, 0x63E7,
    0x74A7, 0x7D86, 0x7E44, 0x7F02, 0x4B26, 0x53E6, 0x5CA5, 0x5D84,
    0x5E42, 0x9F0D, 0x3B24, 0x43E4, 0x44A3, 0x3D82, 0x7E4C, 0x770B,
    0x2B23, 0x2BE3, 0x24A1, 0x658A, 0x5E49, 0x4F08, 0x1322, 0x0BE1,
    0x4CA9, 0x4589, 0x3E48, 0x2F07, 0x0B22, 0x3BE8, 0x3CA9, 0x3589,
    0x2648, 0x1708, 0x3327, 0x33E8, 0x2CA9, 0x2589, 0x164A, 0x6F13,
    0x2327, 0x23E9, 0x1CAA, 0x158B, 0x6652, 0x5F13, 0x1B28, 0x1BEA,
    0x0CAB, 0x5591, 0x4E53, 0x4715, 0x1329, 0x0BEB, 0x4CB0, 0x4592,
    0x3E55, 0x2F17, 0x0B2A, 0x3BEE, 0x3CB1, 0x3594, 0x2657, 0x171B,
    0x332C, 0x33EF, 0x2C92, 0x2535, 0x15B8, 0x6EBB, 0x22CC, 0x236F,
    0x1BF2, 0x1475, 0x6598, 0x5DFB, 0x1A6C, 0x1AEF, 0x0B32, 0x5475,
    0x4CD8, 0x44FB, 0x120C, 0x0A4F, 0x4B92, 0x43D5, 0x3BD8, 0x2BDB,
    0x096C, 0x3AAF, 0x3AF2, 0x32F5, 0x22D8, 0x127B, 0x31EC, 0x320F,
    0x2A12, 0x21D5, 0x1178, 0x6BFB, 0x216C, 0x216F, 0x1932, 0x10B5,
    0x6338, 0x5ADB, 0x20EC, 0x20CF, 0x1872, 0x62D5, 0x6298, 0x5A3B,
    0x20AC, 0x206F, 0x5A52, 0x6235, 0x61F8, 0x617B, 0x284C, 0x51EF,
    0x59F2, 0x61B5, 0x6958, 0x70BB, 0x498C, 0x598F, 0x6172, 0x7115,
    0x78B8, 0xB39B, 0x494C, 0x592F, 0x70F2, 0x8095, 0xAB38, 0xC2DB,
    0x50EC, 0x68CF, 0x8072, 0xA2D5, 0xBA98, 0xD23B, 0x58AC, 0x786F,
    0x9252, 0xAA35, 0xC1F7, 0xD97A, 0x604B, 0x79EE, 0x91F0, 0xA9B3,
    0xC154, 0xD8B6, 0x618A, 0x798C, 0x916E, 0xA910, 0xC0B0, 0xDB96,
    0x6149, 0x792A, 0x90EB, 0xA88C, 0xC332, 0xDAD3, 0x60E7, 0x78C8,
    0x9068, 0xAACE, 0xC28F, 0xDA2E, 0x60A5, 0x7865, 0x924B, 0xAA2B,
    0xC1EA, 0xD969, 0x6042, 0x79E8, 0x91E8, 0xA9A7, 0xC145, 0xD8A3,
    0x0000, 0x1082, 0x2104, 0x3186, 0x4208, 0x52AA, 0x632C, 0x73AE,
    0x8430, 0x94B2, 0xA554, 0xB5D6, 0xC658, 0xD6DA, 0xE75C, 0xFFFF,
};


struct _sprite{
    uint16_t y;
    uint16_t x;
    uint8_t* sprite;
    struct _sprite* nextSpr;
};

//scary memory things

//used for storing functino pointers
#define FUNCTION_LEN 10
#define FUNCTIONS ((void*)0x50100000)
#define FUNCTION_ACCESS(x) ( *(FUNCTIONS + (x * sizeof(void*))) )


//memory start for tty
#define TTY_WIDTH 32
#define TTY_HEIGHT 17

#define SCREEN_TTY ((uint8_t*)0x50100028)
#define SCREEN_TTY_ACCESS(x, y) (*(SCREEN_TTY + (y * TTY_WIDTH) + x))



//memory start for background sprites
#define BACKGROUND_WIDTH 32
#define BACKGROUND_HEIGHT 17

#define SCREEN_BACKGROUND ((uint8_t**)0x50100248)
#define SCREEN_BACKGROUND_ACCESS(x, y) (*(SCREEN_BACKGROUND + (y * BACKGROUND_WIDTH) + x))

//memory for set colours
#define COLOUR_LEN 256
#define COLOURS ((uint16_t*)0x50100AC8)
#define COLOUR_ACCESS(x) (*(COLOURS + x))

#define DYNAMIC_SPRITE_LEN 68
#define DYNAMIC_SPRITE ((struct _sprite*)0x50100CC8)
#define DYNAMIC_SPRITE_ACCESS(x) ( *(DYNAMIC_SPRITE + x) )

//as a reminder thisis the start of the linked this. the address at this addres is the first sprite
#define DYNAMIC_SPRITE_LIST_START (*((struct _sprite**)0x50100FF8))

#define TEMP_DATA_LEN 8
#define TEMP_DATA ((uint8_t*)0x50100FFC)
#define TEMP_DATA_ACCESS(x) (*(TEMP_DATA + x))
#define TEMP_DATA_ACCESS_AS(type, x) (*(((type*)0x50100FFC) + x))

#define SPRITE_ACCESS(s, x, y) (*(s + (y*10) + x))
//the rest is sprite data which should be 8bytes (4bytes = sprite address, 2*16bytes for x y)

void memorySetup(){

    //set all tty screen and background pointers to 0
    for(uint8_t y = 0; y < TTY_HEIGHT; y++){
		for(uint8_t x = 0; x < TTY_WIDTH; x++){
			SCREEN_TTY_ACCESS(x, y) = 0;
            SCREEN_BACKGROUND_ACCESS(x, y) = NULL;//
		}
	}
	
	//set all dynamic sprites to 0 in x, y and pointer
	for(uint8_t x = 0; x < DYNAMIC_SPRITE_LEN; x++){
		DYNAMIC_SPRITE_ACCESS(x).y = 0;
		DYNAMIC_SPRITE_ACCESS(x).x = 0;
		DYNAMIC_SPRITE_ACCESS(x).sprite = NULL;
        DYNAMIC_SPRITE_ACCESS(x).nextSpr = NULL;
	}
	
    //set colours in the DPRAM
    //reverse byte order so that high byte gets sent first then little byte
    //OG byte order =       Little byte -> high   byte (little endian)
    //required byte order = high   byte -> little byte (big endian)
    for(int i = 0; i < COLOUR_LEN*2; i+=2){
        (*((uint8_t*)COLOURS + i     )) = *((uint8_t*)colour_palette + (i+1));
        (*((uint8_t*)COLOURS + (i+1) )) = *((uint8_t*)colour_palette +  i);
        //COLOUR_ACCESS(i) = colour_palette[i];
    }

}


//reverse this start at bottom of sprite list and work up
//update data sprite list so all sprites are connected in linked list
void spriteListUpdate(){
    DYNAMIC_SPRITE_LIST_START = NULL;
    for(int8_t i = DYNAMIC_SPRITE_LEN-1; i >= 0 ; i--){
        if(DYNAMIC_SPRITE_ACCESS(i).sprite != NULL){ //check if current the sprite is active
            DYNAMIC_SPRITE_ACCESS(i).nextSpr = DYNAMIC_SPRITE_LIST_START;
            DYNAMIC_SPRITE_LIST_START = &DYNAMIC_SPRITE_ACCESS(i);
        }else{
            //DYNAMIC_SPRITE_ACCESS(i).nextSpr = NULL;
        }
    }
}


//reverse this start at bottom of sprite list and work up
//update data sprite list so all sprites are connected in linked list
/*    void spriteListUpdate(){
        TEMP_DATA_ACCESS_AS(struct _sprite*, 0) = NULL;
        for(uint8_t i = 0; i < DYNAMIC_SPRITE_LEN; i++){
            if(DYNAMIC_SPRITE_ACCESS(i).sprite != NULL){ //check if current the sprite is active
                if(TEMP_DATA_ACCESS_AS(struct _sprite*, 0) != NULL){//check if there is a previous sprite
                    (*TEMP_DATA_ACCESS_AS(struct _sprite*, 0)).nextSpr = &DYNAMIC_SPRITE_ACCESS(i); //set the prvious dynamic->next_sprite to this sprites address
                }
                else{//if there is no prvious then this is first and store to find first sprite
                    DYNAMIC_SPRITE_LIST_START = &DYNAMIC_SPRITE_ACCESS(i);
                }
                TEMP_DATA_ACCESS_AS(struct _sprite*, 0) = &DYNAMIC_SPRITE_ACCESS(i);//set this in temp data to access next look.
            }
            else{ //possibly dont need this one
                DYNAMIC_SPRITE_ACCESS(i).nextSpr = NULL;
            }
        }
    }*/


#endif