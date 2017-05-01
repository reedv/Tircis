#include "mainprocess.h"



MainProcess::MainProcess () 
{
  strcpy(workdir,"") ;
  strcpy(bbHotFile,"") ;
  strcpy(bbColdFile,"")  ;
  strcpy(scanFile, "") ;
  startBin = 38 ;
  stopBin = 233 ;
  lFlag = false ;
  gp = new GeneProcess () ;
  so = new suchi_offsets () ;
  wc = new Wavecal () ;

  waveNames.clear() ;
  temps.clear() ;
  tHot = 50. ;
  tCold = 20. ;


}

void MainProcess::setBlackBodies (char *bbcoldFile, char *bbhotFile, float TCold, float THot){

  tCold = TCold ;
  tHot = THot ;
  strcpy(this->bbColdFile, bbcoldFile) ;
  strcpy(this->bbHotFile, bbhotFile) ;

  printf ("HOT Blackbody file is : %s\r\n", this->bbHotFile ) ;
  printf ("COLD Blackbody file is : %s\r\n", this->bbColdFile ) ;

}

void MainProcess::readProcessFile (char * qstr)
{
  char outPref[420], str[420], *tmp ;
  float seg_yintcpt, seg_slope, npts ;
  int i, val, leftFlag, nWaveFiles ;

  // qstr is the name of the input process file
  printf ("reading %s\r\n",qstr) ;
  FILE *fil = fopen (qstr, "r") ;
  if (fil == NULL) {
      printf ("Problem opening process file %s\r\n", qstr) ;
      return ;
    }


  fgets (str, 420, fil) ;
  printf ("%s\r\n",str) ;
  strcpy (workdir, str) ;
  // number of wave files and the profile segment specification

  fgets (str, 420, fil) ;
  tmp = strtok (str, " \t") ;
  nWaveFiles = atoi (tmp) ;
  tmp = strtok (NULL, " \t") ;
  seg_yintcpt = atof (tmp) ;
  tmp = strtok (NULL, " \t") ;
  seg_slope = atof (tmp) ;
  tmp = strtok (NULL, " \t") ;
  npts = atoi (tmp) ;
  tmp = strtok (NULL, " \t") ;
  leftFlag = atoi (tmp) ;
  lFlag = false ;
  constFlag = false ;
  if (leftFlag ==1) lFlag = true ;
  tmp = strtok (NULL, " \t") ;
  i = atoi (tmp) ;
  if (i==1) constFlag = true ;
  tmp = strtok (NULL, " \t") ;
  printf ("xoff is %s\r\n",tmp) ;
  xoffAvg = atof (tmp) ;
  tmp = strtok (NULL, " \t\r\n") ;
  printf ("yoff is %s\r\n",tmp) ;
  yoffAvg = atof (tmp) ;


  gp->setSegParamsNew(seg_yintcpt, seg_slope, npts, lFlag);
  wc->setStartStop (startBin, stopBin, lFlag);
  if (nWaveFiles !=0) {
      waveNames.clear() ;
      temps.clear() ;
      for (i=0; i<nWaveFiles; i++) {
          fgets (str, 420, fil) ;
          tmp = strtok (str, ",") ;
          waveNames.push_back(tmp) ;
          tmp = strtok (NULL, " \t\r\n") ;
          temps.push_back (atof(tmp)) ;
        }
      wc->setWaveFiles (waveNames, temps) ;
      wc->processFull () ;

    } else {
      fgets (str, 420, fil) ;
      tmp=strtok (str, "\r\n") ;
      gp->readWaveFile (tmp) ;
      wc->wavelens.clear() ;
      for (i=0; i<160; i++) {
          wc->wavelens.push_back(gp->wavelen[i]) ;
          printf ("%d\t %f\r\n", i, wc->wavelens[i]) ;
        }
    }
	// read in the segment start file (segs.off)
    fgets (str, 420, fil) ;
    tmp=strtok (str, "\r\n") ;
	gp->readSegmentFile (str) ;
  
  // get blackbodies
  fgets (str, 420, fil) ;
  tmp = strtok (str, ",") ;
  strcpy (bbHotFile, tmp) ;
  tmp = strtok (NULL, " \r\n") ;
  tHot = atof (tmp) ;
  fgets (str, 420, fil) ;
  tmp = strtok (str, ",") ;
  strcpy (bbColdFile, tmp) ;
  tmp = strtok (NULL, " \r\n") ;
  tCold = atof (tmp) ;

  // the gen the scan image
  fgets (str, 420, fil) ;
  tmp=strtok (str, "\r\n") ;
  strcpy (scanFile, tmp) ;
  fgets (str, 420, fil) ;
  tmp=strtok (str, "\r\n") ;
  strcpy (outPref, tmp) ;
  gp->setOutpref (outPref) ;
  so->setOutprefix (outPref) ;

}


void MainProcess::run ()
{
  int i ;

  gp->readBBFiles (bbHotFile, bbColdFile, tHot, tCold) ;
  gp->processBB() ;
  gp->genBBCurves() ;
  // scanfile

  sf = new SuchiFile (scanFile) ;
  sf->readSuchiData() ;
  so->constOffsetFlag = constFlag ;
  // resample



  if (this->constFlag) {

      so->resampleArray(sf->indat, xoffAvg, yoffAvg,sf->nbands) ;
      //totFrames = so->totFrames ;
    }
  else {
      char offFile [420], outstr [240] ;
      strcpy (offFile, workdir) ;
      strcat(offFile, "/offsets.txt") ;
      xoffArr = new float [sf->nbands+20] ;
      yoffArr = new float [sf->nbands+20] ;
      so->calcOffsets (sf->indat, xoffArr, yoffArr, 0, sf->nbands-1) ;
      printf ("Offsets calculated\r\n") ;
      ///ui->aveXOffLE->setText (QString::number(so->ave_x)) ;
      // write offsetArray to text file
      FILE *fil = fopen (offFile, "w") ;
      if (fil == NULL) {
          printf ("Problem with offsets file : %s\r\n", offFile) ;
          return ;
        }

      so->resampleArray (sf->indat, xoffArr, yoffArr,sf->nbands) ;
      printf ("Array resampled for gene\r\n") ;

      for (i=0; i<sf->nbands; i++){
          fprintf (fil, "%f %f\r\n", xoffArr[i], yoffArr[i]) ;
        }
      fclose (fil) ;


    }

  //	printf ("Calculating radiance\r\n") ;
  cout << endl << "Calculating radiance" << endl;

  gp->processCube (so) ;

  gp->calcEmissivity() ;


}
