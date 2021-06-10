#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <SDL.h>
#include "common.h"

enum direction   { dirHorizontal, dirVertical };
enum alignement  { alignLeft, alignCenter, alignRight };
enum vAlignement { valignMiddle, valignTop, valignBottom };
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
    void (*onClick)(Uint32 x, Uint32 y, struct _ViewModel*); 
    Uint16 paddingLeft, paddingTop;
    // for internal calculations:
    Uint16 x, y, x2, y2, absW, absH, sumAbsH, sumAbsW, sumRelH, sumRelW;
    enum { none = 0, hover = 1 << 0 } state;
} ViewModel;

#include "events.h"

ViewModel layout = { 
    .stack = dirVertical,
    .initialChildCount = 3, .initialChilds = (ViewModel[]){
        {
            .h = 30,"px",
            .stack = dirHorizontal,
            .initialChildCount = 5, .initialChilds = (ViewModel[]){ // window header
                {
                    .w = 60,"px", //TODO: implement menus
                    .innerText = "File...", .align = alignCenter,
                    .bgColor = &gray4
                },
                {
                    .bgColor = &gray4,
                    .innerText = "GridMerge v0.1", .align = alignCenter,
                    .dragsWindow = true
                },
                {
                    .w = 46,"px",
                    .imageId = "minimize-light", .align = alignCenter,
                    .bgColor = &gray4,
                    .bgHover = &gray6,
                    .onClick = &onClickMinimize,
                },
                {
                    .w = 46,"px",
                    .imageId = "restore-light", .align = alignCenter,
                    .bgColor = &gray4,
                    .bgHover = &gray6,
                    .onClick = &onClickRestoreOrMaximize,
                },
                {
                    .w = 46,"px",
                    .imageId = "close-light", .align = alignCenter,
                    .bgColor = &gray4,
                    .bgHover = &red1,
                    .onClick = &onClickClose,
                }
            }
        }, 
        {
            .stack = dirHorizontal,
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

#define isUnitPx(x) (x[0] == 'p' && x[1] == 'x' && x[2] == 0)
#define isUnitPcnt(x) (x[0] == '%' && x[1] == 0)

bool createModel(ViewModel* parentNode) {
    // set the size of the dinamic array for the model's child nodes based on the layout's node childs
    arrsetcap(parentNode->childs, parentNode->initialChildCount);
    // iterate initial layout child nodes
    for (int i = 0; i < parentNode->initialChildCount; i++) {
        ViewModel node = parentNode->initialChilds[i];
        // recursivelly navigate to child nodes
        if (!createModel(&node)) return false;
        // add new model node to the dynamic array of parent node
        arrput(parentNode->childs, node);
    }
    // cleanup initial layout definition nodes (now using dynamic version (layout->childs))
    parentNode->initialChilds = NULL;
    return true;
}
bool calcModelSums(ViewModel* parentNode) {
    // iterate layout child nodes
    parentNode->sumAbsW = parentNode->sumAbsH = 0;
    parentNode->sumRelW = parentNode->sumRelH = 0;
    for (int i = 0; i < arrlen(parentNode->childs); i++) {
        ViewModel* node = &parentNode->childs[i];

        // set the sum of the absolute and relative dimensions of the child nodes
        if (parentNode->stack == dirHorizontal) { 
            if (isUnitPx(node->wUnit)) parentNode->sumAbsW += node->w;
            else parentNode->sumRelW += (node->w == 0) ? 100 : node->w;
        }
        if (parentNode->stack == dirVertical) { 
            if (isUnitPx(node->hUnit)) parentNode->sumAbsH += node->h;
            else parentNode->sumRelH += (node->h == 0) ? 100 : node->h;
        }

        // recursivelly navigate to child nodes
        if (!calcModelSums(node)) return false;
    }
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
        if (isUnitPx(node->wUnit)) w = node->w; 
        else if (isUnitPcnt(node->wUnit) || node->wUnit[0] == 0)
            w = ((parentWidth - parentNode->sumAbsW) * node->w / 
                    ((parentNode->sumRelW == 0) ? 100 : parentNode->sumRelW));
        else return false;
        if (isUnitPx(node->hUnit)) h = node->h; 
        else if (isUnitPcnt(node->hUnit) || node->hUnit[0] == 0) 
            h = ((parentHeight - parentNode->sumAbsH) * node->h / 
                    ((parentNode->sumRelH == 0) ? 100 : parentNode->sumRelH));
        else return false;

        // update dynamic node with calculated dimensions
        node->x = x; node->y = y; node->x2 = x + w; node->y2 = y + h; node->absH = h; node->absW = w;

        //recursivelly navigate to child nodes
        if (!loadModelAttributes(node, x, y, w, h)) return false;

        // adjust positioning for the next node
        if (parentNode->stack == dirVertical) { 
            y += h; 
            h -= parentNode->sumAbsH - h; 
        }
        if (parentNode->stack == dirHorizontal) { 
            x += w; 
            w -= parentNode->sumAbsW - w;
        }
    }
    return true;
}

bool loadModel(ViewModel* model, int x, int y, int parentWidth, int parentHeight) { 
    if (!createModel(model)) return false;
    if (!calcModelSums(model)) return false;
    if (!loadModelAttributes(model, x, y, parentWidth, parentHeight)) return false;
    return true;
}
bool computeModel(ViewModel* model, int x, int y, int parentWidth, int parentHeight) { 
    if (!calcModelSums(model)) return false;
    if (!loadModelAttributes(model, x, y, parentWidth, parentHeight)) return false;
    return true;
}

#endif