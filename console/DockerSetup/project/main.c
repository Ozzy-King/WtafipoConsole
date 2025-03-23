#define DEBUG 1
#include "pico/stdlib.h"

#include "boardLED.h"
#include "SpiHelper.h"
#include "screen.h"

#include "DPRAMmemory.h"




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

void main(){
    int currentRow = 0;
    int swap = 3;
    //setup
    initSpi0();
	initScreenPins();
    memorySetup();
    InitScreen();

    uint8_t* str = "  Hallo End??";

    for(int y = 0; y < 16; y++)
        for(int x = 0; x < 16; x++)
            SCREEN_BACKGROUND_ACCESS(x ,y) = &testSprite[0][0];

    DYNAMIC_SPRITE_ACCESS(0).sprite = &test2Sprite[0][0]; //replace  
    DYNAMIC_SPRITE_ACCESS(0).x = 10*3; //replace 
    DYNAMIC_SPRITE_ACCESS(0).y = 10*4; //replace 

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
        DYNAMIC_SPRITE_ACCESS(0).y += swap;//increment

        if(DYNAMIC_SPRITE_ACCESS(0).y >= SCREEN_HEIGHT - GRID_HEIGHT){
            swap = -3;
        }
        else if(DYNAMIC_SPRITE_ACCESS(0).y == 10*4){
            swap = 3;
        }

        newDraw();
        //sleep_ms(100);
    }
}