#include "cvprocess.h"
#include <vector>
//#include <QVector4D>
#include <limits>
#include <iostream>
//#include <QFile>
#define NO_NEIGHBOR   -1
using namespace std;


double lenSquared (float *arr1, float *arr2, int npts) ;
CVProcess::CVProcess(char *filename1, char *filename2)
{

     img1 = cvLoadImage (filename1, 0) ;
     img2 = cvLoadImage (filename2, 0) ;

   // im1 = imread (filename1) ;
   // im2 = imread (filename2) ;
    int hg = 1 ;
    hg *= 2 ;
    extractSURF () ;
    xyoffs = 0L ;
}

CVProcess::CVProcess(){
    xyoffs = 0L ;
}


CVProcess::~CVProcess(){
    cvReleaseImage (&img1) ;
    cvReleaseImage (&img2) ;
    delete [] xyoffs ;
}

void CVProcess::loadFromFloatArray (float *data, float *data1, int ns, int nl){
    int i ;
    unsigned char *bdata ;
    float minVal, maxVal, fval, scale ;
    minVal = 1.E9 ;
    maxVal = -1.E9 ;
    // find min and max
    for (i=0; i<ns * nl; i++){
        fval = *(data+i) ;
        if (fval< minVal ) minVal = fval ;
        if (fval>maxVal) maxVal = fval ;
        fval = *(data1+i) ;
        if (fval< minVal ) minVal = fval ;
        if (fval>maxVal) maxVal = fval ;
    }
    scale = 255. / (maxVal - minVal) ;



    img1 = cvCreateImage (cvSize (ns, nl), 8, 1) ;
    img2 = cvCreateImage (cvSize (ns, nl), 8, 1) ;
    //IplImage* img  = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);

    //int step       = img->widthStep/sizeof(uchar);

    bdata =  (uchar *)img1->imageData ;
    for (i=0;i<ns * nl; i++) {
        fval = *(data1+i) ;

        bdata[i] = (fval - minVal) * scale ;
    }
    bdata =  (uchar *)img2->imageData ;
    for (i=0;i<ns * nl; i++) {
        fval = *(data+i) ;

        bdata[i] = (fval - minVal) * scale ;
    }



    extractSURF () ;


}


// loading and extractSURF but from a subscene of the orig, used to avoid the fringe
// running down the center of the scene
void CVProcess::loadFromFloatArray (float *odata, float *odata1, int ns, int nl,
    int startx, int starty, int nx_orig, int ny_orig){
    int i, j ;
    unsigned char *bdata ;
    float minVal, maxVal, fval, scale ;
    minVal = 1.E9 ;
    maxVal = -1.E9 ;

    float *data, *data1 ;
    data = new float [ns * nl] ;
    data1 = new float [ns * nl] ;

    for (i=0; i<nl; i++) {
        for (j=0; j<ns; j++){
            data[i*ns+j] = odata[(i+starty)*nx_orig+(j+startx)] ;
            data1[i*ns+j] = odata1[(i+starty)*nx_orig+(j+startx)] ;
        }
    }

    // find min and max
    for (i=0; i<ns * nl; i++){
        fval = *(data+i) ;
        if (fval< minVal ) minVal = fval ;
        if (fval>maxVal) maxVal = fval ;
        fval = *(data1+i) ;
        if (fval< minVal ) minVal = fval ;
        if (fval>maxVal) maxVal = fval ;
    }
    scale = 255. / (maxVal - minVal) ;



    img1 = cvCreateImage (cvSize (ns, nl), 8, 1) ;
    img2 = cvCreateImage (cvSize (ns, nl), 8, 1) ;
    //IplImage* img  = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);

    //int step       = img->widthStep/sizeof(uchar);

    //QFile qf ("/data3/harold/data/test/feattest") ;
    //qf.open (QIODevice::WriteOnly) ;
    bdata =  (uchar *)img1->imageData ;
    for (i=0;i<ns * nl; i++) {
        fval = *(data1+i) ;

        bdata[i] = (fval - minVal) * scale ;
    }
    //qf.write ((char *)bdata, ns*nl) ;
    bdata =  (uchar *)img2->imageData ;
    for (i=0;i<ns * nl; i++) {
        fval = *(data+i) ;

        bdata[i] = (fval - minVal) * scale ;
    }


    //qf.write ((char *)bdata, ns*nl) ;
    //qf.close () ;

    extractSURF () ;
    delete [] data ;
    delete [] data1 ;

}

void CVProcess::extractSURF (){
    int i ;

    CvMemStorage* memoryBlock = cvCreateMemStorage();
    CvSeq* image1KeyPoints;
    CvSeq* image1Descriptors;
    CvSeq* image2KeyPoints;
    CvSeq* image2Descriptors;
    // Only values with a hessian greater than 500 are considered for keypoints
    CvSURFParams params = cvSURFParams(500, 1);
    cvExtractSURF(img1, 0, &image1KeyPoints, &image1Descriptors, memoryBlock, params);
    cvExtractSURF(img2, 0, &image2KeyPoints, &image2Descriptors, memoryBlock, params);



    // Find matching keypoints in both images
        vector<vector<CvPoint2D32f> > keyPointMatches;
        keyPointMatches.push_back(vector<CvPoint2D32f>());
        keyPointMatches.push_back(vector<CvPoint2D32f>());

        for ( i = 0; i < image1Descriptors->total; i++) {
            const CvSURFPoint* image1KeyPoint = (const CvSURFPoint*) cvGetSeqElem(image1KeyPoints, i);
            const float* image1Descriptor =  (const float*) cvGetSeqElem(image1Descriptors, i);
            int nearestNeighbor =
                    this->findNaiveNearestNeighbor(
                            image1Descriptor,
                            image1KeyPoint,
                            image2Descriptors,
                            image2KeyPoints
                            );

            if (nearestNeighbor == NO_NEIGHBOR) {
                continue;
            }

            keyPointMatches[0].push_back(((CvSURFPoint*) cvGetSeqElem(image1KeyPoints, i))->pt);
            keyPointMatches[1].push_back(((CvSURFPoint*) cvGetSeqElem(image2KeyPoints, nearestNeighbor))->pt);

        }

        float xdist, ydist ;
        if (xyoffs) delete [] xyoffs ;
        nMatchPoints = keyPointMatches[0].size()  ;
        xyoffs = new float [nMatchPoints*2] ;
        for (i=0; i<nMatchPoints; i++) {
            xdist = keyPointMatches[0].at(i).x - keyPointMatches[1].at(i).x ;
            ydist = keyPointMatches[0].at(i).y - keyPointMatches[1].at(i).y ;
            //std::cout << i << " " << xdist<< " " << ydist << std::array_end ;
            xyoffs [i*2] = xdist ;
            xyoffs [i*2+1] = ydist ;
        }


}

int CVProcess::findNaiveNearestNeighbor(
   const float* image1Descriptor,
   const CvSURFPoint* image1KeyPoint,
   CvSeq* image2Descriptors,
   CvSeq* image2KeyPoints)

{
    int descriptorsCount = (int)(image2Descriptors->elem_size/sizeof(float));
    double minSquaredDistance = std::numeric_limits<double>::max();
    double lastMinSquaredDistance = std::numeric_limits<double>::max();

    int neighbor;
    for (int i = 0; i < image2Descriptors->total; i++) {
        const CvSURFPoint* image2KeyPoint = (const CvSURFPoint*) cvGetSeqElem(image2KeyPoints, i);
        const float* image2Descriptor = (const float*) cvGetSeqElem(image2Descriptors, i);

        if (image1KeyPoint->laplacian != image2KeyPoint->laplacian)
            continue; // Don't worry about key points unless laplacian signs are equal

        double squaredDistance =
                this->compareSURFDescriptors(
                    image1Descriptor,
                    image2Descriptor,
                    descriptorsCount,
                    lastMinSquaredDistance);


        if (squaredDistance < minSquaredDistance) {
            neighbor = i;
            lastMinSquaredDistance = minSquaredDistance;
            minSquaredDistance = squaredDistance;
        } else if (squaredDistance < lastMinSquaredDistance) {
            lastMinSquaredDistance = squaredDistance;
        }
    }

    if (minSquaredDistance < 0.7 * lastMinSquaredDistance)
        return neighbor;

    return NO_NEIGHBOR;
}


double CVProcess::compareSURFDescriptors(
   const float* image1Descriptor,
   const float* image2Descriptor,
   int descriptorsCount,
   float lastMinSquaredDistance)

{
    double totalCost = 0;

    for (int i = 0; i < descriptorsCount; i += 4) {
		float desc1[] = {image1Descriptor[i+0], image1Descriptor[i+1], image1Descriptor[i+2], image1Descriptor[i+3]} ;
		float desc2[] = {image2Descriptor[i+0], image2Descriptor[i+1], image2Descriptor[i+2], image2Descriptor[i+3]} ;
		totalCost += lenSquared(desc1, desc2, 4) ;
        //totalCost += (descriptor2 - descriptor1).lengthSquared();
        if (totalCost > lastMinSquaredDistance)
            break;
    }

   return totalCost;
}


double lenSquared (float *arr1, float *arr2, int npts) {

	int i ;
	double diff, total = 0. ;

	for (i=0; i<npts; i++) {
		diff = arr1[i] - arr2[i] ;
		total +=  (diff * diff) ;
	}

	return total ;
}

