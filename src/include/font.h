#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <SDL.h>
#include "SDL_ttf.h"
#include "res.h"
#include "constants.h"

TTF_Font* mainFont;

bool initFont() {
    if(TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init SDL_TTF: %s\n", SDL_GetError() );
		return false;
    }
    // load the main TTF font from embeded resources
	if (!(mainFont = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_roboto_thin.start), rc_roboto_thin.size), 1, 14))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
		return false;
	}
    TTF_SetFontHinting(mainFont, TTF_HINTING_LIGHT_SUBPIXEL);

    return true;
}

bool drawText(SDL_Renderer* renderer, const char* text, int x, int y, int maxW, int maxH) {
    SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(mainFont, text, black, white);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect dest = {x, y, textSurface->w > maxW ? maxW : textSurface->w, 
                           textSurface->h > maxH ? maxH : textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &dest);

    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

#endif