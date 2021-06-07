#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
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

    if (!initWindow(&windowWidth, &windowHeight)) return terminateAndLog("Could not init main window");
    if (!initFont()) return terminateAndLog("Failed to load fonts");
    if (!loadSpriteSets()) return terminateAndLog("Failed to load sprite set");
    if (!renderLayout(windowRenderer, 0, 0, windowWidth, windowHeight)) return terminateAndLog("Error while rendering the main layout!");

    // #### functionality tests: #########

/*
    // for loading Images:
	if(IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Error initializing SDL_image: %s\n", SDL_GetError() );
		return terminate();
	}
*/

 //   initFont2(windowRenderer);
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
//    drawText2(windowRenderer, "andre", 870, 250, 700, 200, white);

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
    cleanup();
    return 0;
}
