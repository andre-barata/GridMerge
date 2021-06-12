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
    if (mainWindowState & winNormal) {
        maximizeMainWindow();
        rect->imageId = "restore-light";
    } else {
        restoreMainWindow();
        rect->imageId = "maximize-light";
    }
}

void onClickMinimize(Uint32 x, Uint32 y, ViewModel* rect) {
    minimizeMainWindow();
}
#endif
#endif
