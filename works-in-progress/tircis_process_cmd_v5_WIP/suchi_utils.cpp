
#include "suchi_utils.h"
#include "math.h"

namespace suchi_utils {


void flattenData (unsigned short *in, float *out, int ns, int nl, int nbands) {
    int i, j, ib ;

    float total ;

    for (i=0; i<nl; i++) {
        for (j=0; j<ns ; j++){
            total = 0. ;
            for (ib=0; ib<nbands; ib++) {
                total += in[ib * ns * nl + i * ns + j] ;
            }
            out[i*ns+j] = total /(float) nbands;
        }
    }



}

void extractAvgProfile (unsigned short *in, float *out, int ns, int nl,
                        int nbands, int startcol, int endcol) {
    int i, j ;
    int ncols  = endcol - startcol + 1 ;
    float *outtmp = new float [ns * nl] ;

    flattenData (in,outtmp,ns, nl, nbands) ;
    float total ;
    for (i=0; i<nl; i++) {
            total = 0. ;
            for (j=startcol; j<endcol+1; j++) {
                total += outtmp [i*ns+j] ;

            }
            out[i] = total / (float) ncols ;
    }



    delete [] outtmp ;

}

/// extractFracProfile - extracts the flattened profile, but then does the
/// fractional interpolation based upon the specified start and stop coordinates
/// A zoom factor of 10 is hardwired and the fractional coordinates are multiplied
/// by ten to figure out which start and end samples for the output profile
void extractFracProfile (unsigned short *in, float *out, int ns, int nl,
                         int nbands, int startcol, int endcol, float startLoc, float endLoc) {

    int i, startSamp, endSamp ;
    startSamp = (int) (startLoc * 20. + 0.5) ;
    endSamp = (int) (endLoc * 20. + 0.5) ;

    float *outtmp = new float [nl];
    float *outtmpZm = new float [nl * 10] ;
    // first call the extractAvgProfile to flatten the array and get the raws profile
    extractAvgProfile (in, out, ns, nl, nbands, startcol, endcol) ;

    // once we have the raw array, then get the zoomed array
    fftZoom (out, outtmpZm, 20, nl) ;
    i = startSamp ;
    while (i<=endSamp) {
        out[(i-startSamp)/20] = outtmpZm [i] ;
        i+= 20 ;
    }

    delete [] outtmp ;
    delete [] outtmpZm ;

}

// this does the extraction of the profile, then flips it and concatenates,
// the leftFlag is a remnant of the situation when we were flipping about the
// x axis, with the leftFlag meaning that the extracted portion would be placed on
// the left of the output array, then flip that portion and concatenate to the left
int procColumn_noFFT (float *indat, float *outline, float startl, float endl, bool leftFlag ) {


           int i, npts2, iloc, npts ;
           double *tmp ;



                npts = int(endl-startl+1.5) ;

                float *segm = new float[npts+10] ;

                npts2 = extractSegment (indat, segm, nl_inst, startl, endl) ;

                //npts2 = int(endl - startl + 1) ;
                npts = 2 * npts2 ;
                tmp = new double [npts] ;
                if (leftFlag) {
                    for (i=0; i<npts2; i++) {
                        //tmp[i] = indat[(startl+i)] ;
                        tmp[i]=segm[i] ;
                    }
                    for (i=npts2; i<npts; i++){
                        //iloc = endl - (i - npts2) ;
                        //tmp[i] = indat [iloc] ;
                        tmp[i]=segm[npts2-i] ;
                    }


                }
                else {
                    for (i=0; i<npts2; i++) {
                        tmp[i]=segm[npts2-i-1] ;
                        //iloc = endl - i ;
                        //tmp[i] = indat[iloc] ;
                    }
                    for (i=npts2; i<npts; i++){
                        tmp[i]=segm[i-npts2] ;
                        //iloc = startl+ i-npts2 ;
                        //tmp[i] = indat[iloc] ;
                    }
                }





                // remove 3D
                suchi_utils::remove3D (tmp, npts) ;

                // apodize
                float *apod = new float [npts] ;
                apodize (apod, npts) ;
                for (i=0; i<npts; i++){
                    outline[i]= tmp[i] * apod[i] ;

                }




    delete [] tmp ;
    delete [] segm ;
    delete [] apod ;

    return (npts) ;
}



int extractSegment (float *indat, float *segm, int npts_in, float startsamp, float endsamp) {

    int count=0, samp, ss, es ;
    float *outtmp = new float [npts_in * 10] ;
    fftZoom (indat, outtmp, 10, npts_in) ;
    ss = int(startsamp *  10) ;
    es = int(endsamp * 10) ;

    for (samp=ss; samp<=es; samp+= 10) {
        segm[count++]=outtmp[samp]/npts_in ;
    }
    return (count) ;
}

/*
// this does the extraction of the profile, then flips it and concatenates,
// the leftFlag is a remnant of the situation when we were flipping about the
// x axis, with the leftFlag meaning that the extracted portion would be placed on
// the left of the output array, then flip that portion and concatenate to the left
int procColumn (float *indat, float *outline, int startl, int endl, bool leftFlag ) {


           int i, npts2, iloc, npts ;
           double *tmp ;


                float *zmdat = new float [512*4] ;
                fftZoom (indat, zmdat, 4, 512) ;
                //FILE *fout = fopen ("/home/harold/rawdat","w") ;
                //fwrite ((char *)indat, 4, 512, fout) ;
                //fclose (fout) ;

                //fout = fopen ("/home/harold/rawzm","w") ;
                //fwrite ((char *)zmdat, 4, 512*4, fout) ;
                //fclose (fout) ;


                npts2 = endl - startl + 1 ;
                npts = 2 * npts2 ;
                tmp = new double [npts] ;
                double *outfft = new double [npts+2] ;
                if (leftFlag) {
                    for (i=0; i<npts2; i++) {
                        tmp[i] = indat[(startl+i)] ;
                    }
                    for (i=npts2; i<npts; i++){
                        iloc = endl - (i - npts2) ;
                        tmp[i] = indat [iloc] ;
                    }


                }
                else {
                    for (i=0; i<npts2; i++) {
                        iloc = endl - i ;
                        tmp[i] = indat[iloc] ;
                    }
                    for (i=npts2; i<npts; i++){
                        iloc = startl+ i-npts2 ;
                        tmp[i] = indat[iloc] ;
                    }
                }

                // remove 3D
                suchi_utils::remove3D (tmp, npts) ;

				// apodize
                float *apod = new float [npts] ;
                apodize (apod, npts) ;
                for (i=0; i<npts; i++){
                    outline[i]= tmp[i] * apod[i] ;
                    tmp[i] = outline[i] ;
                }

               fftcall (tmp, outfft, npts, -1) ;
               getMag (outfft, outline, npts) ;


    delete [] tmp ;
    delete [] outfft ;
    return (80) ;
}

*/
// this does the extraction of the profile, then flips it and concatenates,
// the leftFlag is a remnant of the situation when we were flipping about the
// x axis, with the leftFlag meaning that the extracted portion would be placed on
// the left of the output array, then flip that portion and concatenate to the left
int procColumn (float *indat, float *outline, float startl, float endl, bool leftFlag ) {


           int i, npts2, iloc, npts ;
           double *tmp ;


                //float *zmdat = new float [512*4] ;
                //fftZoom (indat, zmdat, 4, 512) ;
                //FILE *fout = fopen ("/home/harold/rawdat","w") ;
                //fwrite ((char *)indat, 4, 512, fout) ;
                //fclose (fout) ;

                //fout = fopen ("/home/harold/rawzm","w") ;
                //fwrite ((char *)zmdat, 4, 512*4, fout) ;
                //fclose (fout) ;
                float *segm = new float[int(endl-startl+1)+1] ;

                npts2 = extractSegment (indat, segm, nl_inst, startl, endl) ;

                //npts2 = int(endl - startl + 1) ;
                npts = 2 * npts2 ;
                tmp = new double [npts] ;
                double *outfft = new double [npts+2] ;
                if (leftFlag) {
                    for (i=0; i<npts2; i++) {
                        //tmp[i] = indat[(startl+i)] ;
                        tmp[i]=segm[i] ;
                    }
                    for (i=npts2; i<npts; i++){
                        //iloc = endl - (i - npts2) ;
                        //tmp[i] = indat [iloc] ;
                        tmp[i]=segm[i-npts2] ;
                    }


                }
                else {
                    for (i=0; i<npts2; i++) {
                        tmp[i]=segm[npts2-i-1] ;
                        //iloc = endl - i ;
                        //tmp[i] = indat[iloc] ;
                    }
                    for (i=npts2; i<npts; i++){
                        tmp[i]=segm[i-npts2] ;
                        //iloc = startl+ i-npts2 ;
                        //tmp[i] = indat[iloc] ;
                    }
                }

                // remove 3D
                suchi_utils::remove3D (tmp, npts) ;

                // apodize
                float *apod = new float [npts] ;
                apodize (apod, npts) ;
                for (i=0; i<npts; i++){
                    outline[i]= tmp[i] * apod[i] ;
                    tmp[i] = outline[i] ;
                }

               fftcall (tmp, outfft, npts, -1) ;
               getMag (outfft, outline, npts) ;


    delete [] tmp ;
    delete [] outfft ;
    delete [] segm ;
    return (npts) ;
}



void apodize (float *apod, int nsamps) {
    int i, nsamps2 ;
    float val ;
    nsamps2 = nsamps / 2 ;
    for (i=0; i<nsamps2; i++){
        val = float(i) / float(nsamps2-1.) ;
        *(apod+i) = val ;
    }
    for (i=0;i<nsamps2; i++){
        *(apod+i+nsamps2) = *(apod+nsamps2-1-i) ;
    }


}

// inplace 3d removal of trend
void remove3D (double *inarr, int npts){

    int i ;
    double *xind = new double [npts] ;
    double *tmp0 = new double [npts] ;
    double *coefs = new double [4] ;
    for (i=0; i<npts; i++) xind[i]=i ;

    polyfit (xind, inarr, npts, 3, coefs, tmp0);
    for (i=0; i<npts; i++) {
        *(inarr+i ) -= tmp0[i] ;
    }
    delete [] tmp0 ;
    delete [] xind ;

}

// call of fftw, output is 'out' and is complex npts array
void fftcall (double *indat, double *out, int npts, int dir){
    fftw_plan fplan, iplan ;
    switch (dir){
    case (-1) :
        fplan = fftw_plan_dft_r2c_1d (npts, indat, (fftw_complex *)&out[0], FFTW_ESTIMATE) ;
        fftw_execute (fplan) ;
        fftw_destroy_plan (fplan) ;
        break ;
    case (1) :
        iplan = fftw_plan_dft_c2r_1d (npts, (fftw_complex *)indat, (double *)&out[0], FFTW_ESTIMATE) ;
        fftw_execute (iplan) ;
        fftw_destroy_plan (iplan) ;
        break ;
    }

}

// complex fft
void fftcallComplex (double *indat, double *outdat, int npts, int dir){
    int N = npts ;
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) &indat[0] ;
    out = (fftw_complex*) &outdat [0] ;

    if (dir == -1){
        p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

        fftw_execute(p); /* repeat as needed */
    }
    else {
        p = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);

        fftw_execute(p); /* repeat as needed */
    }

    fftw_destroy_plan(p);
    //fftw_free(in); fftw_free(out);
}

// complex to real and imaginary output
// use in conjunction with fftcall to get the real and imag components of the fft (useful for the
// gene) routine
void complex_components (double *indat, double *real, double *imag, int npts) {
	int i ;




	for (i=0; i<npts; i++){
    	real [i] = *(indat+i*2) ;
    	imag [i] = *(indat+i*2+1) ;
	}

}



void getMag (double *in, float *mag, int npts) {
    int i ;
    float magval ;
    for (i=0; i<npts; i++){
        magval = sqrt(*(in+i*2) * *(in+i*2) + *(in+i*2+1) * *(in+i*2+1)) ;
        mag[i] = magval ;

    }
}

void getMagTan (double *in, float *mag, int npts) {
    int i ;
    double reVal, imVal, theta, val ;

    for (i=0; i<npts; i++){
        reVal = in[i*2] ;
        imVal = in[i*2+1] ;
        theta = atan2 (imVal, reVal) ;
        val = (reVal * cos (theta) + imVal * sin(theta)) ;
        mag[i] = (val>0.) ? (val) : (0.) ;

    }

}

float getMax (float *mag, int npts, int *sub){

    int i ;
    float maxval = -1.E9 ;
    *sub=-1 ;

    for (i=0; i<npts; i++) {
        if (mag[i]>maxval) {
            maxval = mag[i] ;
            *sub = i ;
        }
    }
    return maxval ;
}

/// getMaxSubpix - finds the subpixel max in the array
/// maxloc is the subpixel location starting from element 0 of the input
///
float getMaxSubpix (float *mag, int npts, float *maxloc, int starts, int ns){

    int i, maxpix ;
    int nslong  ;
    float maxval =-1.E9 ;
    double pixval ;


    fftw_complex *in, *out, *inbig, *outbig ;

    fftw_plan iplan, fplan ;
    ns = npts ;
    nslong = ns * 8 ;
    in = (fftw_complex *) fftw_malloc (sizeof(fftw_complex) * ns) ;
    out = (fftw_complex *) fftw_malloc (sizeof(fftw_complex) * ns) ;
    inbig = (fftw_complex *) fftw_malloc (sizeof(fftw_complex) * nslong) ;
    outbig = (fftw_complex *) fftw_malloc (sizeof(fftw_complex) * nslong) ;
    float *magbig = new float [nslong] ;



    for (i=0; i<ns; i++) {
        in[i][0] = mag[i+starts] ;
        in[i][1] = 0. ;

    }

    fplan = fftw_plan_dft_1d (ns, in, out, FFTW_FORWARD, FFTW_ESTIMATE ) ;
    fftw_execute (fplan) ;
    fftw_destroy_plan(fplan) ;
    for (i=0; i<nslong; i++) {
        outbig[i][0] = outbig[i][1] = 0. ;
        inbig[i][0] = inbig[i][1] = 0. ;
    }
    for (i=0; i<ns/2; i++){
        inbig[i][0] = out[i][0] ;
        inbig[i][1] = out[i][1] ;
        inbig[nslong-ns/2+i][0] = out[i+ns/2][0] ;
        inbig[nslong-ns/2+i][1] = out[i+ns/2][1] ;

    }
    iplan = fftw_plan_dft_1d (nslong, inbig, outbig, FFTW_BACKWARD, FFTW_ESTIMATE ) ;
    fftw_execute (iplan) ;
    fftw_destroy_plan (iplan) ;

    getMagTan ((double *)outbig, magbig, nslong);
    for (i=0; i<nslong; i++) {
        //pixval = outbig[i][0]*outbig[i][0] + outbig[i][1] * outbig[i][1] ;

        pixval = magbig [i] ;
        if (pixval > maxval) {
            maxpix = i ;
            maxval = pixval ;
        }
    }



    *maxloc = starts + float(maxpix) / 8. ;
    fftw_free (in) ;
    fftw_free (out) ;
    return (maxval) ;

}

void fftZoom (float *inarr, float *outarr, int zmfac, int npts) {
    int i, nptszm, outloc ;
    nptszm = npts * zmfac ;

    fftw_complex *in = new fftw_complex [npts] ;
    fftw_complex *out = new fftw_complex [npts] ;


    // complex needs to be npts/2+1 double complex samples
    fftw_complex  *inzm = new fftw_complex [nptszm] ;
    fftw_complex *outzm = new fftw_complex [nptszm] ;

    for (i=0; i<npts; i++) {
        in[i][0] = inarr[i] ;
        in[i][1] = 0. ;
    }
    for (i=0; i<nptszm; i++) {
        outzm[i][0] = 0. ;
        outzm[i][1]=0. ;
    }

    fftcallComplex ((double *) in, (double *) out, npts, -1) ;

    for (i=0; i<npts/2; i++) {

        outzm [i][0] = out[i][0] ;
        outzm [i][1] = out[i][1] ;


    }

    for (i=npts/2; i< npts; i++){
        outloc = nptszm - npts + i ;
        outzm[outloc][0]= out[i][0] ;
        outzm[outloc][1]= out[i][1] ;
    }

    fftcallComplex ((double *) outzm, (double *) inzm, nptszm, 1) ;
    getMag ((double *) inzm, outarr, nptszm) ;
    for (i=0; i<nptszm;i++) outarr[i]/=nptszm ;
    delete [] inzm ;
    delete [] out ;
    delete [] outzm ;
    delete [] in ;
    return ;

}


// given temp (celcius) and wavelength (microns) return radiance
float bb2rad (float temp, float wave){
    double val, tval ;

    tval = temp + 273.15 ;
    val = 1.191066E8/ pow(wave,5) / (exp(1.4388E4/(wave*tval))-1.) ;
    return (float) val ;


}


float bb2temp (float rad, float wave){
    double k, k2, k1, emiss=1., val ;
    double h, c ;
    k = 1.38066e-23 ;
    c = 2.99793E8 ;
    h = 6.626068E-34 ;

    k1 = 119.104E7 ;
    //
    // convert microns to meters
    wave = wave / 1000000. ;
    // convert rad to m-3 rather than m-2 microns
    rad = rad * 1E6 ;
    k2 = h * c / (k * wave) ;
    k1 = 2. * h * c * c / pow(wave,5.) ;

    val = log ((emiss * k1)/(rad)+1.) ;
    val = k2 / val - 273.15 ;
    return (float(val)) ;


}

void calcMedian (float *xyarr, int npts, float *xout, float *yout){

    int i, is, inew ;
    float fval, yval ;
    float *valArr = new float [npts] ;
    float *yvalArr = new float [npts] ;

    for (i=0; i<npts; i++) {
        valArr [i]= -999. ;
        yvalArr[i] = -999. ;
    }

    for (i=0; i<npts; i++){
        fval = *(xyarr+i*2) ;
        yval = *(xyarr+i*2+1) ;
        for (is=0; is<npts; is++) {
            if (fval > valArr[is]){
                for (inew=npts-1; inew> is; inew--){
                    valArr[inew] = valArr [inew-1];
                    yvalArr [inew] = yvalArr [inew-1] ;
                }
                valArr[is] = fval ;
                yvalArr [is] = yval ;
                break ;
            }

        }
    }


    *xout = valArr [(int)(npts/2)] ;
    *yout = yvalArr [(int)(npts/2)] ;
    delete [] valArr ;
    delete [] yvalArr ;
}


void calcMedian (float *xarr, int npts, float *xout){

    int i, is, inew ;
    float fval ;
    float *valArr = new float [npts] ;

    for (i=0; i<npts; i++) valArr [i]= -999. ;

    for (i=0; i<npts; i++){
        fval = *(xarr+i) ;
        for (is=0; is<npts; is++) {
            if (fval > valArr[is]){
                for (inew=npts-1; inew> is; inew--){
                    valArr[inew] = valArr [inew-1];
                }
                valArr[is] = fval ;
                break ;
            }

        }
    }


    *xout = valArr [(int)(npts/2)] ;
    delete [] valArr ;
}




}
