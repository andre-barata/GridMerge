#ifndef RES_H
#define RES_H


// macro for defining symbol variables for every resource file
#define RESFILE(file, extension) \
    extern const char _binary_res_ ## file ## _ ## extension ## _start[]; \
    extern const char _binary_res_ ## file ## _ ## extension ## _end[]; \
    extern const char _binary_res_ ## file ## _ ## extension ## _size[]; \
    struct resStruct rc_ ## file;
#define RESINIT(file, extension) \
    rc_ ## file.start = _binary_res_ ## file ## _ ## extension ## _start; \
    rc_ ## file.end   = _binary_res_ ## file ## _ ## extension ## _end; \
    rc_ ## file.size = &_binary_res_ ## file ## _ ## extension ## _end[0] - &_binary_res_ ## file ## _ ## extension ## _start[0];

struct resStruct {
    const char* start;
    const char* end;
    unsigned long size;
};

// THE list of resource files
RESFILE(test,txt)
RESFILE(roboto_thin,ttf)
//...
void resInit(void) {
    RESINIT(test,txt)
    RESINIT(roboto_thin,ttf)
//  ...
}

/*
usage example: 

    char txt[rc_test.size+1];
    sprintf(txt, "%s", rc_test.start);
    SDL_Log( "out: %s\n", txt);
*/



#endif