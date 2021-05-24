#include <string.h>
#include <utf8proc.h>
#include <SDL.h>
#include <SDL_image.h>
#include "include/SDL_ttf.h"
#include "include/common.h"
#include "include/res.h"

#include <stdbool.h>
#define CODE(...) #__VA_ARGS__

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char input[1024] = "";
    SDL_Texture *textTexture;

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initWindow(&windowWidth, &windowHeight)) return terminate();

	if(IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL_image: %s\n", SDL_GetError() );
		return false;
	}

    if(TTF_Init() < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not init SDL_TTF: %s\n", SDL_GetError() );
        terminate();
    }

	if (!(font = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_roboto_thin.start), rc_roboto_thin.size), 1, 14))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
        terminate();
	}
    TTF_SetFontHinting(font, TTF_HINTING_LIGHT_SUBPIXEL);

	// Start sending SDL_TextInput events
	SDL_StartTextInput();

    bool quit = false;
    while(!quit){
        SDL_Event e;
        
        SDL_SetRenderDrawColor(windowrenderer, 80, 80, 80, 255);
        SDL_RenderClear(windowrenderer);
        
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    return false;
                case SDL_TEXTINPUT:
                    prevLen = strlen(input);
                    sprintf(input, "%s%s", input, e.text.text);
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(input) > 0) {
                        input[prevLen] = 0;
                    }
                    break;
            }
        }

	    SDL_RenderCopy(windowrenderer, bufferTexture, NULL, NULL);

        if (strlen(input)) {
            SDL_Color foreground = { 0, 0, 0 };
            SDL_Color background = { 255, 255, 255 };
//            SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font, (char*)input, foreground);
            SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(font, (char*)input, foreground, background);
            textTexture = SDL_CreateTextureFromSurface(windowrenderer, textSurface);

            SDL_Rect dest;
            dest.x = 320 - (textSurface->w / 2.0f);
            dest.y = 240;
            dest.w = textSurface->w;
            dest.h = textSurface->h;
            SDL_RenderCopy(windowrenderer, textTexture, NULL, &dest);

            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(windowrenderer);

    }
    return terminate();
}
