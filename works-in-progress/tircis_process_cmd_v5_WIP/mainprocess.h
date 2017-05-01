/** 
 * mainprocess
 **/
#ifndef MAINPROCESS_H
#define MAINPROCESS_H


#include "wavecal.h"
#include "geneprocess.h"
#include "suchi_offsets.h"
#include "suchifile.h"
/**
 * MainProcess class
 * @brief Main processing class for Tircis_Process software.
 * @section DESCRIPTION
 * This class contains the members and methods to take the Tircis 
 * scan file from raw image to a radiance file, an emissivity file,
 * and a temperature file.
 */
class MainProcess 
{
public:
	/**
	 * Constructor called by main.cpp, initializes and creates and
	 * displays the GUI.
	 */
    explicit MainProcess();
    Wavecal *wc ;
    GeneProcess *gp ;
    suchi_offsets *so ;
    SuchiFile *sf ;
	vector <float> temps ;
	vector <string> waveNames ;
	char workdir[420], bbHotFile[420], bbColdFile[420], scanFile[420] ;
    float tHot, tCold ;
    float xoffAvg, yoffAvg, *xoffArr, *yoffArr ;
    float startBin, stopBin ;
    bool lFlag, constFlag ;
    void readProcessFile (char * filename) ;
    void setBlackBodies (char *bbcoldFile, char *bbhotFile, float TCold, float THot) ;
    void run () ;

};

#endif // MAINPROCESS_H
