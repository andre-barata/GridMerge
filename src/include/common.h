#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <gl/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

SDL_GLContext glContext;
SDL_Window* mainWindow;

bool initGlew(void) {
    glewExperimental = GL_TRUE; 
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK ) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        return false;
    }
    return true;
};

bool initGlWindow(int* width, int* height) {
    if( SDL_Init(SDL_INIT_VIDEO) != 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error initializing SDL! %s\n", SDL_GetError() );
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    mainWindow = SDL_CreateWindow("Main", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS );
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

    glContext = SDL_GL_CreateContext(mainWindow);
    if (glContext == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating GL context(SDL)! %s\n", SDL_GetError() );
        return false;
    }

    SDL_GetWindowSize(mainWindow, width, height);

    // vsync enabled
    SDL_GL_SetSwapInterval(1);

    return true;
}

int terminate() {
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(mainWindow);
    SDL_Quit();
    return 1;
}

#endif