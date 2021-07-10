/*
  res.h : embedded resource macros and file list
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
#ifndef RES_H
#define RES_H

typedef struct _ResStruct {
    const char* start;
    const char* end;
    unsigned long size;
} ResStruct;

// macro for defining symbol variables for every resource file
#define RESFILE(file, extension) \
    extern const char _binary_res_ ## file ## _ ## extension ## _start[]; \
    extern const char _binary_res_ ## file ## _ ## extension ## _end[]; \
    extern const char _binary_res_ ## file ## _ ## extension ## _size[]; \
    ResStruct rc_ ## file;
#define RESINIT(file, extension) \
    rc_ ## file.start = _binary_res_ ## file ## _ ## extension ## _start; \
    rc_ ## file.end   = _binary_res_ ## file ## _ ## extension ## _end; \
    rc_ ## file.size = &_binary_res_ ## file ## _ ## extension ## _end[0] - &_binary_res_ ## file ## _ ## extension ## _start[0];

// THE list of resource files
//RESFILE(roboto_thin,ttf)
RESFILE(opensans_regular,ttf)
RESFILE(opensans_semibold,ttf)
RESFILE(set10x10,png)
//...
void resInit(void) {
    //RESINIT(roboto_thin,ttf)
    RESINIT(opensans_regular,ttf)
    RESINIT(opensans_semibold,ttf)
    RESINIT(set10x10,png)
//  ...
}

/*
usage example: 

    char txt[rc_test.size+1];
    sprintf(txt, "%s", rc_test.start);
    SDL_Log( "out: %s\n", txt);
*/



#endif
