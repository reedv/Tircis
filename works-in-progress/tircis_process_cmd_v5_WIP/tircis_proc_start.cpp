#include "tircis_proc_start.h"
#include "ui_tircis_proc_start.h"
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include "mainprocess.h"


Tircis_Proc_start::Tircis_Proc_start(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tircis_Proc_start)
{
    ui->setupUi(this);
    mp = new MainProcess () ;

}

Tircis_Proc_start::~Tircis_Proc_start()
{
    delete ui;
    delete mp ;
}

void Tircis_Proc_start::on_browseWorkdirButton_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, "Work directory")  ;
    ui->workdirLE->setText(str);
}

void Tircis_Proc_start::on_browseProcFileButton_clicked()
{
    QString str = QFileDialog::getOpenFileName (this, "Work directory")  ;
    ui->procfileLE->setText(str);

}

void Tircis_Proc_start::on_TheGoButton_clicked()
{
    QString str = ui->procfileLE->text() ;
    //readProcessFile (str) ;
    mp->readProcessFile (str) ;
    mp->start () ;
}



void Tircis_Proc_start::readProcessFile (QString qs) {

    // process file
    // 1st line is working directory
    // 2nd line is a series of numbers
    // 1st number is the number of wavecal files, 0 for wavelength file instead
    // 2nd and 3rd numbers are the start and stop elements out of the interferometer array for interferometer construction
    // then the low and high blackbodies
    // and then the scan file

    QString str, workdir, bbHotFile, bbColdFile, scanFile ;
    QStringList strList ;

    bool constOffFlag ;
    int i, nwaveFiles, leftFlag, val ;
    float tHot, tCold, xoff, yoff, totFrames ;
    QList<QString> waveNames ;
    QList<float> temps ;
    float *xoffArr, *yoffArr, aveX, aveY ;



    lFlag = false ;
    constFlag = false ;

    QFile qf (qs) ;
    qf.open( QIODevice::ReadOnly) ;
    QTextStream qts (&qf) ;
    str = qts.readLine () ;
    QDir qd (str) ;
    if (qd.exists()) workdir = str ;

    str = qts.readLine() ;
    strList = str.split(" ") ;
    nwaveFiles = strList[0].toInt() ;
    startCol = strList[1].toInt() ;
    endCol = strList[2].toInt() ;
    leftFlag = strList[3].toInt() ;
    if (leftFlag==1) lFlag = true ;
    val = strList[4].toInt() ;
    if (val ==1) {
        constFlag = true ;
        xoff = strList[5].toFloat() ;
        yoff = strList[6].toFloat() ;
    }

    gene->setSegParams(startCol, endCol, lFlag);



    // wavelength stuff

    if (nwaveFiles !=0) {
        for (i=0; i<nwaveFiles; i++) {
            str = qts.readLine () ;
            strList = str.split(",") ;
            waveNames.append (strList[0]) ;
            temps.append (strList[1].toFloat()) ;
        }
        wavecal->setWaveFiles (waveNames, temps) ;
        wavecal->setStartStop(startCol, endCol, lFlag);
        wavecal->processFull() ;

    }
    else {
        // get the name of the wavelength file.
        str = qts.readLine() ;
        //
        gene->readWaveFile (str) ;
        for (i=0; i<80; i++) {
            wavecal->wavelens.append( gene->wavelen[i]) ;
        }
    }

    prdlg->setProgressButtons(1);

    this->prdlg->plotWavecal (0) ;

    // get bbhot and bbcold
    str = qts.readLine() ;
    strList = str.split (",") ;
    bbHotFile = QString("%1/%2").arg(workdir).arg(strList[0]) ;
    tHot = strList[1].toFloat() ;
    str = qts.readLine() ;
    strList = str.split (",") ;
    bbColdFile = QString("%1/%2").arg(workdir).arg(strList[0]) ;
    tCold = strList[1].toFloat() ;
    gene->readBBFiles (bbHotFile, bbColdFile, tHot, tCold) ;
    prdlg->loadBBImages() ;
    gene->processBB() ;
    gene->genBBCurves() ;



    // then get the scan file
    str = qts.readLine () ;
    scanFile = QString("%1/%2").arg(workdir).arg(str) ;
    QFile qf0 (scanFile) ;
    if (!qf0.exists()){
        qDebug() << "Can not find scan file : " << scanFile ;
    }
    suchi = new SuchiFile (scanFile) ;
    suchi->readSuchiData() ;
    prdlg->setScanFile(suchi) ;
    prdlg->loadScanImage (0) ;


    // get the offset parameters for constant offsets
    if (ui->constOffRB->isChecked()) {
        aveX = ui->aveXOffLE->text().toFloat () ;
        aveY = ui->aveYOffLE->text().toFloat() ;
        constOffFlag = true ;
        so->resampleArray(suchi->indat, aveX, aveY,suchi->nbands) ;
        totFrames = so->totFrames ;
     }

    // if not constant, calculate the offsets
    else {
        xoffArr = new float [suchi->nbands+20] ;
        yoffArr = new float [suchi->nbands+20] ;
        so->calcOffsets (suchi->indat, xoffArr, yoffArr, 0, suchi->nbands-1) ;
        ui->aveXOffLE->setText (QString::number(so->ave_x)) ;
        ui->aveYOffLE->setText (QString::number(so->ave_y)) ;
        so->resampleArray (suchi->indat, xoffArr, yoffArr,suchi->nbands) ;
        delete [] xoffArr ;
        delete [] yoffArr ;

    }

    prdlg->loadStackedImage (160) ;
    prdlg->setProgressButtons(2);
    // then do the radiance
    gene->processCube (so);
    prdlg->loadBands () ;

    prdlg->loadRadianceImage (gene->nfinWaves/2, true) ;
    prdlg->setProgressButtons(3);
    // for debug write out suchi_outarr
    //QFile qf ("/home/harold/stacked") ;
    //qf

}



