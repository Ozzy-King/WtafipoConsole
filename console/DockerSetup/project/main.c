#define DEBUG 1
#include "pico/stdlib.h"

#include "boardLED.h"
#include "SpiHelper.h"
#include "screen.h"

#include "DPRAMmemory.h"

uint32_t fast_rand(uint32_t seed) {
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}


uint8_t testSprite[10][10] = {
    {10,80,80,80,80,80,80,80,80,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,0,0,0,0,0,0,0,0,10},
    {10,60,60,60,60,60,60,60,60,10}
};

uint8_t test2Sprite[10][10] = {
    {240,240,94,94,94,94,94,94,240,240},
    {240,94,94,94,94,94,94,94,94,240},
    {94,94,94,94,94,94,94,94,94,94},
    {94,94,94,94,94,94,94,94,94,94},
    {94,94,94,94,94,94,94,94,94,94},
    {94,94,94,94,94,94,94,94,94,94},
    {94,94,94,94,94,94,94,94,94,94},
    {94,94,94,94,94,94,94,94,94,94},
    {240,94,94,94,94,94,94,94,94,240},
    {240,240,94,94,94,94,94,94,240,240}
};
uint8_t test3Sprite[10][10] = {
    {240,240,112,112,112,112,112,112,240,240},
    {240,112,112,112,112,112,112,112,112,240},
    {112,112,112,112,112,112,112,112,112,112},
    {112,112,112,112,112,112,112,112,112,112},
    {112,112,112,112,112,112,112,112,112,112},
    {112,112,112,112,112,112,112,112,112,112},
    {112,112,112,112,112,112,112,112,112,112},
    {112,112,112,112,112,112,112,112,112,112},
    {240,112,112,112,112,112,112,112,112,240},
    {240,240,112,112,112,112,112,112,240,240}
};

struct _ball{
    int8_t dx;
    int8_t dy;
    uint8_t i;
};

uint8_t ballUpdate(struct _ball* b){
    uint8_t t = 0;
    if(DYNAMIC_SPRITE_ACCESS(b->i).x >= SCREEN_WIDTH-GRID_WIDTH || DYNAMIC_SPRITE_ACCESS(b->i).x <= 0){
        b->dx = -b->dx; t = 1;
    }
    if(DYNAMIC_SPRITE_ACCESS(b->i).y >= SCREEN_HEIGHT-GRID_HEIGHT || DYNAMIC_SPRITE_ACCESS(b->i).y <= 0){
        b->dy = -b->dy; t = 1;
    }
    return t;
}

#define BLIST_LEN 1
void main(){
    int currentRow = 0;
    int swap = 1;


    //setup
    initSpi0();
	initScreenPins();
    memorySetup();
    InitScreen();

    uint8_t* str = "  Hallo End??";

    for(int y = 0; y < TTY_HEIGHT; y++)
        for(int x = 0; x < TTY_WIDTH; x++)
            SCREEN_BACKGROUND_ACCESS(x ,y) = &testSprite[0][0];


    struct _ball bList[BLIST_LEN];
    for(int i = 0 ;i < BLIST_LEN; i++){
        bList[i].dx = fast_rand(i*5)%2 == 0? 3 : -3 ;
        bList[i].dy = fast_rand(i*6)%2 == 0? 3 : -3 ;
        bList[i].i = i;
        DYNAMIC_SPRITE_ACCESS(i).sprite = &test2Sprite[0][0]; //replace  
        DYNAMIC_SPRITE_ACCESS(i).x = fast_rand(i*500)%(320-10); //replace 
        DYNAMIC_SPRITE_ACCESS(i).y = fast_rand(i*40)%(170-10); //replace 
    }
    DYNAMIC_SPRITE_ACCESS(0).sprite = &test3Sprite[0][0]; //replace  

    printString(str, 0, 0);
    printNumber(DYNAMIC_SPRITE_ACCESS(0).x, 1, 0);
    printNumber(DYNAMIC_SPRITE_ACCESS(0).y, 2, 0);
    spriteListUpdate();
    printNumber(DYNAMIC_SPRITE_LIST_START, 3, 0);
    //newDraw();


    //drawTTY();

    while(1){
        
        printNumber(DYNAMIC_SPRITE_ACCESS(0).x, 1, 0);
        printNumber(DYNAMIC_SPRITE_ACCESS(0).y, 2, 0);
        for(uint i = 0; i < BLIST_LEN; i++){
            DYNAMIC_SPRITE_ACCESS(i).y += bList[i].dy;//increment
            DYNAMIC_SPRITE_ACCESS(i).x += bList[i].dx;//increment
            if(ballUpdate(&bList[i])){
                DYNAMIC_SPRITE_ACCESS(i).y += bList[i].dy;//increment
                DYNAMIC_SPRITE_ACCESS(i).x += bList[i].dx;//increment
            }
        }

        newerDraw();
        //sleep_ms(100);
    }
}