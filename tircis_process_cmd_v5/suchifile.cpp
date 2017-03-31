#include "suchifile.h"
#include "suchi_utils.h"
#include "Instrument_config.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

SuchiFile::SuchiFile(const char *filename)
{
	int status ; 
    long nbytes ;
	struct stat filstat ;

    ns = ns_inst ;
    nl = nl_inst ;
	//FILE *fil = fopen (filename, "r") ;
	int fildes = open (filename, O_RDWR) ;
	if (fildes < 0) {
		printf ("Could not open %s\r\n", filename) ;
		return ;
	}
	status = fstat (fildes, &filstat) ;
	nbytes = filstat.st_size ;
    nbands = nbytes / (ns * nl * 2) ;
    strcpy (datafile, filename) ;
    flatdat = 0 ;
	//close (fildes) ;
}
SuchiFile::~SuchiFile () {

    if (flatdat) delete [] flatdat ;
    if (indat) delete [] indat ;
}


void SuchiFile::readSuchiMeta(char *ifile){

    int status ;
    // check if file exists
	FILE *fil = fopen (ifile, "r") ;
	if (fil == NULL ) {
		printf ("Problem with %s\r\n", ifile) ;
		return ;
	}
	fclose(fil) ;
    //

}



void SuchiFile::readSuchiData () {
    long npix ;
    npix = long(ns ) * nl * nbands ;
    indat = new unsigned short [npix] ;

	FILE *fil = fopen (datafile, "r") ;
	if (fil == NULL ) {
		printf ("Problem with %s\r\n", datafile) ;
		return ;
	}
	fread ((char *) indat, 2, npix, fil) ;
	fclose (fil) ;

}



void SuchiFile::getSuchiData (char *ifile, unsigned short *indata) {

	FILE *fil = fopen (ifile, "r") ;
	if (fil == NULL ) {
		printf ("Problem with %s\r\n", ifile) ;
		return ;
	}
	fread ((char *) indata, 2, long(ns) * nl * nbands, fil) ;
	fclose (fil) ;


}

void SuchiFile::flattenData() {
    if (flatdat) delete [] flatdat ;
    flatdat = new float [ns *nl] ;
    suchi_utils::flattenData (this->indat, flatdat, ns, nl, this->nbands) ;

}

void SuchiFile::flattenData (unsigned short *indat, float *outdat)
{

    suchi_utils::flattenData (indat, outdat, ns, nl, this->nbands) ;
}

void SuchiFile::getRawProfile (vector<float> &out, int startcol, int stopcol) {
    int i ;
    float *outdat = new float [nl] ;
    suchi_utils::extractAvgProfile (indat, outdat, ns, nl, nbands, startcol, stopcol) ;
    out.clear() ;
    for (i=0; i<nl; i++) out.push_back(outdat[i]) ;
    delete [] outdat ;
}


float SuchiFile::getMagFFTProfile(vector<float> &out, int startcol, int stopcol) {
    int i ;
    float maxval = -1. ;
    float *outdat = new float [nl] ;
    float *outtmp = new float [512] ;
    suchi_utils::extractAvgProfile (indat, outdat, ns, nl, nbands, startcol, stopcol) ;
    int npts = suchi_utils::procColumn (outdat, outtmp, 10, 133, 0 ) ;

    for (i=0; i<npts; i++) {
        out.push_back (outtmp[i] ) ;
        if (outtmp[i] > maxval) maxval = outtmp[i] ;
    }

    delete [] outdat ;
    delete [] outtmp ;

    return maxval ;


}

/// Returns the magnitude of the fft of the profile extracted based upon start and stop column as well
/// as start and stop row, profile returned in the out vector
float SuchiFile::getMagFFTProfile(vector<float> &out, int startcol,
                                  int stopcol, int startrow, int endrow, float *maxloc) {
    int i ;
    float *outdat = new float [nl] ;
    float *outtmp = new float [512] ;
    float maxval = -9999. ;
    suchi_utils::extractAvgProfile (indat, outdat, ns, nl, nbands, startcol, stopcol) ;
    int npts = suchi_utils::procColumn (outdat, outtmp, startrow, endrow, 0 ) ;
    maxval = suchi_utils::getMaxSubpix (outtmp, 80, maxloc, 0, 50) ;


    out.clear () ;
    for (i=startrow; i<=endrow; i++ ) {
        out.push_back (outtmp[i]) ;
    }

    return maxval ;


}


/// Re
float SuchiFile::getMagFFTProfileZoomed (vector<float> &out, int startcol,
                                   int stopcol, float startLoc, float stopLoc, float *maxloc) {

    int i, count = 0, numPtsSeg ;
    float *outdat = new float [512] ;
    float *outtmp = new float [512] ;
    float *outtmp1 = new float [512] ;
    float maxval = -9999. ;

    //
    //
    //QFile qf ("/home/harold/workdir/avprofs");
    //qf.open(QIODevice::WriteOnly) ;
    suchi_utils::extractAvgProfile (indat, outdat, ns, nl, nbands, startcol, stopcol) ;
    //qf.write ((char *)outdat, nl * 4) ;
    // first get the raw profile
    suchi_utils::extractFracProfile (indat, outdat, ns, nl, nbands, startcol, stopcol, startLoc, stopLoc) ;
    //qf.write ((char *)outdat, nl * 4) ;
    //qf.close() ;
    numPtsSeg = stopLoc - startLoc + 1 ;


    int npts = suchi_utils::procColumn (outdat, outtmp, 0, numPtsSeg-1, 0 ) ;
    maxval = suchi_utils::getMaxSubpix (outtmp, 80, maxloc, 0, 50) ;

    out.clear() ;
    for (i=0; i<numPtsSeg; i++) {
        out.push_back (outtmp[i] ) ;
        //if (outtmp[i] > maxval) maxval = outtmp[i] ;
    }
    return maxval ;


}
