#ifndef FONT2_H
#define FONT2_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "common.h"
#include "res.h"

stbtt_fontinfo font;
int fontBaseline, fontAscent;
float fontScale;
stbtt_pack_context packCtx;

bool initFont2(SDL_Renderer* renderer) {
    // load the main TTF font from embeded resources
	//if (!(mainFont = TTF_OpenFontRW(SDL_RWFromMem((void*)(rc_opensans_semibold.start), rc_opensans_semibold.size), 1, 12))) {
    stbtt_InitFont(&font, (const unsigned char*)rc_opensans_regular.start, stbtt_GetFontOffsetForIndex(rc_opensans_regular.start, 0));
    fontScale = stbtt_ScaleForMappingEmToPixels(&font, 12);
    stbtt_GetFontVMetrics(&font, &fontAscent,0,0);
    fontBaseline = (int)(fontAscent * fontScale);



    SDL_Surface* packBitmap = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 512, 8, 0, 0, 0, 0);
    SDL_Color colors[256];
    for (int i = 0; i < 256; i++) colors[i].r = colors[i].g = colors[i].b = (Uint8)i;
    SDL_SetPaletteColors(packBitmap->format->palette, colors, 0, 256);

    stbtt_packedchar* packChars = malloc(font.numGlyphs * sizeof(stbtt_packedchar));
    stbtt_PackBegin(&packCtx, packBitmap->pixels, packBitmap->w, packBitmap->h, packBitmap->pitch, 1, NULL);
    
    stbtt_PackSetOversampling(&packCtx, 3, 1);
    stbtt_PackFontRange(&packCtx, rc_opensans_regular.start, 0, 17, 0, font.numGlyphs, packChars);
    stbtt_PackEnd(&packCtx);

    SDL_Texture* packTexture = SDL_CreateTextureFromSurface(renderer, packBitmap);
    SDL_RenderCopy(renderer, packTexture, NULL, &(SDL_Rect){0, 0, packBitmap->w, packBitmap->h});

    return true;
}

int getCodepointSize(unsigned char* text) {
    if (text[0] < 0x80) return 1;
    else if (text[0] < 0xE0) return 2;
    else if (text[0] < 0xF0) return 3;
    else if (text[0] < 0xF8) return 4;
    return 0;
}
Uint32 getCodepoint(unsigned char* text, int size) {
    Uint32 cp;
    switch (size) {
        case 1: cp = text[0] & 0x7F;
        case 2: cp = text[0] & 0x1F;
        case 3: cp = text[0] & 0x0F;
        case 4: cp = text[0] & 0x07;
    }
    for(int i = 1; i < size; ++i)
        cp = (cp << 6) | (Uint32)(text[i] & 0x3F);
    return cp;
}

SDL_Texture* characterSprites = NULL;
// TODO: add hashmap cache
// gets a character glyph GPU texture, with antialias, subpixel allignement and LCD ClearType Harmony LCD rendering
SDL_Texture* getCharacterSprite(SDL_Renderer* renderer, const stbtt_fontinfo* info, float scale, float shiftX, int cp, 
                        int* w, int* h, int* xoff, int* yoff) {
    unsigned char* cpBitmap;
    SDL_Surface* characterSurface;
    cpBitmap = stbtt_GetCodepointBitmapSubpixel(&font, scale*3, scale, shiftX, 0, cp, w, h, xoff, yoff);
    if (*w == 0) return NULL;
    int spriteW = 1 + (*w - 1) / 3;
    characterSurface = SDL_CreateRGBSurfaceWithFormat(0, spriteW, *h, 32, SDL_PIXELFORMAT_ARGB8888);
    int ix, iy, i3 = 0, i4 = 0, b, g, r;
    unsigned char* pDst = characterSurface->pixels;
stopwatchStart();
    for (iy = 0; iy < *h; iy++) {
        i3 = iy * *w;
        i4 = iy * characterSurface->pitch;
        for (ix = 0; ix < *w; ix += 3) {
            b = *(pDst + i4 + 0) = (ix + 2 < *w) ? *(cpBitmap + i3 + 2) : 0; // Blue
            g = *(pDst + i4 + 1) = (ix + 1 < *w) ? *(cpBitmap + i3 + 1) : 0; // Green
            r = *(pDst + i4 + 2) = (ix + 0 < *w) ? *(cpBitmap + i3 + 0) : 0; // Red
            *(pDst + i4 + 3) = 0xff;//(r + g + b) / 3; // Alpha
            i3 += 3; i4 += 4;
        }
    }
stopwatchStop("pixel for");
    free(cpBitmap);
stopwatchStart();
    SDL_Texture* charaterTexture = SDL_CreateTextureFromSurface(renderer, characterSurface);
stopwatchStop("SDL_CreateTextureFromSurface");
    SDL_FreeSurface(characterSurface);
    *w = spriteW;
    return charaterTexture;
}


bool drawText2(SDL_Renderer* renderer, unsigned char* text, int x, int y, int maxW, int maxH, SDL_Color textColor) {
    int lsb, advance, cpSz, cp, w, h, xoff, yoff;
    float ix = 0, xShift;
    SDL_Texture* characterSprite;
    while (*text) {
        cpSz = getCodepointSize(text);
        cp = (cpSz > 1) ? (int)getCodepoint(text, cpSz) : *text;

        stbtt_GetCodepointHMetrics(&font, *text, &advance, &lsb);

        xShift = ix - (float)floor(ix) - (lsb * fontScale * 2);
        characterSprite = getCharacterSprite(renderer, &font, fontScale, xShift, cp, &w, &h, &xoff, &yoff);

        SDL_SetTextureBlendMode(characterSprite, SDL_BLENDMODE_ADD);
        if ((characterSprite != NULL) && (ix + xoff + w <= maxW) && (yoff + h <= maxH)) {
stopwatchStart();
            SDL_RenderCopy(renderer, characterSprite, NULL, &(SDL_Rect){x + ix + xoff, y + yoff, w, h});
stopwatchStop("SDL_RenderCopy");
        }

        ix += advance * fontScale;
        if (*(text + cpSz))
            ix += fontScale * stbtt_GetCodepointKernAdvance(&font, cp, getCodepoint(text + cpSz, getCodepointSize(text + cpSz)));
        text += cpSz;
    }

    //SDL_DestroyTexture(textTexture);
    return true;
}

#endif