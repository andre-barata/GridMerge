#ifndef EVENTS_H
#define EVENTS_H

#include <SDL.h>
#include "common.h"

// proto functions
void onClickClose(Uint32 x, Uint32 y, ViewModel* rect);
void onClickRestoreOrMaximize(Uint32 x, Uint32 y, ViewModel* rect);
void onClickMinimize(Uint32 x, Uint32 y, ViewModel* rect);

#endif
#ifdef EVENTS_IMPL
#ifndef EVENTS_IMPL_H
#define EVENTS_IMPL_H
#include "render.h"
#include "viewModel.h"

void onClickClose(Uint32 x, Uint32 y, ViewModel* rect) {
    cleanup();
    exit(0);
}

void onClickRestoreOrMaximize(Uint32 x, Uint32 y, ViewModel* rect) {
    int width, height;
    if (mainWindowState & winNormal) {
        maximizeMainWindow();
        rect->imageId = "restore-light";
    } else {
        restoreMainWindow();
        rect->imageId = "maximize-light";
    }
    SDL_GetWindowSize(mainWindow, &width, &height);

    if (!computeModel(&layout, 0, 0, width, height)) return;
    if (renderLayoutRects(windowRenderer, &layout)) SDL_RenderPresent(windowRenderer);
    else SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not render main layout!\n" );
}

void onClickMinimize(Uint32 x, Uint32 y, ViewModel* rect) {
    minimizeMainWindow();
}
#endif
#endif
