#ifndef RENDER_H
#define RENDER_H

#include <string.h>
#include <SDL.h>
#include "common.h"
#include "viewModel.h"

// proto functions
bool renderLayoutRect(ViewModel* model);

bool renderLayout(int x, int y, int windowWidth, int windowHeight) {
    if (!loadModel(&layout, x, y, windowWidth, windowHeight)) return false;
    return renderLayoutRect(&layout);
}

bool renderLayoutRect(ViewModel* model) {
    for (int i = 0; i < arrlen(model->childs); i++) {
        ViewModel* child = &model->childs[i];

        if (child->w > 0 && child->h > 0) {
            if (child->bgColor != NULL) SDL_SetRenderDrawColor(windowRenderer, child->bgColor->r, child->bgColor->g, child->bgColor->b, 255);
            SDL_RenderFillRect(windowRenderer, &(SDL_Rect){child->x, child->y, child->w, child->h});
        }

        // recursivelly navigate to the child node
        if (!renderLayoutRect(child)) return false;
    }
    return true;
}

#endif

