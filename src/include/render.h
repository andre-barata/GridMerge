#ifndef RENDER_H
#define RENDER_H

#include <string.h>
#include <SDL.h>
#include "common.h"
#include "viewModel.h"


// proto functions
bool renderLayoutRect(SDL_Renderer* renderer, ViewModel* model);

bool renderLayout(SDL_Renderer* renderer, int x, int y, int windowWidth, int windowHeight) {
    if (!loadModel(&layout, x, y, windowWidth, windowHeight)) return false;
    return renderLayoutRect(renderer, &layout);
}

bool renderLayoutRect(SDL_Renderer* renderer, ViewModel* model) {
    for (int i = 0; i < arrlen(model->childs); i++) {
        ViewModel* child = &model->childs[i];

        if (child->w > 0 && child->h > 0) {
            if (child->bgColor != NULL) SDL_SetRenderDrawColor(renderer, child->bgColor->r, child->bgColor->g, child->bgColor->b, 255);
            SDL_RenderFillRect(renderer, &(SDL_Rect){child->x, child->y, child->w, child->h});
            
            if (child->imageId != NULL) {
                int x, y;
                Sprite s = shget(spriteHashSet, child->imageId);
                if (child->align == left) x = child->x;
                else if (child->align == center) x = child->x + (child->w / 2) - (s.parentSet->spriteW / 2); 
                else if (child->align == right) x = child->x +  child->w - s.parentSet->spriteW; 
                if (child->vAlign == middle) y = child->y + (child->h / 2) - (s.parentSet->spriteH / 2);
                else if (child->vAlign == top) y = child->y; 
                else if (child->vAlign == bottom) x = child->y + child->h - s.parentSet->spriteH; 
                SDL_RenderCopy(renderer, s.parentSet->texture, &(SDL_Rect){s.x, s.y, s.parentSet->spriteW, s.parentSet->spriteH}, 
                    &(SDL_Rect){x, y, s.parentSet->spriteW, s.parentSet->spriteH});
            }

            if (child->innerText != NULL) {
                int x, y, textWidth, textHeight, calcWidth, glyphs;
                TTF_MeasureUTF8(mainFont, child->innerText, child->w, &textWidth, &glyphs);
                textHeight = TTF_FontHeight(mainFont);
                if (child->align == left) x = child->x;
                else if (child->align == center) x = child->x + (child->w / 2) - (textWidth / 2); 
                else if (child->align == right) x = child->x +  child->w - textWidth; 
                if (child->vAlign == middle) y = child->y + (child->h / 2) - (textHeight / 2);
                else if (child->vAlign == top) y = child->y; 
                else if (child->vAlign == bottom) x = child->y + child->h - textHeight; 
                drawText(renderer, child->innerText, x, y, child->w, child->h, lightGray);
            }
        }

        // recursivelly navigate to the child node
        if (!renderLayoutRect(renderer, child)) return false;
    }
    return true;
}

#endif

