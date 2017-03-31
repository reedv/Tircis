#include "suchi_offsets.h"
#include "suchi_utils.h"
#include "cvprocess.h"
#include <omp.h>

using namespace std ;

suchi_offsets::suchi_offsets() 
{
    ave_x = 0. ;
    ave_y = 0. ;
    totFrames = 0 ;
    strcpy(outPrefix,"");
    nsamps = 324 ;
    nlines = 256 ;
    nbands = 0 ;
    xoffArr = 0;
    yoffArr = 0l ;
    defineWindow (10, 100, 180,138, nsamps, nlines) ;

}



void suchi_offsets::setConstantFlag (bool f){
    constOffsetFlag = f ;
}

void suchi_offsets::defineWindow (int x0, int y0, int nx, int ny, int nsamps, int nlines){
    startx = x0 ;
    starty = y0 ;
    nswin = nx ;
    nlwin = ny ;
    this->nsamps = nsamps ;
    this->nlines = nlines ;
}


void suchi_offsets::setOutprefix(char * opref) {
    strcpy (outPrefix, opref) ;
}

void suchi_offsets::calcOffsets (unsigned short *idat, float *xoff, float *yoff, int startFrame, int numFrames){

    int i, j,  js,  iFrame, refInc ;
    long il, npixtot ;
    unsigned short *iptr ;
    float *chip0, *chip1, xinc, yinc, x0, y0 ;

    int iterInc = 10 ;
    int nframesCalc = 0 ;
    nbands = numFrames ;
    npixtot = long(nsamps) * nlines ;
    xoffArr = new float [numFrames] ;
    yoffArr = new float [numFrames] ;
    //int nframes = 1 ;
    CVProcess *cv = new CVProcess () ;
    chip0 = new float [npixtot] ;
    chip1 = new float [npixtot] ;
    // running offset to keep track
    x0 = 0. ;
    y0 = 0. ;

    xoff[0] = 0. ;
    yoff[0] = 0. ;
    xoffArr[0] = yoffArr[0] = 0. ;
    refInc = 0 ;
    for (il=0; il <npixtot; il++){
        chip0 [il] = idat[il+startFrame * npixtot] ;
    }
    for (i=1; i< numFrames; i++){
        for (il=0; il <npixtot; il++){
            chip1 [il] = idat[(startFrame+i)*npixtot+il] ;
        }



        cv->loadFromFloatArray (chip0, chip1, nswin, nlwin, startx, starty, nsamps, nlines) ;
        if (cv->nMatchPoints > 3) {
            suchi_utils::calcMedian (cv->xyoffs, cv->nMatchPoints, &xinc, &yinc ) ;
            ave_x += xinc / (i - refInc) ;
            ave_y += yinc / (i - refInc) ;
            nframesCalc++ ;
        } else {
            xinc = 0. ;
            yinc = 0. ;
        }
        xoff[i] = xinc + xoff[refInc] ;
        yoff[i] = yinc + yoff[refInc] ;
        xoffArr[i] = xoff[i] ;
        yoffArr[i] = yoff[i] ;
        // check if you need to refresh the reference chip
        if (i%iterInc == 0) {
            refInc = i ;
            for (il=0; il <npixtot; il++) chip0[il] = chip1[il] ;

        }
        cout << "Number of match points : "<<  cv->nMatchPoints << endl ;


    }


    ave_x /= nframesCalc ;
    ave_y /= nframesCalc ;
}


void suchi_offsets::resampleArray (unsigned short *idat, float *xoff, float *yoff, int nframes){
    int i, j, is, js, samploc, nLines, newloc0 ; // iframe
    int xloc,yloc, zloc ;
    float xoffval ;
    float frac0, frac1, diff ;
    float magoff = fabs (ave_y * nframes )-1 ;
    int iLoc, lastis=0 ;
    totFrames = magoff + nlines  ;
    long npix = nsamps * nlines ;

    nLines = 0 ;
    if (totFrames > 800) totFrames = 800 ;

    // do the y offset, which is done on an individual frame
    if (ave_x > 0. || ave_x <0.)
    {
        unsigned short *tmpframe0 = new unsigned short [npix * totFrames] ;
        // first frame, no offsets applied
        for (i=0;i<npix; i++)*(tmpframe0+i)=*(idat+i) ;

        for (int iframe=0; iframe<totFrames; iframe++) {
            xoffval = xoff[iframe] ;
            xloc = int(xoffval) ;
            frac0 = 1. - (xoffval-xloc) ;
            frac1 = 1. -frac0 ;
            for (is=0; is<nlines; is++){

                for (js=0; js<nsamps; js++){
                    newloc0 = js + xloc ;
                    if (newloc0 < 0) newloc0 = 0 ;
                    if (newloc0 >= nsamps-2) newloc0 = nsamps-2 ;
                    *(tmpframe0+iframe*npix+is*nsamps+js) =
                            frac0 * *(idat+iframe*npix+is * nsamps+newloc0)+
                            frac1 * *(idat+iframe*npix+is * nsamps+newloc0+1);
                }

            }

//            cout << "hi" << endl;

        }
        for (i=0; i<npix * totFrames; i++){
            *(idat+i)=*(tmpframe0+i) ;
        }
        delete [] tmpframe0 ;
    }



    // output array is totFrames x nsamples x nlines
    outarr = new unsigned short [npix * totFrames] ;
    unsigned short *tmpframe = new unsigned short [nsamps * nlines] ;
    if (ave_y > 0) {
        for (i=0; i<magoff; i++){
            for (is=lastis; is<nframes-1; is++){
                if (yoff[is]<=i && yoff[is+1]>i){
                    iLoc = is ;
                    frac0 = (yoff[is+1] -i) / diff ;
                    frac1 = 1. - frac0 ;
                    break ;
                }
            }
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }// resample the frames to get a new frame to insert in the stack array
            // now put that frame in the appropriate location
            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + nlines - is - nlines;
                if (yloc< 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }

    }
    if (ave_y < 0.){
        for (i=0; i<magoff; i++){
            for (is=lastis; is<nframes-1; is++){
                if (-yoff[is]<=i && -yoff[is+1]>i){
                    iLoc = is ;
                    lastis = is ;
                    diff =  (-yoff[is+1] - i) + (i +  yoff[is]) ;
                    frac0 = (-yoff[is+1] -i) / diff ;
                    frac1 = 1. - frac0 ;
                    break ;
                }
            }
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }// resample the frames to get a new frame to insert in the stack array

            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + is - nlines;

                if (yloc < 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc]
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }


    }
    totFrames = nLines ;



    cout << "size of tmpstr[420], output is " << nlines << " " <<  totFrames  << " "<< nsamps << endl ;

    char tmpstr[420], filename [420], filehdr[420] ;
    strcpy (filename, outPrefix) ;
    strcat (filename,"_stacked") ;
    strcpy (filehdr, outPrefix) ;
    strcat (filehdr,"_stacked") ;
    strcat (filehdr,".hdr") ;

    FILE *fout = fopen (filename, "w") ;
    if (fout == NULL) {
        cout << "could not open output file : aborting" << endl ;
        return ;
    }
    fwrite ((char *) &outarr[0], 2, npix * totFrames, fout) ;
    fclose (fout) ;

    FILE *foutHdr = fopen (filehdr, "w") ;
    fprintf (foutHdr,"Envi = \r\ndescription = {\r\nTircis Stacked Cube.}\r\n") ;
    fprintf (foutHdr, "samples = %d\r\n", nsamps) ;
    fprintf (foutHdr, "lines = %ld\r\n", totFrames) ;
    fprintf (foutHdr, "bands = %d\r\n", nlines) ;
    fprintf (foutHdr, "header offset=0") ;
    fprintf (foutHdr, "file type= ENVI Standard\r\n") ;
    fprintf (foutHdr, "data type= 12\r\n") ;
    fprintf (foutHdr, "interleave = bip\r\n") ;
    cout << "Header file written : " << filehdr ;
    fclose (foutHdr) ;


}

// array dimensions are nlines (elems in interferometer) by number of frames in image + 2 * nlines by number of samples
void suchi_offsets::resampleArray (unsigned short *idat, float ave_x, float ave_y,  int nframes )
{
    int i, j, is, js, samploc, xloc, yloc, zloc, nLines ;
    int newloc0 ; // iframe,
    int fullbounds ;

    float fracLoc, frac0, frac1, xoffval ;
    float magoff = fabs (ave_y * nframes )-1 ;
    int iLoc ;
    totFrames = magoff + nlines  ;
    if (totFrames > 1000) {
        totFrames = 1000 ;
        magoff = 999. ;
    }
    printf ("resample array : ave y offset is %f\r\n", ave_y) ;
    printf ("resample array : nframes is %d and totFrames is %ld\r\n", nframes,
            totFrames) ;
    printf ("magoff : %f\r\n", magoff) ;
    long npix = nsamps * nlines ;

    nLines = 0 ;

    if (ave_x > 0. || ave_x <0.){
        unsigned short *tmpframe0 = new unsigned short [npix * totFrames] ;
        if (tmpframe0 == NULL) {
            printf ("Could not alloc temp array\r\n") ;
        }
        // first frame, no offsets applied
        for (i=0;i<npix; i++)*(tmpframe0+i)=*(idat+i) ;

        //        cout << "iframe" << array_end;

        for (int iframe=1; iframe<totFrames; iframe++) {
            xoffval = ave_x * iframe ;
            xloc = int(xoffval) ;
            frac0 = 1. - (xoffval-xloc) ;
            frac1 = 1. - frac0 ;
            for (is=0; is<nlines; is++){

                for (js=0; js<nsamps; js++){
                    newloc0 = js + xloc ;
                    if (newloc0 < 0) newloc0 = 0 ;
                    if (newloc0 >= nsamps-2) newloc0 = nsamps-2 ;
                    *(tmpframe0+iframe*npix+is*nsamps+js) =
                            frac0 * *(idat+iframe*npix+is * nsamps+newloc0)+
                            frac1 * *(idat+iframe*npix+is * nsamps+newloc0+1);
                }

            }

            //            cout << iframe << array_end;

        }
        for (i=0; i<npix * totFrames; i++){
            *(idat+i)=*(tmpframe0+i) ;
        }
        delete [] tmpframe0 ;
    }


    // output array is totFrames x nsamples x nlines
    outarr = new unsigned short [npix * totFrames] ;\
    fullbounds = totFrames * npix ;
    unsigned short *tmpframe = new unsigned short [npix] ;
    // motion from bottom to top (decreasing in y)
    if (ave_y > 0.) {

        for (i=0; i<magoff; i++){
            fracLoc =  (float (i) / ave_y) ;
            iLoc = (int) fracLoc ;
            frac0 = 1. - (fracLoc - iLoc) ;
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }
            // now put that frame in the appropriate location
            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + nlines - is - nlines;
                if (yloc< 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }


    }

    if (ave_y < 0.) {

        for(int ii=0; ii< int(magoff); ii++){
            fracLoc = -(float (ii) / ave_y) ;
            iLoc = (int) fracLoc ;
            frac0 = 1. - (fracLoc - iLoc) ;
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }
            // now put that frame in the appropriate location
            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = ii + is - nlines - 1;

                if (yloc < 0) continue ;
                if (is==0) nLines++ ;
                //printf ("YLoc is %d\r\n", yloc) ;
                for (js=0; js<nsamps; js++){
                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc]
                    samploc = yloc * npix + zloc * nlines + xloc ;
                    if (samploc >= fullbounds) {
                        printf ("yloc : %d\tzloc : %d\r\n",yloc, zloc) ;
                        printf ("BOUNDS EXCEEDED\r\n") ;
                        exit(-1) ;
                    }
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }


    }
    totFrames = nLines ;
    cout << "size of output is " << nlines << " " <<  totFrames  << " "<< nsamps << endl ;

    char tmpstr[420], filename [420], filehdr[420] ;
    strcpy (filename, outPrefix) ;
    strcat (filename,"_stacked") ;
    strcpy (filehdr, outPrefix) ;
    strcat (filehdr,"_stacked") ;
    strcat (filehdr,".hdr") ;

    // write to output file
    FILE *fout  = fopen (filename, "w") ;
    fwrite ((char*) outarr, 2, npix * totFrames, fout) ;
    fclose (fout) ;

    // write hdr file
    FILE *foutHdr = fopen (filehdr, "w") ;
    fprintf (foutHdr,"Envi = \r\ndescription = {\r\nTircis Stacked Cube.}\r\n") ;
    fprintf (foutHdr, "samples = %d\r\n", nsamps) ;
    fprintf (foutHdr, "lines = %ld\r\n", totFrames) ;
    fprintf (foutHdr, "bands = %d\r\n", nlines) ;
    fprintf (foutHdr, "header offset=0") ;
    fprintf (foutHdr, "file type= ENVI Standard\r\n") ;
    fprintf (foutHdr, "data type= 12\r\n") ;
    fprintf (foutHdr, "interleave = bip\r\n") ;
    cout << "Header file written : " << filehdr ;
    fclose (foutHdr) ;

}

