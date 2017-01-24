#ifndef SUCHI_UTILS_H
#define SUCHI_UTILS_H

#endif // SUCHI_UTILS_H
extern void polyfit (double *x, double *y, int npts, int degree, double *coefs, double *yf) ;
#include <fftw3.h>
#include "Instrument_config.h"

namespace suchi_utils
{

    void flattenData (unsigned short *in, float *out, int ns, int nl, int nbands) ;
    void extractAvgProfile (unsigned short *in, float *out, int ns, int nl,
                        int nbands, int startcol, int endcol) ;
    void extractFracProfile (unsigned short *in, float *out, int ns, int nl,
                             int nbands, int startcol, int endcol, float startLoc, float endLoc) ;
    int extractSegment (float *indat, float *segm, int npts_in, float startsamp, float endsamp) ;
    int procColumn (float *indat, float *outline, float startl, float endl, bool leftFlag ) ;
    int procColumn_noFFT (float *indat, float *outline, float startl, float endl, bool leftFlag ) ;
    void fftZoom (float *inarr, float *outarr, int zmfac, int npts) ;
    void apodize (float *apod, int nsamps) ;
    void remove3D (double *, int npts) ;
    void fftcall (double *indat, double *outdat, int npts, int dir) ;
    void fftcallComplex (double *indat, double *outdat, int npts, int dir) ;
	void complex_components (double *indat, double *real, double *imag, int npts) ;
    void getMag (double *in, float *mag, int npts) ;
    void getMagTan (double *in, float *mag, int npts) ;
    float getMax (float *mag, int npts, int *sub ) ;
    float getMaxSubpix (float *mag, int npts, float *maxloc, int starts, int ns) ;
    void calcMedian (float *xarr, int npts, float *xout) ;
    void calcMedian (float *xyarr, int npts, float *xout, float *yout) ;
    float bb2rad (float temp, float wave) ;
    float bb2temp (float rad, float wave) ;

}
