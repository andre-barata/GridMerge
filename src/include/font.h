#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include "common.h"
#include "res.h"

TTF_Font* mainFont;

bool initFont() {
    if(TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init SDL_TTF: %s\n", SDL_GetError() );
		return false;
    }
    // load the main TTF font from embeded resources
	if (!(mainFont = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_opensans_semibold.start), rc_opensans_semibold.size), 1, 12))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
		return false;
	}
    TTF_SetFontHinting(mainFont, TTF_HINTING_LIGHT_SUBPIXEL);

    return true;
}

bool drawText(SDL_Renderer* renderer, const char* text, int x, int y, int maxW, int maxH, SDL_Color textColor) {
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(mainFont, text, textColor);//TTF_RenderUTF8_Shaded(mainFont, text, black, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int w = textSurface->w > maxW ? maxW : textSurface->w;
    int h = textSurface->h > maxH ? maxH : textSurface->h;

    SDL_RenderCopy(renderer, textTexture, &(SDL_Rect){0,0,w,h}, &(SDL_Rect){x, y, w, h});

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

#endif