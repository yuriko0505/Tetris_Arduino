#include <Arduino.h>
#include "components.h"

uint8_t field[FIELD_WIDTH][FIELD_HEIGHT] = {0};

uint8_t block[7][BLOCK_WIDTH][BLOCK_HEIGHT] = {
    {  // BLOCK_TYPE_I
        {0,0,0,0},
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_O
        {0,0,0,0},
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_S
        {0,0,0,0},
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_Z
        {0,0,0,0},
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_L
        {0,0,0,0},
        {1,1,1,0},
        {1,0,0,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_J
        {0,0,0,0},
        {1,1,1,0},
        {0,0,1,0},
        {0,0,0,0}
    },
    {  // BLOCK_TYPE_T
        {0,0,0,0},
        {1,1,1,0},
        {0,1,0,0},
        {0,0,0,0}
    }
};