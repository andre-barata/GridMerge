#include <SDL.h>
#include <SDL_ttf.h>
#include "include/common.h"
#include "include/res.h"

#include <stdbool.h>
#define CODE(...) #__VA_ARGS__

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight;
    SDL_Renderer *windowrenderer;
    SDL_Rect      viewport;
    TTF_Font*     font;

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initWindow(&windowWidth, &windowHeight)) return terminate();

    if(!(windowrenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get window's surface. %s\n", SDL_GetError() );
        terminate();
    }

    viewport = (SDL_Rect){ 0, 0, 680, 480 };

	if ( !(font = TTF_OpenFont("font.ttf", 72)) ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
        terminate();
	}

	// Start sending SDL_TextInput events
	SDL_StartTextInput();

    // start render loop
    bool quit = false;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) quit = true;
            if(event.type == SDL_QUIT) quit = true;
        
        
            SDL_SetRenderDrawColor(windowrenderer, 80, 80, 80, 255);
            SDL_RenderClear(windowrenderer);
            //SDL_RenderCopy(windowrenderer, m_image, nullptr, nullptr);
            SDL_RenderSetViewport(windowrenderer, &viewport);
            SDL_RenderPresent(windowrenderer);

        }

    }
    return terminate();
}
