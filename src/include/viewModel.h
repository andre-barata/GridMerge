#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "common.h"

enum direction { horizontal, vertical};
typedef struct _Layout {
    Uint16 w; char wUnit[3]; // "px" or "%"
    Uint16 h; char hUnit[3];
    SDL_Color* bgColor;
    enum direction stack; 
    int childCount; struct _Layout *childs;
} Layout;

Layout layout = { 
    .stack = vertical,
    .childCount = 2, .childs = (Layout[]){
        {
            .w = 100,"%", .h = 29,"px", 
            .bgColor = &gray4,
        }, 
        {
            .stack = horizontal,
            .childCount = 3, .childs = (Layout[]){
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


typedef struct _ViewModel {
    Uint16 x, y, w, h;
    SDL_Color* bgColor;
    enum direction stack; 
    struct _ViewModel* childs;
    char* id;
    bool resizable;
    char* innerText;
    //SDL_Image image;
    enum {left, center, right} align;
    enum {top, middle, bottom} vAlign;
    Uint16 paddingLeft, paddingTop;
} ViewModel;

ViewModel viewModel = {};

bool loadModel(Layout* parentDefinition, ViewModel* parentModel, int x, int y, int parentWidth, int parentHeight) {
    int w = parentWidth, h = parentHeight;

    arrsetcap(parentModel->childs, parentDefinition->childCount);
    for (int i = 0; i < parentDefinition->childCount; i++) {
        Layout* definition = &parentDefinition->childs[i];
        ViewModel model = {};

        // if the layout node has no dimensions, assume 100% of the available space
        if (definition->w > 0 && definition->h > 0) {
            if (strcmp(definition->wUnit, "px") == 0 ) w = definition->w;
            else if (strcmp(definition->wUnit, "%") == 0 ) w = parentWidth * definition->w / 100;
            else return false;
            if (strcmp(definition->hUnit, "px") == 0 ) h = definition->h;
            else if (strcmp(definition->hUnit, "%") == 0 ) h = parentHeight * definition->h / 100;
            else return false;

            if (w > 0 && h > 0) {
                model.x = x; model.y = y; model.h = h; model.w = w;
                model.bgColor = definition->bgColor;
            }
        }

        //recursivelly navigate to child nodes
        if (!loadModel(definition, &model, x, y, w, h)) return false;
        arrput(parentModel->childs, model);
        
        // adjust positioning for the next node
        if (parentDefinition->stack == vertical) { y += h; h = parentHeight - h; }
        if (parentDefinition->stack == horizontal) { x += w; w = parentWidth - w; }
    }
    return true;
}


#endif