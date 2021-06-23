/*
  font_stb.h: A library for loading and rendering fonts with stb_truetype and SDL2
  Copyright (C) 2021 RangeCode, Lda. <info@rangecode.com>

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

#ifndef FONT2_H
#define FONT2_H

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "../common.h"
#include "../res.h"

// Load a font pack into a cache texture, with a specified font size
// the first half of the texture contains a black background character set, and the second part, it's negative image
// this is used later for faster GPU based blending
SDL_Texture* loadPackCacheTexture(SDL_Renderer* renderer, const unsigned char* fontFileData, float fontSizesEm[], int cntFontSizes,
    int sampleWidth, int sampleHeight, int* numChars, stbtt_pack_range** ranges) {
    // sample 3 times wider font, to account for individual R G B channels
    int cntSamples = 3;
    stbtt_pack_context packCtx;

    // create a grayscale bitmap for the pack. assign a pallete in case we want to display the surface
    SDL_Surface* packBitmap = SDL_CreateRGBSurface(SDL_SWSURFACE, sampleWidth * cntSamples, sampleHeight, 8, 0, 0, 0, 0);
    SDL_Color colors[256]; 
    for (int i = 0; i < 256; i++) colors[i].r = colors[i].g = colors[i].b = (Uint8)i;
    SDL_SetPaletteColors(packBitmap->format->palette, colors, 0, 256);
    
    // allocate <numGlyphs> packed chars
    stbtt_fontinfo fontInfo;
    stbtt_InitFont(&fontInfo, fontFileData, stbtt_GetFontOffsetForIndex(fontFileData, 0));
    *numChars = fontInfo.numGlyphs;
    // start the pack, set oversampling
    stbtt_PackBegin(&packCtx, (unsigned char*)packBitmap->pixels, packBitmap->w, packBitmap->h, packBitmap->pitch, cntSamples /* padding is 3: 1 pixel*/, NULL);
    stbtt_PackSetOversampling(&packCtx, cntSamples, 1);
    // add a font range for each specified size and save ranges
    *ranges = (stbtt_pack_range*)calloc(cntFontSizes, sizeof(stbtt_pack_range));
    for(int i = 0; i < cntFontSizes; i++) {
        stbtt_packedchar* packChars = (stbtt_packedchar*)malloc(fontInfo.numGlyphs * sizeof(stbtt_packedchar));
        (*ranges)[i].first_unicode_codepoint_in_range = 0;
        (*ranges)[i].array_of_unicode_codepoints      = NULL;
        (*ranges)[i].num_chars                        = *numChars;
        (*ranges)[i].chardata_for_range               = packChars;
        (*ranges)[i].font_size                        = -fontSizesEm[i];
        stbtt_PackFontRanges(&packCtx, fontFileData, 0, &(*ranges)[i], 1);
    }
    stbtt_PackEnd(&packCtx);

    // sample RGB channels from pixels (from oversampled by 3x (R,G,B))
    // 3 samples are also saved for subpixel alignement based on the same 3x oversample 
    // adds the inverted color version in the bottom
    // Texture:
    //  ____
    // |    | -> white on black 1/3
    // |____|    sample aligned to bitmap's first pixel
    // |    | -> white on black 2/3
    // |____|    sample aligned to bitmap's second pixel
    // |    | -> white on black 3/3
    // |____|    sample aligned to bitmap's third pixel
    // |    | -> black on white 1/3 (inverted colors)
    // |____|    sample aligned to bitmap's first pixel
    // |    | -> black on white 2/3 (inverted colors)
    // |____|    sample aligned to bitmap's second pixel
    // |    | -> black on white 3/3 (inverted colors)
    // |____|    sample aligned to bitmap's third pixel
    
    Uint8* txPixels; int txPitch;
    SDL_Texture* packTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, sampleWidth, sampleHeight * cntSamples * 2);
    SDL_LockTexture(packTexture, NULL, (void**)&txPixels, &txPitch);
    int bmpSize = sampleWidth * cntSamples * sampleHeight;
    int offsetInvertedTx = txPitch * sampleHeight * cntSamples;
    Uint8* bmpPixels = (Uint8*)packBitmap->pixels;
    Uint8* bmpPixelsEnd = bmpPixels + bmpSize;
    //#   define gamma 1 /*1.43, 2.22*/
    //*(txPixels++) = pow(*(ftPixels + 2), 1.0 / gamma) * (256.0 / pow(256, 1.0 / gamma)); // B
    stopwatchStart();
    for (int i = 0; i < cntSamples; i++) {
        while (bmpPixels < bmpPixelsEnd) {
            *(txPixels++ + offsetInvertedTx) = 0xFF - (*(txPixels) = *(bmpPixels + 2)); // B
            *(txPixels++ + offsetInvertedTx) = 0xFF - (*(txPixels) = *(bmpPixels + 1)); // G
            *(txPixels++ + offsetInvertedTx) = 0xFF - (*(txPixels) = *(bmpPixels + 0)); // R
            *(txPixels++ + offsetInvertedTx) =         *(txPixels) = 0xFF; // A
            bmpPixels += 3;
        }
        // move bitmap pointer back to the start, minus 1 for the alignement offset
        bmpPixels -= bmpSize - 1;
    }
    stopwatchStop("Texture loop");
    SDL_UnlockTexture(packTexture);
    SDL_FreeSurface(packBitmap);
    return packTexture;
}

int numChars;
stbtt_pack_range* ranges;
SDL_Texture* packTexture;
float fontSizes[] = {12};
//float fontSizes[] = {8, 9, 10, 12, 14, 18};

bool initFont2(SDL_Renderer* renderer) {
    packTexture = loadPackCacheTexture(renderer, (const unsigned char*)rc_opensans_semibold.start, fontSizes, sizeof(fontSizes)/sizeof(fontSizes[0]), 512, 512, &numChars, &ranges);
    if (packTexture == NULL) return false;
    //SDL_RenderCopy(renderer, packTexture, NULL, &(SDL_Rect){ 0, 100, 512, 512*2*3});
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
        case 1: cp = text[0] & 0x7F; break;
        case 2: cp = text[0] & 0x1F; break;
        case 3: cp = text[0] & 0x0F; break;
        case 4: cp = text[0] & 0x07; break;
    }
    for(int i = 1; i < size; ++i)
        cp = (cp << 6) | (Uint32)(text[i] & 0x3F);
    return cp;
}

bool drawText2(SDL_Renderer* renderer, unsigned char* text, float x, float y, int maxW, int maxH, SDL_Color textColor, int sizeEm) {
    int sx, sy, dx, dy, cpSz, cp, w, h, iSz = -1, xStart = x;
    SDL_Rect packRect, packRectInv, dstRect;
    stbtt_packedchar pc;
    stopwatchStart();
    // find the font range for the specified font size
    for (int i = 0; i < sizeof(fontSizes)/sizeof(fontSizes[0]); i++)
        if ((int)fontSizes[i] == sizeEm) iSz = i;
    if (iSz == -1) return false;
    stbtt_pack_range range = ranges[iSz];

    x += 0.5f; y += 0.5f;
    // render the text
    while (*text) {
        cpSz = getCodepointSize(text);
        cp = (cpSz > 1) ? (int)getCodepoint(text, cpSz) : *text;

        if (cp > numChars) cp = ' ';
        pc = range.chardata_for_range[cp];

        w = (pc.x1 - pc.x0) / 3 + 1;
        h = pc.y1 - pc.y0;
            
        dx = x + pc.xoff;
        dy = y + pc.yoff;
        sx =          pc.x0 / 3;
        sy = pc.y0 + (pc.x0 % 3) * 512;

        if (x - xStart > maxW) return true;

        packRect = (SDL_Rect){ sx, sy, w, h};
        packRectInv = (SDL_Rect){ sx, sy + 512 * 3, w, h};
        dstRect = (SDL_Rect){ dx, dy, w, h};
            
        SDL_SetTextureBlendMode(packTexture, SDL_BLENDMODE_MOD);
        SDL_RenderCopy(renderer, packTexture, &packRectInv, &dstRect);
        SDL_SetTextureColorMod(packTexture, textColor.r, textColor.g, textColor.b);
        SDL_SetTextureBlendMode(packTexture, SDL_BLENDMODE_ADD);
        SDL_RenderCopy(renderer, packTexture, &packRect, &dstRect);

        x += pc.xadvance;
        text += cpSz;
    }
    stopwatchStop("text loop");

    return true;
}

bool getTextDimensions(unsigned char* text, int* width, int* height, int sizeEm) {
    int cpSz, cp, w, h, iSz = -1, x = 0;
    SDL_Rect packRect, packRectInv, dstRect;
    stbtt_packedchar pc;
    // find the font range for the specified font size
    for (int i = 0; i < sizeof(fontSizes)/sizeof(fontSizes[0]); i++)
        if ((int)fontSizes[i] == sizeEm) iSz = i;
    if (iSz == -1) return false;
    stbtt_pack_range range = ranges[iSz];

    while (*text) {
        cpSz = getCodepointSize(text);
        cp = (cpSz > 1) ? (int)getCodepoint(text, cpSz) : *text;

        if (cp > numChars) cp = ' ';
        pc = range.chardata_for_range[cp];

        w = (pc.x1 - pc.x0) / 3 + 1;
            
        *width = x + pc.xoff + w;
        if (pc.y1 > *height) *height = pc.y1;

        x += pc.xadvance;
        text += cpSz;
    }

    return true;
}

#endif