#ifndef EVENTS_H
#define EVENTS_H

#include <SDL.h>
#include "common.h"

void onClickClose(Uint32 x, Uint32 y) {
    cleanup();
    exit(0);
}

#endif