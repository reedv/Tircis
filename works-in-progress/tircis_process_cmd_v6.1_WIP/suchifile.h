#ifndef SUCHIFILE_H
#define SUCHIFILE_H
#include <vector>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

using namespace std ;

class SuchiFile 
{
public:
    SuchiFile(const char *) ;
    ~SuchiFile () ;
    void readSuchiMeta (char *ifile) ;
    void flattenData (unsigned short *indat, float *outdat) ;
    void flattenData () ;
    void readSuchiData() ;

    void getSuchiData (char * ifile, unsigned short *) ;
    void getRawProfile (vector<float> &outdat, int start, int stop) ;
    float getMagFFTProfile(vector<float> &out, int startcol, int stopcol) ;
    float getMagFFTProfile(vector<float> &out, int startcol, int stopcol, int startrow, int stoprow, float *maxloc) ;
    float getMagFFTProfileZoomed (vector<float> &out, int startcol,
                                       int stopcol,  float startrow, float endrow, float *maxloc) ;
    int ns, nl, nbands ;
    unsigned short *indat ;

    float *flatdat ;
    char datafile [420] ;

    
    
};

#endif // SUCHIFILE_H
