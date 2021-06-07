#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "common.h"

enum direction   { horizontal, vertical };
enum alignement  { left, center, right };
enum vAlignement { middle, top, bottom };
typedef struct _ViewModel {
    Uint16 w; char wUnit[3]; // "px" or "%"
    Uint16 h; char hUnit[3];
    SDL_Color* bgColor;
    SDL_Color* bgHover;
    enum direction stack; 
    int initialChildCount; struct _ViewModel *initialChilds;
    struct _ViewModel *childs;
    char* id;
    char* innerText;
    char* imageId;
    enum alignement align;
    enum vAlignement vAlign;
    // behavior
    bool dragsWindow;
    void (*onClick)(); 
    Uint16 paddingLeft, paddingTop;
    // for internal calculations:
    Uint16 x, y, x2, y2, sumAbsH, sumAbsW, sumRelH, sumRelW;
    enum { none = 0, hover = 1 << 0 } state;
} ViewModel;

ViewModel layout = { 
    .stack = vertical,
    .initialChildCount = 3, .initialChilds = (ViewModel[]){
        {
            .h = 30,"px",
            .stack = horizontal,
            .initialChildCount = 5, .initialChilds = (ViewModel[]){ // window header
                {
                    .w = 60,"px", //TODO: implement menus
                    .innerText = "File...", .align = center,
                    .bgColor = &gray4
                },
                {
                    .bgColor = &gray4,
                    .innerText = "GridMerge v0.1", .align = center,
                    .dragsWindow = true
                },
                {
                    .w = 46,"px",
                    .imageId = "minimize-light", .align = center,
                    .bgColor = &gray4,
                    .bgHover = &gray6,
                },
                {
                    .w = 46,"px",
                    .imageId = "restore-light", .align = center,
                    .bgColor = &gray4,
                    .bgHover = &gray6,
                },
                {
                    .w = 46,"px",
                    .imageId = "close-light", .align = center,
                    .bgColor = &gray4,
                    .bgHover = &gray6,
                }
            }
        }, 
        {
            .stack = horizontal,
            .initialChildCount = 3, .initialChilds = (ViewModel[]){
                {
                    .w = 45,"%",
                    .bgColor = &gray1
                },
                {
                    .w = 10,"%",
                    .bgColor = &gray2
                },
                {
                    .w = 45,"%",
                    .bgColor = &gray1
                }
            }
        }, 
        {
            .h = 22,"px", 
            .bgColor = &blue1
        } 
    }
}; 


bool createModel(ViewModel* parentNode) {
    // set the size of the dinamic array for the model's child nodes based on the layout's node childs
    arrsetcap(parentNode->childs, parentNode->initialChildCount);
    // iterate initial layout child nodes
    for (int i = 0; i < parentNode->initialChildCount; i++) {
        ViewModel node = parentNode->initialChilds[i];

        // set the sum of the absolute and relative dimensions of the child nodes
        if (parentNode->stack == horizontal) { 
            if (strcmp(node.wUnit, "px") == 0 ) parentNode->sumAbsW += node.w;
            else parentNode->sumRelW += (node.w == 0) ? 100 : node.w;
        }
        if (parentNode->stack == vertical) { 
            if (strcmp(node.hUnit, "px") == 0 ) parentNode->sumAbsH += node.h;
            else parentNode->sumRelH += (node.h == 0) ? 100 : node.h;
        }

        // recursivelly navigate to child nodes
        if (!createModel(&node)) return false;
        // add new model node to the dynamic array of parent node
        arrput(parentNode->childs, node);
    }
    // cleanup initial layout definition nodes (now using dynamic version (layout->childs))
    parentNode->initialChilds = NULL;
    return true;
}

bool loadModelAttributes(ViewModel* parentNode, int x, int y, int parentWidth, int parentHeight) {
    int w = parentWidth, h = parentHeight;

    // iterate dynamic child nodes
    for (int i = 0; i < parentNode->initialChildCount; i++) {
        ViewModel* node = &parentNode->childs[i];

        // if the layout node has no dimensions, assume 100% of the available space
        if (node->w == 0) node->w = 100;
        if (node->h == 0) node->h = 100;

        // assign either an absolute size in pizels or a relative size in %.
        // if the percentages don't add up to 100, use the sum of the values as the max percent value
        if (strcmp(node->wUnit, "px") == 0 ) w = node->w; 
        else if (strcmp(node->wUnit, "%") == 0 || node->wUnit[0] == 0)
            w = ((parentWidth - parentNode->sumAbsW) * node->w / 
                    ((parentNode->sumRelW == 0) ? 100 : parentNode->sumRelW));
        else return false;
        if (strcmp(node->hUnit, "px") == 0 ) h = node->h; 
        else if (strcmp(node->hUnit, "%") == 0 || node->hUnit[0] == 0) 
            h = ((parentHeight - parentNode->sumAbsH) * node->h / 
                    ((parentNode->sumRelH == 0) ? 100 : parentNode->sumRelH));
        else return false;

        // update dynamic node with calculated dimensions
        node->x = x; node->y = y; node->x2 = x + w; node->y2 = y + h; node->h = h; node->w = w;

        //recursivelly navigate to child nodes
        if (!loadModelAttributes(node, x, y, w, h)) return false;

        // adjust positioning for the next node
        if (parentNode->stack == vertical) { 
            y += h; 
            h -= parentNode->sumAbsH - h; 
        }
        if (parentNode->stack == horizontal) { 
            x += w; 
            w -= parentNode->sumAbsW - w;
        }
    }
    return true;
}

bool loadModel(ViewModel* model, int x, int y, int parentWidth, int parentHeight) { 
    if (!createModel(model)) return false;
    if (!loadModelAttributes(model, x, y, parentWidth, parentHeight)) return false;
    return true;
}


#endif