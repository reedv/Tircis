#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

#ifndef SUCHI_OFFSETS_H
#define SUCHI_OFFSETS_H


class suchi_offsets  
{
public:
    explicit suchi_offsets();
    void defineWindow (int x0, int y0, int nx, int ny, int samps, int lines) ;
    void calcOffsets (unsigned short *idat, float *xoff, float *yoff, int startFrame, int numFrames) ;
    void setConstantFlag (bool b) ;
    void resampleArray (unsigned short *indat, float xoff, float yoff, int numFrames) ;
    void resampleArray (unsigned short *indat, float *xoff, float *yoff, int numFrames) ;
    void setOutprefix (char *) ;
    unsigned short *outarr ;
    float ave_x, ave_y, *xoffArr, *yoffArr ;
    int nbands, startx, starty, nswin, nlwin, nsamps, nlines ;
    long totFrames ;
    bool constOffsetFlag ;
    char outPrefix[420] ;
    
};

#endif // SUCHI_OFFSETS_H
