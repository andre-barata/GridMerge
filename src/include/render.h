#ifndef RENDER_H
#define RENDER_H

#include <string.h>
#include <SDL.h>
#include "common.h"
#include "viewModel.h"

// proto functions
bool renderLayoutRect(layRect* lRect, int x, int y, int parentWidth, int parentHeight);

bool renderLayout(int x, int y, int windowWidth, int windowHeight) {
    return renderLayoutRect(&layout, x, y, windowWidth, windowHeight);
}

// TODO: implement algorithm for calculating the remaining sibling height/width to dinamically size a node
//       add a "*" metric, simbolizing any amount of remaining pixels
bool renderLayoutRect(layRect* lRect, int x, int y, int parentWidth, int parentHeight) {
    int w = parentWidth, h = parentHeight;

    for (int i = 0; i < lRect->childCount; i++) {
        layRect* child = &lRect->childs[i];

        // if the layout node has no dimensions, assume 100% of the available space
        if (child->w != 0 || child->h != 0) {
            if (strcmp(child->wUnit, "px") == 0 ) w = child->w;
            else if (strcmp(child->wUnit, "%") == 0 ) w = parentWidth * child->w / 100;
            else return false;
            if (strcmp(child->hUnit, "px") == 0 ) h = child->h;
            else if (strcmp(child->hUnit, "%") == 0 ) h = parentHeight * child->h / 100;
            else return false;

            // draw the layout rectangle if dimensions are greater than 0
            if (w > 0 && h > 0) {
                SDL_SetRenderDrawColor(windowRenderer, child->bgColor->r, child->bgColor->g, child->bgColor->b, 255);
                SDL_RenderFillRect(windowRenderer, &(SDL_Rect){x, y, w, h});
            }
        }

        // recursivelly navicage to the child node
        if (!renderLayoutRect(child, x, y, w, h)) return false;
        // adjust positioning for the next node
        // TODO: check size of the next nodes, not just the previous ones
        if (lRect->stack == vertical) { y += h; h = parentHeight - h; }
        if (lRect->stack == horizontal) { x += w; w = parentWidth - w; }
    }
    return true;
}

#endif

