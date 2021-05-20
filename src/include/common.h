#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <SDL.h>

SDL_Window* mainWindow;

bool initWindow(int* width, int* height) {
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error initializing SDL! %s\n", SDL_GetError() );
        return false;
    }
    mainWindow = SDL_CreateWindow("GridMerge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS );
    if (mainWindow == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating SDL window! %s\n", SDL_GetError() );
        return false;
    }

    // attempt to maximize
    int display_index = SDL_GetWindowDisplayIndex(mainWindow);
    if (display_index < 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "failed to get default display index %s\n", SDL_GetError() );
    }
    SDL_Rect usable_bounds;
    if (SDL_GetDisplayUsableBounds(display_index, &usable_bounds) != 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "failed to get display usable bounds %s\n", SDL_GetError() );
    }
    else {
        SDL_SetWindowPosition(mainWindow, usable_bounds.x, usable_bounds.y);
        SDL_SetWindowSize(mainWindow, usable_bounds.w, usable_bounds.h);
    }

    SDL_GetWindowSize(mainWindow, width, height);

    return true;
}

int terminate() {
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    return 1;
}

#endif