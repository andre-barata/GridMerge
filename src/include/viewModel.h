#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "common.h"

enum direction { horizontal, vertical };
typedef struct _Layout {
    Uint16 w; char wUnit[3]; // "px" or "%"
    Uint16 h; char hUnit[3];
    SDL_Color* bgColor;
    enum direction stack; 
    int childCount; struct _Layout *childs;
} Layout;

Layout layout = { 
    .stack = vertical,
    .childCount = 5, .childs = (Layout[]){
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
        }, 
        {
            .w = 100,"%", .h = 18,"px", 
            .bgColor = &white,
        },
        {
            .stack = horizontal,
            .childCount = 3, .childs = (Layout[]){
                {
                    .w = 55,"%", .h = 100,"%", 
                    .bgColor = &gray1,
                },
                {
                    .w = 10,"%", .h = 100,"%", 
                    .bgColor = &gray2,
                },
                {
                    .w = 35,"%", .h = 100,"%", 
                    .bgColor = &gray1,
                }
            }
        },
        {
            .w = 100,"%", .h = 18,"px", 
            .bgColor = &white,
        } 
    }
}; 


typedef struct _ViewModel {
    Uint16 x, y, w, h, sumAbsH, sumAbsW, sumRelH, sumRelW;
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

bool createModel(Layout* parentDefinition, ViewModel* parentModel) {

    // set the size of the dinamic array for the model's child nodes based on the layout's node childs
    arrsetcap(parentModel->childs, parentDefinition->childCount);
    // iterate layout child nodes
    for (int i = 0; i < parentDefinition->childCount; i++) {
        Layout* definition = &parentDefinition->childs[i];
        // instantiate the model nodes
        ViewModel model = {};

        // set the sum of the absolute and relative dimensions of the child nodes
        if (parentDefinition->stack == horizontal) { 
            if (strcmp(definition->wUnit, "px") == 0 ) parentModel->sumAbsW += definition->w;
            else parentModel->sumRelW += (definition->w == 0) ? 100 : definition->w;
        }
        if (parentDefinition->stack == vertical) { 
            if (strcmp(definition->hUnit, "px") == 0 ) parentModel->sumAbsH += definition->h;
            else parentModel->sumRelH += (definition->h == 0) ? 100 : definition->h;
        }

        //recursivelly navigate to child nodes
        if (!createModel(definition, &model)) return false;
        // add new model node to the dynamic array of parent node
        arrput(parentModel->childs, model);
    }
    return true;
}

bool loadModelAttributes(Layout* parentDefinition, ViewModel* parentModel, int x, int y, int parentWidth, int parentHeight) {
    int w = parentWidth, h = parentHeight;

    // iterate layout child nodes
    for (int i = 0; i < parentDefinition->childCount; i++) {
        Layout* definition = &parentDefinition->childs[i];
        // get previously instantiated model node, with computed dimension sums (in createModel())
        ViewModel* model = &parentModel->childs[i];

        // if the layout node has no dimensions, assume 100% of the available space
        if (definition->w == 0) definition->w = 100;
        if (definition->h == 0) definition->h = 100;
        if (parentModel->sumRelW == 0) parentModel->sumRelW = 100;
        if (parentModel->sumRelH == 0) parentModel->sumRelH = 100;


        if (strcmp(definition->wUnit, "px") == 0 ) w = definition->w; 
        else if (strcmp(definition->wUnit, "%") == 0 || definition->wUnit[0] == 0)
            w = ((parentWidth - parentModel->sumAbsW) * definition->w / parentModel->sumRelW);
        else return false;
        if (strcmp(definition->hUnit, "px") == 0 ) h = definition->h; 
        else if (strcmp(definition->hUnit, "%") == 0 || definition->hUnit[0] == 0) 
            h = ((parentHeight - parentModel->sumAbsH) * definition->h / parentModel->sumRelH);
        else return false;

        model->x = x; model->y = y; model->h = h; model->w = w;
        model->bgColor = definition->bgColor;

        //recursivelly navigate to child nodes
        if (!loadModelAttributes(definition, model, x, y, w, h)) return false;

        // adjust positioning for the next node
        if (parentDefinition->stack == vertical) { 
            y += h; 
            h -= parentModel->sumAbsH - h; 
        }
        if (parentDefinition->stack == horizontal) { 
            x += w; 
            w -= parentModel->sumAbsW - w;
        }
    }
    return true;
}

bool loadModel(int x, int y, int parentWidth, int parentHeight) { 
    ViewModel* pModel = &viewModel;
    if (!createModel(&layout, pModel)) return false;
    if (!loadModelAttributes(&layout, pModel, x, y, parentWidth, parentHeight)) return false;
    return true;
}


#endif