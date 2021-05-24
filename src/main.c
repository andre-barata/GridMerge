#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
#include <SDL_image.h>
#include "include/SDL_ttf.h"
#include "include/common.h"
#include "include/res.h"

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char input[1024] = "";
    SDL_Texture *textTexture;

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initWindow(&windowWidth, &windowHeight)) return terminate();

    if(!initFont()) return terminate();

    /*
    // for loading Images:
	if(IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL_image: %s\n", SDL_GetError() );
		return terminate();
	}
    */

	// Start sending SDL_TextInput events
	SDL_StartTextInput();

    bool quit = false;
    while(!quit){
        SDL_Event e;
        
        SDL_SetRenderDrawColor(windowRenderer, 80, 80, 80, 255);
        SDL_RenderClear(windowRenderer);
        
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

	    SDL_RenderCopy(windowRenderer, bufferTexture, NULL, NULL);

        if (strlen(input)) {
            SDL_Surface* textSurface = TTF_RenderUTF8_Shaded(mainFont, (char*)input, black, white);
            textTexture = SDL_CreateTextureFromSurface(windowRenderer, textSurface);

            SDL_Rect dest = {320-(textSurface->w/2.0f), 240, textSurface->w, textSurface->h};
            SDL_RenderCopy(windowRenderer, textTexture, NULL, &dest);

            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }

        SDL_RenderPresent(windowRenderer);

    }
    return terminate();
}
