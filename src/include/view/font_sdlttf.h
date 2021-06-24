/*
  font_sdlttf.h: A library for loading and rendering fonts with SDL_TTF and SDL2
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
#ifndef FONT_SDLTTF_H
#define FONT_SDLTTF_H

#include <stdbool.h>
#include <SDL.h>
#define TTF_USE_SDF
#include "../thirdparty/SDL_ttf.h"
#include "../common.h"
#include "../res.h"

TTF_Font* mainFont;

bool initFont(SDL_Renderer* renderer) {
    if(TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init SDL_TTF: %s\n", SDL_GetError() );
		return false;
    }
    // load the main TTF font from embeded resources
	if (!(mainFont = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_opensans_semibold.start), rc_opensans_semibold.size), 1, 12))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
		return false;
	}

    return true;
}

bool drawText(SDL_Renderer* renderer, const char* text, int x, int y, int maxW, int maxH, SDL_Color textColor, SDL_Color bgColor, int sizeEm) {
    SDL_Surface* textSurface = TTF_RenderUTF8_LCD(mainFont, text, textColor, bgColor);
    if (textSurface == NULL) return false;
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) return false;

    int w = textSurface->w > maxW ? maxW : textSurface->w;
    int h = textSurface->h > maxH ? maxH : textSurface->h;

    if (SDL_RenderCopy(renderer, textTexture, &(SDL_Rect){0,0,w,h}, &(SDL_Rect){x, y, w, h}) != 0) return false;

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
    return true;
}

bool getTextDimensions(unsigned char* text, int* width, int* height, int sizeEm) {
    int glyphs;
    TTF_MeasureUTF8(mainFont, text, 90000, width, &glyphs);
    *height = TTF_FontHeight(mainFont);
}

#endif