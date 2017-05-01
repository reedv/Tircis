#ifndef WAVECAL_H
#define WAVECAL_H

#include "Instrument_config.h"
#include <vector>
#include <string>
#include <string.h>

using namespace std ;

class Wavecal 
{
public:
    explicit Wavecal();
    ~Wavecal() ;
    //void setWaveFiles (QList<QString>, QList<float>) ;
    void setWaveFiles (vector<string>, vector<float>) ;
    void setStartStop (int start, int stop, bool leftFlag) ;
    void setOutputFile (char *str) ;
    void setProcessLoc (int) ;
    void loadProfiles() ;
    void process () ;
    void processFull() ;
    void processFullArray() ;

    bool leftFlag ;
    double *pixvals, *wavenum, *wavecoefs, *yf ;
    float *profiles, *prof0, *prof1, *firstFile ;
    unsigned short  *wavedata ;
    int procLoc ;
    long npixTot ;
    char outfile[420] ;
	//char **filenames ;
	vector <string> filenames ;
	vector <float> wavelens ;
    int nfiles, start, stop, ns, nl ;
    
    
    
};

#endif // WAVECAL_H
