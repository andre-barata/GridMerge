#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "constants.h"

enum direction { horizontal, vertical};
typedef struct LayRect {
    int w; char wUnit[3]; // "px" or "%"
    int h; char hUnit[3];
    SDL_Color* bgColor;
    enum direction stack; struct LayRect* childs;
} layRect;

/// Layout definition
layRect layout = { 
    .w = 100,"%", .h = 100,"%", 
    .stack = vertical,
    .childs = (layRect[]){
        {
            .w = 100,"%", .h = 24,"px", 
            .bgColor = &gray4,
        }, 
        {
            .stack = horizontal,
            .childs = (layRect[]){
                {
                    .w = 45,"%", .h = 100,"%", 
                    .bgColor = &gray1,
                },
                {
                    .w = 10,"%", .h = 100,"%", 
                    .bgColor = &gray2,
                },
                {
                    .w = 45,"%", .h = 100,"%", 
                    .bgColor = &gray1,
                }
            }
        }
    }
}; 

#endif