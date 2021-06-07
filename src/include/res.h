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
RESFILE(roboto_thin,ttf)
RESFILE(opensans_regular,ttf)
RESFILE(opensans_semibold,ttf)
RESFILE(set10x10,png)
//...
void resInit(void) {
    RESINIT(roboto_thin,ttf)
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