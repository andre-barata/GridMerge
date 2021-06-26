/*
  events.h: functions for handling UI events
  Copyright (C) 2021 RangeCode, Lda. <info@rangecode.com>
  https://www.rangecode.com

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#ifndef EVENTS_H
#define EVENTS_H

#include <SDL.h>
#include "../common.h"

// proto functions
void onClickClose(Uint32 x, Uint32 y, ViewModel* rect);
void onClickRestoreOrMaximize(Uint32 x, Uint32 y, ViewModel* rect);
void onClickMinimize(Uint32 x, Uint32 y, ViewModel* rect);

#endif
#ifdef EVENTS_IMPL
#ifndef EVENTS_IMPL_H
#define EVENTS_IMPL_H
#include "../view/render.h"
#include "../model/viewModel.h"

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
