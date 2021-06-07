#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <SDL.h>
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Color black     = { 0x00, 0x00, 0x00 };
SDL_Color gray1     = { 0x1e, 0x1e, 0x1e };
SDL_Color gray2     = { 0x25, 0x25, 0x26 };
SDL_Color gray3     = { 0x2d, 0x2d, 0x2d };
SDL_Color gray4     = { 0x3c, 0x3c, 0x3c };
SDL_Color gray5     = { 0x40, 0x40, 0x40 };
SDL_Color gray6     = { 0x50, 0x50, 0x50 };
SDL_Color lightGray = { 0xcc, 0xcc, 0xcc };
SDL_Color white     = { 0xff, 0xff, 0xff };
SDL_Color blue1     = { 0x00, 0x7a, 0xcc };

#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

SDL_Window* mainWindow;
SDL_Renderer* windowRenderer;
SDL_Texture *bufferTexture;

SDL_HitTestResult resizeCallback(SDL_Window* window, const SDL_Point* point, void* data){ 
    return SDL_HITTEST_RESIZE_BOTTOM;
}

// initializations

bool initWindow(int* width, int* height) {
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL! %s\n", SDL_GetError() );
        return false;
    }
    mainWindow = SDL_CreateWindow("GridMerge", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE );
    if (mainWindow == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating SDL window! %s\n", SDL_GetError() );
        return false;
    }

    // attempt to create a GPU renderer (DirectX, OpenGL, OpenGL ES...), if unsuccessfull create a software renderer
    windowRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if(windowRenderer == NULL) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Failed to create hardware renderer. Fallback to SOFTWARE rendering... %s\n", SDL_GetError() );
        
        windowRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_SOFTWARE);
        if(windowRenderer == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create software renderer. %s\n", SDL_GetError() );
            return false;
        }
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
    //SDL_SetWindowResizable(mainWindow, SDL_TRUE);
    //SDL_SetWindowHitTest(mainWindow, resizeCallback, NULL);
    //SDL_MinimizeWindow(mainWindow);
    SDL_GetWindowSize(mainWindow, width, height);

    // create a buffer bitmap and a GPU texture from it
    
    SDL_Surface* buffer = SDL_CreateRGBSurfaceWithFormat(0, *width, *height, 32, SDL_PIXELFORMAT_RGBA32);
    if (buffer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture surface: %s\n", SDL_GetError());
        return false;
    }
	bufferTexture = SDL_CreateTextureFromSurface(windowRenderer, buffer);
	SDL_FreeSurface(buffer);
	if ( bufferTexture == NULL ) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error creating buffer texture: %s\n", SDL_GetError());
		return false;
	}
    
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Continuing from window init... \n" );
    return true;
}

// StopWatch timer

uint64_t stopwatchStartTime, stopwatchStopTime;
double stopwatchElapsedTime;
void stopwatchStart() {
    stopwatchStartTime = SDL_GetPerformanceCounter();
}
void stopwatchStop(char* logMessage) {
    stopwatchStopTime = SDL_GetPerformanceCounter();
    double elapsedTime = (double)((stopwatchStopTime - stopwatchStartTime) / (double)(SDL_GetPerformanceFrequency()));
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s : duration = %f s\n", logMessage, elapsedTime);
}

// Sprites

typedef struct _Sprite {
    const char* id;
    Uint16 x, y;
    enum { light, dark } brightness;
    struct _SpriteSet* parentSet;        
} Sprite;
typedef struct _SpriteSet {
    SDL_Texture* texture;
    Uint16 spriteW, spriteH;
    Uint16 numSpritesX, numSpritesY;
    ResStruct* resource;
    Sprite* sprites;
} SpriteSet;

// define a sprite set for image set10x10.png embeded in the resources
SpriteSet set10x10 = {
    .spriteW = 10, .spriteH = 10,
    .numSpritesX = 4, .numSpritesY = 2,
    .resource = &rc_set10x10,
    .sprites = (Sprite[]) {
        { .id = "close-dark", .brightness = dark },
        { .id = "restore-dark", .brightness = dark },
        { .id = "minimize-dark", .brightness = dark },
        { .id = "maximize-dark", .brightness = dark },
        { .id = "close-light", .brightness = light },
        { .id = "restore-light", .brightness = light },
        { .id = "minimize-light", .brightness = light },
        { .id = "maximize-light", .brightness = light }
    }
};

// HashSet for addressing each Sprite by Id
typedef struct { char* key; Sprite value; } spriteItem;
spriteItem* spriteHashSet = NULL;

SDL_Texture* loadSpriteSetTexture(ResStruct* resource) {
    int w,h,bpp;
    // load PNG to RGBA buffer
    unsigned char *setBuffer = stbi_load_from_memory((const stbi_uc*)(resource->start), resource->size, &w, &h, &bpp, 4);
    if (setBuffer == NULL) return false;
    SDL_Surface* setSurface = SDL_CreateRGBSurfaceWithFormatFrom(setBuffer, w, h, bpp, w*bpp, SDL_PIXELFORMAT_ARGB8888);
    if (setSurface == NULL) return false;
    SDL_Texture* setTexture = SDL_CreateTextureFromSurface(windowRenderer, setSurface);
    SDL_FreeSurface(setSurface); setSurface = NULL;
    free(setBuffer); setBuffer = NULL;
    return setTexture;
}

bool loadSpriteSets() {
    // init set10x10
    SDL_Texture* texture10x10 = loadSpriteSetTexture(set10x10.resource);
    for (int i = 0; i < set10x10.numSpritesX * set10x10.numSpritesY; i++) {
        set10x10.sprites[i].parentSet = &set10x10;
        set10x10.texture = texture10x10;
        set10x10.sprites[i].y = (i / set10x10.numSpritesX) * set10x10.spriteH;
        set10x10.sprites[i].x = (i % set10x10.numSpritesX) * set10x10.spriteW;
        shput(spriteHashSet, set10x10.sprites[i].id, set10x10.sprites[i]);
    }

    return true;
}

// terminate and cleanup

void cleanup() {
    SDL_StopTextInput();

    SDL_DestroyTexture(set10x10.texture);
    shfree(spriteHashSet);

	TTF_CloseFont(mainFont);
	SDL_DestroyTexture(bufferTexture);
	bufferTexture = NULL;

    SDL_DestroyRenderer(windowRenderer);
    SDL_DestroyWindow(mainWindow);
}
int terminate() {
    cleanup();
    SDL_Quit();
    return 1;
}
int terminateAndLog(const char* text) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s :: %s\n", text, SDL_GetError());
    terminate();
}


#endif