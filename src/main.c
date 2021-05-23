#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include "include/common.h"
#include "include/res.h"

#include <stdbool.h>
#define CODE(...) #__VA_ARGS__

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight;
    //SDL_Rect viewport;
    char input[1024];
    SDL_Texture *text;

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

    //viewport = (SDL_Rect){ 0, 0, 680, 480 };

	if (!(font = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_roboto_thin.start), rc_roboto_thin.size), 1, 72))) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error loading font: %s\n", SDL_GetError() );
        terminate();
	}

	// Start sending SDL_TextInput events
	SDL_StartTextInput();

    // start render loop
    bool quit = false;
    while(!quit){
        //static const unsigned char* keys = SDL_GetKeyboardState( NULL );
        SDL_Event e;
        
        SDL_SetRenderDrawColor(windowrenderer, 80, 80, 80, 255);
        SDL_RenderClear(windowrenderer);
        
        while ( SDL_PollEvent( &e ) != 0 ) {
            switch (e.type) {
                case SDL_QUIT:
                    return false;
                case SDL_TEXTINPUT:
                    sprintf(input, "%s%s", input, e.text.text);
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(input) > 0) {
                        //input.pop_back();
                    }
                    break;
            }
        }

	    SDL_RenderCopy(windowrenderer, bufferTexture, NULL, NULL);

        if ( strlen(input) ) {
            SDL_Color foreground = { 255, 255, 255 };
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, input, foreground);
            text = SDL_CreateTextureFromSurface(windowrenderer, textSurface);

            SDL_Rect dest;
            dest.x = 320 - (textSurface->w / 2.0f);
            dest.y = 240;
            dest.w = textSurface->w;
            dest.h = textSurface->h;
            SDL_RenderCopy(windowrenderer, text, NULL, &dest);

            SDL_DestroyTexture(text);
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(windowrenderer);

    }
    return terminate();
}
