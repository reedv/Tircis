#ifndef CVPROCESS_H
#define CVPROCESS_H
#include <stdio.h>

#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include "opencv/highgui.h"


#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;



class CVProcess
{
public:
    CVProcess(char *, char *);
    CVProcess();
    ~CVProcess () ;
    void loadFromFloatArray (float *data, float *data1, int ns, int nl);
    void loadFromFloatArray (float *data, float *data1, int ns, int nl, int startx, int starty,
                             int nx_orig, int ny_orig);
    void extractSURF () ;

    int     findNaiveNearestNeighbor(const float* image1Descriptor, const CvSURFPoint* image1KeyPoint,
       CvSeq* image2Descriptors, CvSeq* image2KeyPoints) ;

    double  compareSURFDescriptors(
       const float* image1Descriptor,
       const float* image2Descriptor,
       int descriptorsCount,
       float lastMinSquaredDistance) ;

    Mat         im1, im2;
    IplImage    *img1, *img2 ;
    int         src_x, src_y, src_xstart, src_ystart, nMatchPoints ;
    float       *xdiffs, *ydiffs, *xyoffs ;

};

#endif // CVPROCESS_H
