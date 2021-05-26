#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "common.h"

enum direction { horizontal, vertical};
typedef struct LayRect {
    int w; char wUnit[3]; // "px" or "%"
    int h; char hUnit[3];
    SDL_Color* bgColor;
    enum direction stack; 
    int childCount; struct LayRect *childs;
} layRect;

/// Layout definition
layRect layout = { 
    .stack = vertical,
    .childCount = 3, .childs = (layRect[]){
        {
            .w = 100,"%", .h = 29,"px", 
            .bgColor = &gray4,
        }, 
        {
            .stack = horizontal,
            .childCount = 3, .childs = (layRect[]){
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