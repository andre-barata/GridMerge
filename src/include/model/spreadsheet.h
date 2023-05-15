/*
  grid_view.h: the model for a spredsheet file
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

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <stdlib.h>
#include <stdio.h>
#include "../thirdparty/minizip3/mz.h"
#include "../thirdparty/minizip3/mz_os.h"
#include "../thirdparty/minizip3/mz_strm.h"
#include "../thirdparty/minizip3/mz_strm_buf.h"
#include "../thirdparty/minizip3/mz_strm_split.h"
#include "../thirdparty/minizip3/mz_zip.h"
#include "../thirdparty/minizip3/mz_zip_rw.h"

typedef struct _Spreadsheet {
    void* zipReader;
    unsigned char** sharedStrings;
    unsigned int sharedStringsCount;
} Spreadsheet;
Spreadsheet* sheets = NULL;
unsigned short sheetNr = 0;

typedef struct _Cell {
    unsigned int row;
    unsigned short col;
    union data
    {
        char* string;
        int integer;
        float decimal;
        unsigned int date;
    };
} Cell;

// gets the string between two tokens: ...<header>(returned string)<tail>...
unsigned char* getContents(unsigned char* buffer, unsigned int size, unsigned char* head, unsigned char* tail, int* startAt) {
    unsigned char *pChar = buffer + *startAt, *pCharEnd = buffer + size, *pHeaderMatch, *pHead = head, *pTail = tail;
    int tailLen = strlen(tail);
    while (pChar < pCharEnd) {
        if (*pChar++ != *pHead++) pHead = head; // if it doesn't match check the next buffer char against the first header char
        if (*pHead) continue;
        // found header
        pHeaderMatch = pChar;
        while (pChar < pCharEnd) {
            if (*pChar++ != *pTail++) pTail = tail;
            if (*pTail) continue;
            // found trail
            int len = pChar - pHeaderMatch - tailLen;
            unsigned char* str = malloc(len+1);
            memcpy(str, pHeaderMatch, len);
            str[len] = 0;
            *startAt = pChar - buffer;
            return str;
        }
    }
    return NULL;
}

// load shared strings. uses hardcoded literal chars for optimum performance. assumes wellformed xml without inter-tag whitespaces
bool loadSharedStrings(Spreadsheet* sheet, unsigned char* fileBuffer, unsigned int bufferSize) {
    stopwatchStart();
    // read the string count
    int startAt = 0, iString = 0;
    unsigned char* sCountInFile = getContents(fileBuffer, bufferSize, "uniqueCount=\"", "\"", &startAt),
        *pChar = fileBuffer + 158, // skip start tags
        *pFirstMatch = NULL;
    int countInFile = atoi(sCountInFile);
    sheet->sharedStrings = malloc(sizeof(void*) * countInFile);
    // pre-allocate all the strings in a single buffer
    unsigned char* stringPool = malloc(bufferSize - 154 /*xml header & root start take 154*/ - (16*countInFile) /*tag at least 16*/ 
                                        - 6 /*ending is 6*/ + (1*countInFile) /*space for null terminators*/);
    while (pChar < fileBuffer + bufferSize) {
        // loop back while we haven't found the starting tag
        if (*pChar++ != '<' || *pChar++ != 't') continue;
        // move pointer to end of the starting tag
        while (*pChar++ != '>');
        // save pointer position
        pFirstMatch = pChar;
        // move pointer to the end tag
        while (*pChar++ != '<' || *pChar++ != '/' || *pChar++ != 't' || *pChar++ != '>');
        //  copy the string to sharedStrings array
        int len = pChar - pFirstMatch - 4;
        memcpy(stringPool, pFirstMatch, len);
        stringPool[len] = 0;
        sheet->sharedStrings[iString] = stringPool;
        stringPool += len + 1;
        iString++;
    }
    stopwatchStop("shared strings");
    if (iString != countInFile || countInFile == 0) return false;
    sheet->sharedStringsCount = iString;
    return true;
}

bool loadSharedStringsFile(Spreadsheet* sheet, void* reader, unsigned char* zipFilePath, unsigned char* innerPath) {
    // set the shared strings path as a search pattern
    mz_zip_reader_set_pattern(reader, innerPath, 1);
    if (mz_zip_reader_open_file(reader, zipFilePath) != MZ_OK) return falseAndLog("could not open ZIP file %s\n", zipFilePath);
    // save the uncompressed file to a memory buffer
    unsigned int tmpBufferSize = mz_zip_reader_entry_save_buffer_length(reader);
    void* tmpBuffer = malloc(tmpBufferSize);
    if (mz_zip_reader_entry_save_buffer(reader, tmpBuffer, tmpBufferSize) != MZ_OK) return falseAndLog("could not Extract ZIP file %s to memory\n", innerPath);
    // load the strings
    if (!loadSharedStrings(sheet, tmpBuffer, tmpBufferSize)) return falseAndLog("failed to load shared strings\n");
    free(tmpBuffer);
    mz_zip_reader_close(reader);
    return true;
}

// cell example: <c r="B2" t="s"><v>2</v></c>
bool loadWorksheet(Spreadsheet* sheet, unsigned char* fileBuffer, unsigned int bufferSize) {
    // TODO: load cell data
    stopwatchStart();
    // read the string count
    int startAt = 0, iString = 0;
    unsigned char *pChar = fileBuffer + 158, // skip start tags
        *pFirstMatch = NULL;
    while (pChar < fileBuffer + bufferSize) {
        // loop back while we haven't found the starting tag
        if (*pChar++ != '<' || *pChar++ != 'c') continue;
        // move pointer to end of the starting tag
        while (*pChar++ != '>');
        // save pointer position
        pFirstMatch = pChar;
        // move pointer to the end tag
        while (*pChar++ != '<' || *pChar++ != '/' || *pChar++ != 'c' || *pChar++ != '>');
        //  copy the string to sharedStrings array
        int len = pChar - pFirstMatch - 4;
        iString++;
    }
    stopwatchStop("load sheet");
    return true;
}

bool loadWorksheetFile(Spreadsheet* sheet, void* reader, unsigned char* zipFilePath, unsigned char* innerPath, int index) {
    // set the sheet path as a search pattern
    mz_zip_reader_set_pattern(reader, innerPath, 1);
    if (mz_zip_reader_open_file(reader, zipFilePath) != MZ_OK) return falseAndLog("could not open ZIP file %s\n", zipFilePath);
    // save the uncompressed file to a memory buffer
    unsigned int tmpBufferSize = mz_zip_reader_entry_save_buffer_length(reader);
    void* tmpBuffer = malloc(tmpBufferSize);
    if (mz_zip_reader_entry_save_buffer(reader, tmpBuffer, tmpBufferSize) != MZ_OK) return falseAndLog("could not Extract ZIP file %s to memory\n", innerPath);
    // load the worksheet
    if (!loadWorksheet(sheet, tmpBuffer, tmpBufferSize)) return falseAndLog("failed to load the worksheet %s\n", innerPath);
    
    free(tmpBuffer);
    mz_zip_reader_close(reader);
    falseAndLog("done reading sheet %s\n", innerPath);
    return true;
}

bool loadSpreadsheet(unsigned char* filename) {
    // allocate a new sheet struct
    sheets = realloc(sheets, sizeof(Spreadsheet) * (sheetNr + 1));
    sheets->zipReader = NULL;

    // open zip file
    mz_zip_reader_create(&sheets[sheetNr].zipReader);

    // shared strings
    if (!loadSharedStringsFile(&sheets[sheetNr], sheets[sheetNr].zipReader, filename, "xl/sharedStrings.xml")) return false;
    
    if (!loadWorksheetFile(&sheets[sheetNr], sheets[sheetNr].zipReader, filename, "xl/worksheets/sheet2.xml", 0)) return false;

    // free string pool and reader
    free(sheets[sheetNr].sharedStrings[0]);
    mz_zip_reader_delete(&sheets[sheetNr].zipReader);

    sheetNr++;
}
#endif
