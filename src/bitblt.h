//
//  bitblt.h
//  Smalltalk-80
//
//  Created by Dan Banay on 2/20/20.
//  Copyright © 2020 Dan Banay. All rights reserved.
//

#pragma once
#include "objmemory.h"
#include <cstdint>


//BItBlt
static const int DestFormIndex = 0;
static const int SourceFormIndex = 1;
static const int HalftoneFormIndex = 2;
static const int CombinationRuleIndex = 3;
static const int DestXIndex = 4;
static const int DestYIndex = 5;
static const int WidthIndex = 6;
static const int HeightIndex = 7;
static const int SourceXIndex = 8;
static const int SourceYIndex = 9;
static const int ClipXIndex = 10;
static const int ClipYIndex = 11;
static const int ClipWidthIndex = 12;
static const int ClipHeightIndex = 13;


// BitBltSimulation
class BitBlt
{
public:
    
    BitBlt(ObjectMemory &memory,
           int destForm,
           int sourceForm,
           int halftoneForm,
           int combinationRule,
           int destX,
           int destY,
           int width,
           int height,
           int sourceX,
           int sourceY,
           int clipX,
           int clipY,
           int clipWidth,
           int clipHeight):
                memory(memory),
                destForm(destForm),
                sourceForm(sourceForm),
                halftoneForm(halftoneForm),
                combinationRule(combinationRule),
                destX(destX),
                destY(destY),
                width(width),
                height(height),
                sourceX(sourceX),
                sourceY(sourceY),
                clipX(clipX),
                clipY(clipY),
                clipWidth(clipWidth),
                clipHeight(clipHeight)
    
    {
        const int WidthInForm = 1;
        const int HeightInForm = 2;

        if (sourceForm != NilPointer)
        {
            sourceFormWidth = memory.integerValueOf(memory.fetchWord_ofObject(WidthInForm, sourceForm));
            sourceFormHeight = memory.integerValueOf(memory.fetchWord_ofObject(HeightInForm, sourceForm));
        }
        else
        {
            sourceX = sourceY = 0;
            sourceFormHeight = sourceFormWidth = 0;
        }
        destFormWidth = memory.integerValueOf(memory.fetchWord_ofObject(WidthInForm, destForm));
        destFormHeight = memory.integerValueOf(memory.fetchWord_ofObject(HeightInForm, destForm));
 
        updatedX = 0;
        updatedY = 0;
        updatedWidth = 0;
        updatedHeight = 0;


    }


    void copyBits();

    void getUpdatedBounds(int *boundsX, int *boundsY, int *boundsWidth, int *boundsHeight)
    {
        *boundsX = updatedX;
        *boundsY = updatedY;
        *boundsWidth = updatedWidth;
        *boundsHeight = updatedHeight;
    }
    
protected:
    
    /* "source"
     "initialize a table of bit masks ... p.356"
     RightMasks <-
         #(0    16r1 16r3 16r7 16rF
             16r1F 16r3F 16r7F 16rFF
             16r1FF 16r3FF 16r7FF 16rFFF
             16r1FFF 16r3FFF 16r7FFF 16rFFFF).
     AllOnes <- 16rFFFF
    */

    static constexpr std::uint16_t RightMasks[] = {
        0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
        0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF
    };
    
    static const std::uint16_t  AllOnes = 0xFFFF;
    
     // clipRange
     void clipRange();

     // merge:with:
    inline std::uint16_t merge_with(std::uint16_t sourceWord, std::uint16_t destinationWord)
    {
        /* "source"
         "These are the 16 combination rules:"
         combinationRule = 0
         ifTrue: [^0].
         combinationRule = 1
         ifTrue: [^sourceWord bitAnd: destinationWord].
         combinationRule = 2
         ifTrue: [^sourceWord bitAnd: destinationWord bitInvert].
         combinationRule = 3
         ifTrue: [^sourceWord].
         combinationRule = 4
         ifTrue: [^sourceWord bitInvert bitAnd: destinationWord].
         combinationRule = 5
         ifTrue: [^destinationWord].
         combinationRule = 6
         ifTrue: [^sourceWord bitXor: destinationWord].
         combinationRule = 7
         ifTrue: [^sourceWord bitOr: destinationWord].
         combinationRule = 8
         ifTrue: [^sourceWord bitInvert bitAnd: destinationWord bitInvert].
         combinationRule = 9
         ifTrue: [^sourceWord bitInvert bitXor: destinationWord].
         combinationRule = 10
         ifTrue: [^destinationWord bitInvert].
         combinationRule = 11
         ifTrue: [^sourceWord bitOr: destinationWord bitInvert].
         combinationRule = 12
         ifTrue: [^sourceWord bitInvert].
         combinationRule = 13
         ifTrue: [^sourceWord bitInvert bitOr: destinationWord].
         combinationRule = 14
         ifTrue: [^sourceWord bitInvert bitOr: destinationWord bitInvert].
         combinationRule = 15
         ifTrue: [^AllOnes]
         */
        
        // These are the 16 combination rules:
        switch (combinationRule)
        {
            case  0
                : return 0;
            case  1 // [^sourceWord bitAnd: destinationWord]
                : return sourceWord & destinationWord;
                
            case  2 // [^sourceWord bitAnd: destinationWord bitInvert]
                : return sourceWord & (~destinationWord);
                
            case  3 // [^sourceWord]
                : return sourceWord;
                
            case  4 //  [^sourceWord bitInvert bitAnd: destinationWord]
                : return (~sourceWord) & destinationWord;
                
            case  5 // [^destinationWord]
                : return destinationWord;
                
            case  6 // [^sourceWord bitXor: destinationWord]
                : return sourceWord ^ destinationWord;
                
            case  7 // [^sourceWord bitOr: destinationWord]
                : return sourceWord | destinationWord;
                
            case  8 //  [^sourceWord bitInvert bitAnd: destinationWord bitInvert].
                : return (~sourceWord) & (~destinationWord);
                
            case  9 // [^sourceWord bitInvert bitXor: destinationWord].
                : return (~sourceWord) ^ destinationWord;
                
            case  10 //  [^destinationWord bitInvert]
                : return ~destinationWord;
                
            case  11 //  [^sourceWord bitOr: destinationWord bitInvert].
                : return sourceWord | (~destinationWord);
                
            case  12 // [^sourceWord bitInvert]
                : return ~sourceWord;
                
            case  13 //  [^sourceWord bitInvert bitOr: destinationWord]
                : return (~sourceWord) | destinationWord;
                
            case  14 // [^sourceWord bitInvert bitOr: destinationWord bitInvert].
                : return (~sourceWord) | (~destinationWord);
                
            case  15
                : return AllOnes;
            default:
                assert(0);
        }
        
        return 0;
    }

    // copyLoop
    void copyLoop();

    // calculateOffsets
    void calculateOffsets();

    // computeMasks
    void computeMasks();

    // checkOverlap
    void checkOverlap();

    ObjectMemory& memory;
    int destForm;
    int sourceForm;
    int halftoneForm;
    int combinationRule;
    int destX;
    int destY;
    int width;
    int height;
    int sourceX;
    int sourceY;
    int clipX;
    int clipY;
    int clipWidth;
    int clipHeight;
    
    // instance variables (pg. 356 G&R)
    int sourceBits, sourceRaster, destBits, destRaster, halftoneBits,
        skew,
        preload, nWords,
        hDir, vDir,
        sourceIndex, sourceDelta, destIndex, destDelta,
        sx, sy, dx, dy, w, h;
    
    //sourceRaster is the source pitch, and destRaster the destination pitch.
    //I guess these terms were unknown to the implementors.
    
    std::uint16_t mask1, mask2, skewMask;
    
    // Additional info
    int sourceFormWidth;
    int sourceFormHeight;
    int destFormWidth;
    int destFormHeight;
    
    // Actual area affected by last copyBots
    int updatedX, updatedY, updatedWidth, updatedHeight;
};

//Character Scanner
static const int LastIndexIndex = 14;
static const int XTableIndexIndex = 15;
static const int StopConditionsIndex = 16;
static const int TextIndex = 17;
static const int TextStyleIndex = 18;
static const int LeftMarginIndex = 19;
static const int RightMarginIndex = 20;
static const int FontIndex = 21;
static const int LineIndex = 22;
static const int RunStopIndexIndex = 23;
static const int SpaceCountIndex = 24;
static const int SpaceWidthIndex = 25;
static const int OutputMediumIndex = 26;

class CharacterScanner: public BitBlt
{
public:
    CharacterScanner(ObjectMemory &memory,
    int destForm,
    int sourceForm,
    int halftoneForm,
    int combinationRule,
    int destX,
    int destY,
    int width,
    int height,
    int sourceX,
    int sourceY,
    int clipX,
    int clipY,
    int clipWidth,
    int clipHeight,
                  
 
    int xTable,
    int lastIndex,
    int stopConditions) :
        BitBlt(memory,
               destForm,
               sourceForm,
               halftoneForm,
               combinationRule,
               destX,
               destY,
               width,
               height,
               sourceX,
               sourceY,
               clipX,
               clipY,
               clipWidth,
               clipHeight),
        xTable(xTable),
        lastIndex(lastIndex),
        stopConditions(stopConditions)
    {
     }
    

    // After scanning, internal state is updated for 3 variables which must be
    // written back to the object store. Here they are:
    int updateDestX() { return destX; }
    int updatedWidth() { return width; }
    int updatedSourceX() { return sourceX; }
    int updatedLastIndex() { return lastIndex; }
    
    // scanCharactersFrom: startIndex to: stopIndex in: sourceString rightX: rightX stopConditions: stops displaying: display

    
    int scanCharactersFrom_to_in_rightX_stopConditions_displaying(int startIndex, int stopIndex, int sourceString,
                                                                  int rightX, int stop, bool displaying);
    
    int stopConditions;
    int xTable;
    int lastIndex;
    
};

