#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
#include <SDL_image.h>
#include "include/SDL_ttf.h"
#include "include/res.h"
#include "include/common.h"
#include "include/font.h"
#include "include/viewModel.h"

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char inputText[1024] = "";

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

    SDL_SetRenderDrawColor(windowRenderer, 80, 80, 80, 255);
    SDL_RenderClear(windowRenderer);
    SDL_RenderCopy(windowRenderer, bufferTexture, NULL, NULL);

    bool quit = false;
    while(!quit) {
        SDL_Event e;
        SDL_Delay(10);
        
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                case SDL_TEXTINPUT:
                    prevLen = strlen(inputText);
                    sprintf(inputText, "%s%s", inputText, e.text.text);
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    else if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                        inputText[prevLen] = 0;
                    }
                    break;
            }
        }

        if (strlen(inputText)) 
            drawText(windowRenderer, inputText, 10, 10, 100, 40);

        SDL_RenderPresent(windowRenderer);

    }
    return terminate();
}
