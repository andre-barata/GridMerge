#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
#include "include/SDL_ttf.h"
#include "include/res.h"
#include "include/common.h"
#include "include/font.h"
#include "include/viewModel.h"
#define EVENTS_IMPL 
#include "include/render.h"
#include "include/events.h"

#include "include/font2.h"

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char inputText[1024] = "";

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if (!initWindow(&windowWidth, &windowHeight)) return terminateAndLog("Could not init main window");
    if (!initFont()) return terminateAndLog("Failed to load fonts");
    if (!loadSpriteSets()) return terminateAndLog("Failed to load sprite set");
    if (!renderLayout(windowRenderer, &layout, 0, 0, windowWidth, windowHeight)) return terminateAndLog("Error while rendering the main layout!");

	// Start sending SDL_TextInput events
    //SDL_StartTextInput();

    renderHover(windowRenderer, 1500, 20);

    bool quit = false;
    ViewModel* mouseDownAt = NULL;
    while(!quit) {
        SDL_Event e;
        SDL_Delay(10);
        
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
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
                case SDL_MOUSEMOTION:
                    renderHover(windowRenderer, e.motion.x, e.motion.y);
                    break;
                case SDL_WINDOWEVENT:
                    if (e.window.event == SDL_WINDOWEVENT_LEAVE)
                        clearHover(windowRenderer);
                    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                        renderUpdatedLayout(windowRenderer, &layout, 0, 0, e.window.data1, e.window.data2);
                    break;
                case SDL_MOUSEBUTTONDOWN: 
                    mouseDownAt = findRectByXY(&layout, e.button.x, e.button.y);
                    break;
                case SDL_MOUSEBUTTONUP: 
                    if ((e.button.clicks == 1) && (e.button.button == SDL_BUTTON_LEFT) && 
                        (mouseDownAt != NULL) && (mouseDownAt->onClick != NULL) &&
                        (mouseDownAt == findRectByXY(&layout, e.button.x, e.button.y)))
                            mouseDownAt->onClick(e.button.x, e.button.y, mouseDownAt);
                    break;
            }
        }

        /*if (strlen(inputText)) {
            drawText(windowRenderer, inputText, 10, 10, 100, 40, white);
            SDL_RenderPresent(windowRenderer);
        }*/
    }
    cleanup();
    return 0;
}
