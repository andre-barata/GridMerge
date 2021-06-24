/*
  render.h : handles rendering, views and presentation logic
  Copyright (C) 2021 RangeCode, Lda. <info@rangecode.com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#ifndef RENDER_H
#define RENDER_H

#include <string.h>
#include <SDL.h>
#include "../common.h"
#include "../model/viewModel.h"


// proto functions
bool renderLayoutRect(SDL_Renderer* renderer, ViewModel* model, bool hover);
bool renderLayoutRects(SDL_Renderer* renderer, ViewModel* model);
void clearHover(SDL_Renderer* renderer);

bool renderLayout(SDL_Renderer* renderer, ViewModel* model, int x, int y, int windowWidth, int windowHeight) {
    if (!loadModel(model, x, y, windowWidth, windowHeight)) return false;
    if (!renderLayoutRects(renderer, model)) return false;
    SDL_RenderPresent(renderer);
    return true;
}
bool renderUpdatedLayout(SDL_Renderer* renderer, ViewModel* model, int x, int y, int windowWidth, int windowHeight) {
    if (!computeModel(model, x, y, windowWidth, windowHeight)) return false;
    if (!renderLayoutRects(renderer, model)) return false;
    SDL_RenderPresent(renderer);
    return true;
}

bool renderLayoutRects(SDL_Renderer* renderer, ViewModel* model) {
    for (int i = 0; i < arrlen(model->childs); i++) {
        ViewModel* child = &model->childs[i];
        if (!renderLayoutRect(renderer, child, false))
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to render layout rect at X:%d Y:%d\n", child->x, child->y);
        // recursivelly navigate to the child node
        if (!renderLayoutRects(renderer, child)) return false;
    }
    return true;
}

bool renderLayoutRect(SDL_Renderer* renderer, ViewModel* model, bool hover) {
    if (model == NULL) return false;
    if (model->absW > 0 && model->absH > 0) {
        // draw background color
        if (hover && model->bgHover != NULL) {
            if (SDL_SetRenderDrawColor(renderer, model->bgHover->r, model->bgHover->g, model->bgHover->b, 255) != 0) return false;
        } else if (model->bgColor != NULL) 
            if (SDL_SetRenderDrawColor(renderer, model->bgColor->r, model->bgColor->g, model->bgColor->b, 255) != 0) return false;
        if (SDL_RenderFillRect(renderer, &(SDL_Rect){model->x, model->y, model->absW, model->absH}) != 0) return false;
        
        if (model->imageId != NULL) {
            // Render alligned Image in the rectangle
            int x, y;
            Sprite s = shget(spriteHashSet, model->imageId);
            if (model->align == alignLeft) x = model->x;
            else if (model->align == alignCenter) x = model->x + (model->absW / 2) - (s.parentSet->spriteW / 2); 
            else if (model->align == alignRight) x = model->x +  model->absW - s.parentSet->spriteW; 
            if (model->vAlign == valignMiddle) y = model->y + (model->absH / 2) - (s.parentSet->spriteH / 2);
            else if (model->vAlign == valignTop) y = model->y; 
            else if (model->vAlign == valignBottom) x = model->y + model->absH - s.parentSet->spriteH; 
            if (SDL_RenderCopy(renderer, s.parentSet->texture, &(SDL_Rect){s.x, s.y, s.parentSet->spriteW, s.parentSet->spriteH}, 
                &(SDL_Rect){x, y, s.parentSet->spriteW, s.parentSet->spriteH}) != 0) return false;
        }

        if (model->innerText != NULL) {
            // Render alligned Inner Text in the rectangle
            int x, y, textWidth, textHeight, calcWidth, glyphs;
            getTextDimensions(model->innerText, &textWidth, &textHeight, 12);
            if (model->align == alignLeft) x = model->x;
            else if (model->align == alignCenter) x = model->x + (model->absW / 2) - (textWidth / 2); 
            else if (model->align == alignRight) x = model->x +  model->absW - textWidth; 
            if (model->vAlign == valignMiddle) y = model->y + (model->absH / 2) - (textHeight / 2);
            else if (model->vAlign == valignTop) y = model->y; 
            else if (model->vAlign == valignBottom) x = model->y + model->absH - textHeight; 
            if (!drawText(renderer, model->innerText, x, y, model->absW, model->absH, lightGray, *model->bgColor, 12)) {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to render text '%s' at X:%d Y:%d\n", model->innerText, x, y);
                return false;
            }
        }
    }
    return true;
}

// find a layout rectangle in a binary space partition fashion using XY coords
ViewModel* findRectByXY(ViewModel* model, Uint32 x, Uint32 y) {
    ViewModel* pChild;
    if (model->stack == dirVertical) {
        for (int i = 0; i < arrlen(model->childs); i++) {
            pChild = &model->childs[i];
            if (y <= pChild->y2) { // navigates inside the first node with the Y coord or higher - assumes childs sorted by coords
                if (pChild->childs == NULL) return pChild;
                return findRectByXY(pChild, x, y);
            }
        }
    } else {
        for (int i = 0; i < arrlen(model->childs); i++) {
            pChild = &model->childs[i];
            if (x <= pChild->x2) { // navigates inside the first node with the X coord or higher - assumes childs sorted by coords
                if (pChild->childs == NULL) return pChild;
                return findRectByXY(pChild, x, y);
            }
        }
    }
    return NULL;
}

ViewModel* hoverRect = NULL;
void renderHover(SDL_Renderer* renderer, Uint32 x, Uint32 y) {
    ViewModel* rect = findRectByXY(&layout, x, y);
    if (rect == NULL) return;
    // clear any previous hover
    if (rect != hoverRect) clearHover(renderer);
    // skip if rectangle doesn't use hover
    if (rect->bgHover == NULL) return;
    // skip if hover bit is already on
    if ((rect->state & hover) == hover ) return;
    // render rect
    if (!renderLayoutRect(renderer, rect, true)) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to render hover layout rect at X:%d Y:%d\n", x, y);
        return;
    }
    hoverRect = rect;
    SDL_RenderPresent(renderer);
    // set hover state bit
    rect->state |= hover;
}

void clearHover(SDL_Renderer* renderer) {
    if (hoverRect == NULL) return;
    // skip if hover bit is already off
    if ((hoverRect->state & hover) != hover ) return;
    // render clear rect
    if (!renderLayoutRect(renderer, hoverRect, false)) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to clear hover layout rect at X:%d Y:%d\n", hoverRect->x, hoverRect->y);
        return;
    }
    SDL_RenderPresent(renderer);
    // clear hover state bit
    hoverRect->state &= ~hover;
}



#endif

