#include <SDL.h>
#include "include/common.h"
#include "include/res.h"

#include <stdbool.h>
#define CODE(...) #__VA_ARGS__

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight;

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initWindow(&windowWidth, &windowHeight)) return terminate();

    

    // start render loop
    bool quit = false;
    while(!quit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) quit = true;
            if(event.type == SDL_QUIT) quit = true;
        }

    }
    return terminate();
}
