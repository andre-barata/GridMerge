#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
#include <SDL_image.h>
#include "include/SDL_ttf.h"
#include "include/res.h"
#include "include/common.h"
#include "include/font.h"
#include "include/viewModel.h"
#include "include/render.h"

#include "include/font2.h"

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char inputText[1024] = "";

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if(!initWindow(&windowWidth, &windowHeight)) return terminate();

    if(!initFont()) return terminate();

    /*
    // for loading Images:
	if(IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL_image: %s\n", SDL_GetError() );
		return terminate();
	}
    */

    if (!renderLayout(0, 0, windowWidth, windowHeight)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error while rendering the main layout! %s\n", SDL_GetError() );
        return terminate();
    }

    // test hashmap
    typedef struct { 
        char* key; char* value;
    } listItem;
    listItem* list = NULL;
    shput(list, "key1", "h");
    shput(list, "key2", "e");
    shput(list, "key3", "l");
    shput(list, "key4", "o");

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, shget(list, "key2"));
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, shget(list, "key3"));
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, shget(list, "key4"));

    // test
    initFont2(windowRenderer);
    //drawText(windowRenderer, "testing 123 testing ç à", 200, 200, 300, 40, white);
    /*
    stopwatchStart();
    drawText(windowRenderer, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ", 870, 150, 700, 200, white);
    stopwatchStop("font1 1st time");

    stopwatchStart();
    drawText(windowRenderer, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ", 870, 200, 700, 200, white);
    stopwatchStop("font1 2nd time");

    stopwatchStart();
    drawText2(windowRenderer, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ", 870, 250, 700, 200, white);
    stopwatchStop("font2 1st time");

    stopwatchStart();
    drawText2(windowRenderer, "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ ", 870, 300, 700, 200, white);
    stopwatchStop("font2 2nd time");*/
    drawText2(windowRenderer, "andre", 870, 250, 700, 200, white);

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "pixel format = %s \n", SDL_GetPixelFormatName(SDL_GetWindowPixelFormat( mainWindow )));
    //370546692

    SDL_RenderPresent(windowRenderer);

	// Start sending SDL_TextInput events
    SDL_StartTextInput();

    bool quit = false;
    while(!quit) {
        SDL_Event e;
        SDL_Delay(10);
        
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                case SDL_TEXTINPUT:
                    prevLen = strlen(inputText);
                    sprintf(inputText, "%s%s", inputText, e.text.text);
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    else if (e.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                        inputText[prevLen] = 0;
                    }
                    break;
            }
        }

        if (strlen(inputText)) {
            drawText(windowRenderer, inputText, 10, 10, 100, 40, white);
            SDL_RenderPresent(windowRenderer);
        }
    }
    return terminate();
}
