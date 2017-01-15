extern "C"{
#include "nrutil.h"
#include "nr.h"
}

#include <mutex>
std::mutex polyfit_mutex;


void  poly1d (double x, double p[], int np) ;
void  poly3d (double x, double p[], int np) ;
void polyfit (double *x, double *y, int npts, int degree, double *coefs, double *yf) ;


/*
void svdfit (double x[], double y[], double sig[], int ndata, double a[], int ma,
        double **u, double **v, double w[], double *chisq,
        void (*funcs)(double, double [], int)) ;
*/

//void polyfit (double *x, double *y, int npts, int degree, double *coefs)
//{

//    double **u, **v, *w, chisq=0.f ;

//    int ncoefs = degree + 1 ;

//    //double *wt ;
//    //	wt = new double [npts] ;

//    double wt[npts];

//    u = dmatrix (1, npts, 1, ncoefs) ;
//    v = dmatrix (1, ncoefs, 1, ncoefs) ;
//    w = dvector (1, ncoefs) ;

//    for (int i =0 ; i<npts; i++) *(wt+i) = 1.0f ;

//    if (degree==1) {
//        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
//                  ncoefs, u, v, w, &chisq, poly1d) ;
//    }
//    if (degree==3) {
//        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
//                  ncoefs, u, v, w, &chisq, poly3d) ;
//    }

//    free_dmatrix (u, 1, npts, 1, ncoefs) ;
//    free_dmatrix (v, 1, ncoefs, 1, ncoefs) ;
//    free_dvector (w, 1,  ncoefs) ;
//    //	delete [] wt ;


//}

void polyfit (double *x, double *y, int npts, int degree, double *coefs, double *yf)
{
    int i ;
    double **u, **v, *w, chisq=0.f ;

    int ncoefs = degree + 1 ;

    //    double *wt ;
    //	wt = new double [npts] ;
    double wt[npts];

    u = dmatrix (1, npts, 1, ncoefs) ;
    v = dmatrix (1, ncoefs, 1, ncoefs) ;
    w = dvector (1, ncoefs) ;

    for (i =0 ; i<npts; i++) *(wt+i) = 1.0f ;

    if (degree==1) {
        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
                  ncoefs, u, v, w, &chisq, poly1d) ;
        for (i=0; i<npts; i++)
            *(yf+i) = coefs[0]+ coefs[1] * *(x+i) ;
    }

    if (degree==3) {

        // PROBLEM HERE!!!
        polyfit_mutex.lock();
        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
                  ncoefs, u, v, w, &chisq, poly3d) ;
        polyfit_mutex.unlock();

        for (i=0; i<npts; i++) {
            *(yf+i) = coefs[0]+ coefs[1] * *(x+i) +
                    coefs[2] * *(x+i) * *(x+i) + coefs[3] * *(x+i) *
                    *(x+i) * *(x+i) ;
        }
    }

    free_dmatrix (u, 1, npts, 1, ncoefs) ;
    free_dmatrix (v, 1, ncoefs, 1, ncoefs) ;
    free_dvector (w, 1,  ncoefs) ;
    //	delete [] wt ;


}

void  poly1d (double x, double p[], int np) {
    p[1]=1.f ;
    p[2]=x ;
}
void  poly3d (double x, double p[], int np) {
    p[1]=1.f ;
    p[2]=x ;
    p[3]=x*x ;
    p[4]=x*x*x ;
}

void polyfitSat (double *xvals, double *yvals, int *sat, int nptsTot, int degree, double *coefs, int *satNum)
{

    int i, count = 0, npts=0 ;
    double **u, **v, *w, chisq=0.f ;
    double *wt ;
    int ncoefs = degree + 1 ;
    *satNum = 0 ;

    double *x = new double [nptsTot] ;
    double *y = new double [nptsTot] ;
    for (i=0; i<nptsTot; i++){
        if (*(sat+i)==0){
            x[count]=xvals[i];
            y[count]=yvals[i];
            count++ ;
        } else *satNum++ ;

    }
    npts = count ;
    wt = new double [npts] ;

    u = dmatrix (1, npts, 1, ncoefs) ;
    v = dmatrix (1, ncoefs, 1, ncoefs) ;
    w = dvector (1, ncoefs) ;

    for (int i =0 ; i<npts; i++) *(wt+i) = 1.0f ;

    if (degree==1) {
        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
                  ncoefs, u, v, w, &chisq, poly1d) ;
    }
    if (degree==3) {
        svdfit_d (x-1, y-1, wt-1, npts, coefs-1,
                  ncoefs, u, v, w, &chisq, poly3d) ;
    }

    free_dmatrix (u, 1, npts, 1, ncoefs) ;
    free_dmatrix (v, 1, ncoefs, 1, ncoefs) ;
    free_dvector (w, 1,  ncoefs) ;
    delete [] wt ;


}


