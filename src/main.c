/*
  GridMerge: a light tool for visually analyzing and merging data
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

#include <string.h>
#include <stdbool.h>
//#include <utf8proc.h>
#include <SDL.h>
#include "include/res.h"
#include "include/common.h"
#include "include/view/font.h"
#include "include/model/viewModel.h"
#define EVENTS_IMPL 
#include "include/view/render.h"
#include "include/controller/events.h"
#include "include/view/gridView.h"

int main(int argc, char *argv[]) {
    int windowWidth, windowHeight, prevLen = 0;
    char inputText[1024] = "";

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);
    resInit();

    if (!initWindow(&windowWidth, &windowHeight)) return terminateAndLog("Could not init main window");
    if (!initFont(windowRenderer)) return terminateAndLog("Failed to load fonts");
    if (!loadSpriteSets()) return terminateAndLog("Failed to load sprite set");
    if (!renderLayout(windowRenderer, &layout, 0, 0, windowWidth, windowHeight)) return terminateAndLog("Error while rendering the main layout!");

	// Start sending SDL_TextInput events
    //SDL_StartTextInput();
    //SDL_RenderClear(windowRenderer);
    /*stopwatchStart();
    //drawText(windowRenderer, "!\\\"#$\%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}", 0, 70, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "De v??rias conversas a que temos a oportunidade de assistir e participar, em alguma altura pode sempre surgir um termo de compara????o misturado com competitividade.", 0, 95, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "Marte ?? um objetivo para as pot??ncias mundiais. Na verdade, sendo aquele planeta deserto, in??spito e sem evid??ncias claras de poder ser um segundo lar para os seres humanos, porqu?? tanto interesse?", 0, 120, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "A NASA tem uma abordagem igualmente ambiciosa, diz que antes de 2030 n??o ser?? vi??vel, mas a China acredita que ser?? o pa??s o primeiro a conseguir esta miss??o tripulada ao planeta.", 0, 145, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "A Google lan??ou a vers??o Beta do Android 12 h?? poucas semanas dirigido ao grupo de testes que vai ajustar todos os seus pormenores at?? se chegar a uma vers??o final dirigida ao p??blico em geral, que dever?? acontecer em finais de agosto. ", 0, 170, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "Al??m dos smartphones da Google, existem mais marcas associadas ao programa Android 12 Developer, mas a Samsung n??o ?? uma delas.", 0, 195, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "Contudo, a empresa sul-coreana que se juntou ?? Google numa fus??o entre o Wear OS e o Tizen para smartwatches, poder?? surpreender os seus utilizadores com o Android 12 e a One UI 4.0 mais cedo do que se esperava.", 0, 220, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "De todas as operadoras, a NOWO ?? a que apresente mensalidade mais baixa para oito servi??os/ofertas. A MEO diminui mensalidade de um servi??o e a NOS e Vodafone aumentam mensalidades.", 0, 245, 800, 25, white, gray4, 12);
    drawText(windowRenderer, "Neste mundo cada vez mais digital, est?? dif??cil ser garantida a privacidade dos utilizadores. Mesmo com o Regulamento Geral de Prote????o de Dados (RGPD) em vigor desde 2018 no nosso pa??s, os casos relacionados com partilha de dados t??m acontecido sucessivamente.", 0, 270, 800, 25, white, gray4, 12);
    stopwatchStop("text render");
    SDL_RenderPresent(windowRenderer);
*/
    loadGrid("d:\\dev\\excel\\allocation1.xlsx");
    ViewModel* gridHolder = getModelById("lGrid");
    renderGrid(windowRenderer, gridHolder->x, gridHolder->y, gridHolder->w, gridHolder->h);

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
