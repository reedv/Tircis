#ifndef GENEPROCESS_H
#define GENEPROCESS_H

#include <string>
#include <vector>
#include <string.h>
#include "suchi_utils.h"
#include "suchi_offsets.h"
#include "Instrument_config.h"
using namespace std ;
/*!
* \class GeneProcess
*  This class provides functionality specific to the Tircis
*  instrument.
*  \brief Main Processing class of the TIRCIS software
*  \author Harold Garbeil
*  \date December 15, 2015
*  \contact garbeil@hawaii.edu
*/



class GeneProcess 
{
public:
    explicit GeneProcess();
    ~GeneProcess () ;

    void readWaveFile (char *) ;
    void readBBFilesList (char *) ;
    void readBBFiles (char * hotfile, char * coldfile, float hotT, float coldT) ;
    void genBBCurves () ;
    void processIntColumn (float *outarr, int colno, int totFrames) ;
    void geneProfile (int colno, float *scene_rad, int bbnum) ;
    void geneFrame (unsigned short *indat, float *scene_rad) ;
    void getProfile (int colno,  float* outdata, int *npts, int bbnum) ;
    void getProfileSegmented (unsigned short *indat, int colno, float *data, int npts) ;
    void getProfileSegmented (int colno, float *outdata, int *npts, int bbnum) ;
    void getProfileSegmented (float *indat, int colno, float *data, int *npts) ;
    void getProfileFFTMag (int colno, float *data, int *npts, int bbnum) ;
    void setProfileSegment (int array_start, int array_end) ;
    void setSegParams (int array_start, int array_end, bool leftFlag) ;
    void setSegParamsNew (float yint, float slope, int npts, bool leftFlag) ;
    void setOutpref (char * ) ;
    void processBB () ;
    void calcEmissivity () ;
    void processCube (suchi_offsets *so) ;
    void findWaveRange () ;
    void readProcessFile (char *) ;
	void readSegmentFile (char *) ;

    vector<string> bbNameList ;
    vector<float> tempList ;
    char outPref[420], scanFile[420] ;
    float *wavenum, *wavelen, *bbPlanck, *tempArr ;
    float *hotimag, *coldimag, *hotreal, *coldreal ;
	float *segs ;
	float seg_yintcpt, seg_slope ;
    unsigned short *hotbbraw, *coldbbraw, *stacked  ;
    float *hotbb, *coldbb, *othersBB, *radArray, *emissArr ;
    int startwave, stopwave, nwaves, stopFinWave, startFinWave, nfinWaves, totFrames ;
    int nptsSeg, nsamps, nlines, ns, nl, nb , array_start, array_end, numBBOthers ;
	float startl, endl ;
    bool leftFlag ;


    
};

#endif // GENEPROCESS_H
