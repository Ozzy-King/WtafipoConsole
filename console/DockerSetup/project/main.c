#define DEBUG 1
#include "pico/stdlib.h"

#include "boardLED.h"
#include "SpiHelper.h"
#include "screen.h"

#include "DPRAMmemory.h"

void printNumber(uint64_t num, uint8_t row, uint8_t col){
    uint8_t c= col;
    while(num != 0){
        uint t = num - ((num/10)*10);

        SCREEN_TTY_ACCESS(row, c) = (t+48);

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
    int swap = 1;
    //setup
    initSpi0();
	initScreenPins();
    memorySetup();
    InitScreen();

    uint8_t* str = "!!Hallo End??";

    for(int y = 0; y < 16; y++)
        for(int x = 0; x < 16; x++)
            SCREEN_BACKGROUND_ACCESS(x ,y) = &testSprite[0][0];
    
    printString(str, 0, 0);
    newDraw();


    //drawTTY();

    while(1){
        SCREEN_BACKGROUND_ACCESS(8, currentRow) = &testSprite[0][0]; //replace
        currentRow += swap;//increment
        if(currentRow == 15){
            swap = -1;
        }
        else if(currentRow == 0){
            swap = 1;
        }
        SCREEN_BACKGROUND_ACCESS(8, currentRow) = &test2Sprite[0][0];//add

        setPWMForBackLight(currentRow);
        newDraw();
        sleep_ms(100);
    }
}