#include "processresultsdlg.h"
#include "ui_processresultsdlg.h"
#include <QScrollArea>

ProcessResultsDlg::ProcessResultsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessResultsDlg)
{
    ui->setupUi(this);
    ui->MainTabWidget->setCurrentIndex(0) ;
    ui->resampDispWidget->setBarFlags (false, true) ;
    ui->coldBBWidget->setBarFlags (false, false) ;
    ui->coldBBWidget->setImageType ("COLD BB") ;
    ui->hotBBWidget->setBarFlags (false, false) ;
    ui->hotBBWidget->setImageType ("Hot BB") ;
    ui->scanImWidget->setBarFlags (false, false) ;
    ui->scanImWidget->setImageType ("Scan Image") ;
    ui->emissImWidget->setBarFlags (false, false) ;
    ui->emissImWidget->setImageType ("Emiss/Temp Image") ;
    ui->radianceImWidget->setImageType("Radiance Image") ;
    connect (ui->resampDispWidget, SIGNAL (llChanged(int)), this, SLOT (stackLineChanged(int))) ;
    connect (ui->coldBBWidget, SIGNAL (reDisplay()), this, SLOT(redispColdBB())) ;
    connect (ui->hotBBWidget, SIGNAL (reDisplay()), this, SLOT(redispHotBB())) ;
    connect (ui->scanImWidget, SIGNAL (reDisplay()), this, SLOT(redispScan())) ;
    connect (ui->radianceImWidget, SIGNAL (reDisplay()), this, SLOT(redispRadiance())) ;
    connect (ui->radianceImWidget, SIGNAL(radClicked(QPoint)), this, SLOT(radClicked (QPoint))) ;
    connect (ui->emissImWidget, SIGNAL(radClicked(QPoint)), this, SLOT(emissClicked (QPoint))) ;
    connect (ui->emissImWidget, SIGNAL (setMnMx (float*)), this, SLOT (emissMinMaxSet (float *))) ;

    connect (ui->emissImWidget, SIGNAL (reDisplay()), this, SLOT(redispEmiss())) ;

    ui->radiancePlot->setAxesNames ("Wavelength (microns)", "Radiance (W /(m2 sr micron))") ;
    ui->emissivityPlot->setAxesNames ("Wavelength (microns)", "Apparent Emissivity") ;
    ui->emissivityPlot_2->setAxesNames ("Wavelength (microns)", "Apparent Emissivity") ;
    ui->proc_wl->setStyleSheet ("background-color:grey") ;
    ui->proc_BB->setStyleSheet ("background-color:grey") ;
    ui->proc_offsets->setStyleSheet ("background-color:grey");
    ui->proc_resamp->setStyleSheet ("background-color:grey") ;
    ui->proc_rad->setStyleSheet ("background-color:grey") ;
    ui->proc_scan->setStyleSheet ("background-color:grey") ;
    ui->proc_emiss->setStyleSheet ("background-color:grey") ;
    emissBand = 0 ;
    emissFlag = false ;
    radFlag = false ;
}

void ProcessResultsDlg::setProgressButtons (int mode) {


ui->radiancePlot->setAxesNames ("Wavelength (microns)", "Radiance (W /(m2 sr micron))") ;
ui->proc_wl->setStyleSheet ("background-color:grey") ;
ui->proc_offsets->setStyleSheet ("background-color:grey");
ui->proc_resamp->setStyleSheet ("background-color:grey") ;
ui->proc_rad->setStyleSheet ("background-color:grey") ;
ui->proc_BB->setStyleSheet ("background-color:grey") ;
ui->proc_scan->setStyleSheet ("background-color:grey") ;
ui->proc_emiss->setStyleSheet ("background-color:grey") ;
    switch (mode) {
        case 0 :
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:yellow") ;
            break ;
        case 1 :
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:yellow") ;
            break ;
        case 2 :
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:green") ;
            ui->proc_offsets->setStyleSheet ("background-color:yellow") ;
            break ;

        case 3 :
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:green") ;
            ui->proc_offsets->setStyleSheet ("background-color:green") ;
            ui->proc_resamp->setStyleSheet ("background-color:yellow") ;
            //ui->proc_rad->setStyleSheet ("background-color:yellow") ;
            break ;
        case 4:
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:green") ;
            ui->proc_offsets->setStyleSheet ("background-color:green") ;
            ui->proc_resamp->setStyleSheet ("background-color:green") ;
            ui->proc_rad->setStyleSheet ("background-color:yellow") ;
            break ;
        case 5:
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:green") ;
            ui->proc_offsets->setStyleSheet ("background-color:green") ;
            ui->proc_resamp->setStyleSheet ("background-color:green") ;
            ui->proc_rad->setStyleSheet ("background-color:green") ;
            ui->proc_emiss->setStyleSheet ("background-color:yellow") ;
            break ;
        case 6:
            ui->proc_wl->setStyleSheet ("background-color:green") ;
            ui->proc_BB->setStyleSheet ("background-color:green") ;
            ui->proc_scan->setStyleSheet ("background-color:green") ;
            ui->proc_offsets->setStyleSheet ("background-color:green") ;
            ui->proc_resamp->setStyleSheet ("background-color:green") ;
            ui->proc_rad->setStyleSheet ("background-color:green") ;
            ui->proc_emiss->setStyleSheet ("background-color:green") ;
            break ;

        }



}
void ProcessResultsDlg::updateProgress (int state) {
    switch (state){
        // wavecal
        case 0:
            setProgressButtons(0) ;
            plotWavecal(0) ;
            break ;
        // bb
        case 1:
            setProgressButtons(1) ;
            loadBBImages() ;
             break ;
        // scan file
        case 2 :
            setProgressButtons(2) ;
            loadScanImage(0) ;
            break ;
        // offsets
        case 3 :
            setProgressButtons(3);
            if (!so->constOffsetFlag) this->plotOffsets(0);
            break ;
        case 4 :
            this->loadStackedImage (60) ;\
            if (!so->constOffsetFlag) this->plotOffsets(0);

            setProgressButtons(4);
            break ;
        case 5 :
            loadBands() ;
            radFlag = true ;
            loadRadianceImage(this->gp->nfinWaves/2, true);
            setProgressButtons(5);

            break ;
        case 6 :
            //loadBands() ;
            emissFlag = true ;
            loadEmissivityImage(this->gp->nfinWaves/2);
            ui->displayEmissBandCB->setCurrentIndex (this->gp->nfinWaves/2) ;
            setProgressButtons(6);

            break ;
    }


}

ProcessResultsDlg::~ProcessResultsDlg()
{
    delete ui;
}

void ProcessResultsDlg::setWavecal (Wavecal *wc) {
    this->wc = wc ;
}


void ProcessResultsDlg::setGeneProc (GeneProcess *gp) {
    this->gp = gp ;
    connect (gp, SIGNAL (radProcced(int)), this, SLOT(dispRadImage (int))) ;
}

void ProcessResultsDlg::setScanFile (SuchiFile *s) {
    this->scandat = s ;
}

void ProcessResultsDlg::setSO (suchi_offsets *so) {
    this->so = so ;
}

void ProcessResultsDlg::plotWavecal (int type) {

    int i ;
    float *xdata = new float [100] ;
    float *ydata = new float [100] ;
    for (i=0; i<100; i++) xdata[i] = i ;
    switch (type) {
        // plot pixel vs wavelength in microns
        case 0 :
        for (i=0; i<100; i++) ydata[i] =gp->wavelen[i];
        ui->wavePlotWidget->setXYData (0, xdata, gp->wavelen, 55) ;
        ui->wavePlotWidget->yAxis->setRange(0, 25) ;
        ui->wavePlotWidget->yAxis->setLabel ("Wavelength (microns)") ;
        ui->wavePlotWidget->xAxis->setLabel ("Frequency bin") ;
        ui->wavePlotWidget->xAxis->setRange (16,40) ;
        ui->wavePlotWidget->replot() ;

        break ;

        case 1:
        for (i=0; i<100; i++) ydata[i] =10000./gp->wavelen[i];
        ui->wavePlotWidget->setXYData (0, xdata, ydata, 80) ;
        ui->wavePlotWidget->yAxis->setRange(400, 1500) ;
        ui->wavePlotWidget->yAxis->setLabel ("Wavenumber") ;
        ui->wavePlotWidget->xAxis->setLabel ("Frequency bin") ;
        ui->wavePlotWidget->xAxis->setRange (16,40) ;
        ui->wavePlotWidget->replot() ;

    }


}

void ProcessResultsDlg::plotOffsets (int arrNum){

    QString titstr ;
    float *xarr = new float [so->nbands] ;
    for (int i=0; i<so->nbands; i++) xarr[i]= i ;
    ui->offsetsPlot->xAxis->setLabel ("Frame") ;
    ui->offsetsPlot->yAxis->setLabel ("Cum Offsets") ;

    if (arrNum == 0){
        titstr = "Y Offsets vs. Frame" ;
        ui->offsetsPlot->setXYData (xarr, so->yoffArr, so->nbands) ;
    }
    else {
        titstr = "x Offsets vs. Frame" ;


        ui->offsetsPlot->setXYData (xarr, so->xoffArr, so->nbands) ;
    }
    ui->offsetsPlot->plotLayout()->addElement(0,0, new QCPPlotTitle (ui->offsetsPlot,titstr) ) ;
    ui->offsetsPlot->replot() ;

}


void ProcessResultsDlg::loadBBImages () {

    ui->coldBBWidget->loadQImage(gp->coldbb, gp->nsamps, gp->nlines);
    ui->hotBBWidget->loadQImage (gp->hotbb, gp->nsamps, gp->nlines) ;
    ui->coldBBWidget->repaint() ;
    ui->hotBBWidget->repaint() ;


}


void ProcessResultsDlg::redispColdBB (){
    ui->coldBBWidget->loadQImage(gp->coldbb, gp->nsamps, gp->nlines);
    ui->coldBBWidget->repaint() ;
}
void ProcessResultsDlg::redispHotBB (){
    ui->hotBBWidget->loadQImage(gp->hotbb, gp->nsamps, gp->nlines);
    ui->hotBBWidget->repaint() ;
}

void ProcessResultsDlg::redispEmiss() {

    // need to check if temp or emissButton toggled
    int band = ui->displayBandCB->currentIndex () ;

    //on_updateEmissButton_clicked() ;
    ui->emissImWidget->repaint() ;

    if (ui->emissRadButton->isChecked() )
        loadEmissivityImage (band) ;
    else loadTemperatureImage (band) ;




}

void ProcessResultsDlg::loadScanImage (int frame){
    ui->scanImWidget->loadQImage ((unsigned short *) &scandat->indat[frame * gp->nsamps * gp->nlines], gp->nsamps, gp->nlines) ;
    ui->nFramesLabel->setText (QString::number (scandat->nbands)) ;
    ui->frameSlider->setMinimum (0) ;
    ui->frameSlider->setMaximum (scandat->nbands) ;
    ui->scanImWidget->repaint() ;
}


void ProcessResultsDlg::loadStackedImage (int colNum){
    int i, j, ns, nl, nb, ival ;
    ns = so->nlines ;
    nl = so->totFrames ;
    ival = ui->horizontalSlider->value() ;
    if (ival != colNum){
        ui->horizontalSlider->setValue (colNum) ;
    }

    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = so->outarr[i * 320L * ns + colNum * ns + j] ;
        }
    }
    ui->resampDispWidget->setImageSize (ns, nl) ;
    ui->resampDispWidget->loadQImage (imdat, ns, nl) ;
    ui->resampDispWidget->repaint() ;


}


void ProcessResultsDlg::loadRadianceImage (int bandNum){
    int i, j, ns, nl, nb ;
    if (!radFlag) return ;
    ns = so->nsamps ;
    nl = so->totFrames ;



    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->radArray [i * ns * gp->nfinWaves +  j * gp->nfinWaves + bandNum] ;
        }
    }
    ui->radianceImWidget->setImageSize (ns, nl) ;
    ui->radianceImWidget->loadQImage (imdat, ns, nl) ;
    ui->radianceImWidget->repaint() ;
    ui->radianceImWidget->mouseClicking = true ;

}


void ProcessResultsDlg::loadEmissivityImage (int bandNum){
    int i, j, ns, nl, nb ;
    if (!emissFlag) return ;
    ns = so->nsamps ;
    nl = so->totFrames ;



    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->emissArr [i * ns * gp->nfinWaves +  j * gp->nfinWaves + bandNum] ;
        }
    }
    ui->emissImWidget->setImageSize (ns, nl) ;
    //ui->emissImWidget->setMinMax (.96, 1.0) ;
    ui->emissImWidget->loadQImage (imdat, ns, nl) ;
    ui->emissImWidget->repaint() ;
    ui->emissImWidget->mouseClicking = true ;

    delete [] imdat ;

}

void ProcessResultsDlg::loadTemperatureImage (int bandNum){
    int i, j, ns, nl ;
    ns = so->nsamps ;
    nl = so->totFrames ;


    if (!emissFlag) return ;
    // float *imdat = new float [nl * ns] ;
    /*
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->tempArr [i * ns +  j ] ;
        }
    }
    */
    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->tempArr [i * ns * gp->nfinWaves +  j * gp->nfinWaves + bandNum] ;
        }
    }
    ui->emissImWidget->setImageSize (ns, nl) ;
    ui->emissImWidget->loadQImage (imdat, ns, nl) ;
    ui->emissImWidget->repaint() ;
    ui->emissImWidget->mouseClicking = true ;

    //delete [] imdat ;

}

void ProcessResultsDlg::loadRadianceImage (int bandNum, bool updateCB){
    int i, j, ns, nl, nb ;
    ns = so->nsamps ;
    nl = so->totFrames ;

    if (!radFlag) return ;

    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->radArray [i * ns * gp->nfinWaves +  j * gp->nfinWaves + bandNum] ;
        }
    }
    if (updateCB){
        ui->displayBandCB->setCurrentIndex (bandNum) ;
    }
    ui->radianceImWidget->setImageSize (ns, nl) ;

    ui->radianceImWidget->loadQImage (imdat, ns, nl) ;
    ui->radianceImWidget->repaint() ;
    ui->radianceImWidget->mouseClicking = true ;

}


void ProcessResultsDlg::dispRadImage (int frames){
    int i, j, ns, nl, nb, bandNum ;
    ns = so->nsamps ;
    nl = frames ;
    bandNum = gp->nfinWaves / 2 ;

    //if (!radFlag) return ;

    float *imdat = new float [nl * ns] ;
    for (i=0; i<nl; i++){
        for (j=0; j<ns; j++){
            imdat[i * ns + j] = gp->radArray [i * ns * gp->nfinWaves +  j * gp->nfinWaves + bandNum] ;
        }
    }
    //if (updateCB){
    //    ui->displayBandCB->setCurrentIndex (bandNum) ;
    //}
    ui->radianceImWidget->setImageSize (ns, nl) ;

    ui->radianceImWidget->loadQImage (imdat, ns, nl) ;
    ui->radianceImWidget->repaint() ;
    ui->radianceImWidget->mouseClicking = true ;

}



void ProcessResultsDlg::on_frameSlider_valueChanged(int value)
{
    if (value >= scandat->nbands) value = scandat->nbands -1 ;
    ui->curFrameLabel->setText (QString::number (value)) ;

    ui->scanImWidget->loadQImage ((unsigned short *) &scandat->indat[value * gp->nsamps * gp->nlines], gp->nsamps, gp->nlines) ;
    ui->scanImWidget->repaint() ;
}

void ProcessResultsDlg::on_horizontalSlider_valueChanged(int value)
{
    loadStackedImage (value) ;
    ui->stack_column_label->setText (QString::number(value)) ;
}

void ProcessResultsDlg::plotStackedData (int loc) {

    int i, icol ;
    int nsamps = so->nlines ;
    float *profarr = new float [so->nlines] ;
    float *xdat = new float [so->nlines] ;

    icol = ui->horizontalSlider->value() ;

    if (so->outarr){
        for (i=0; i<nsamps; i++){
            profarr [i] = so->outarr[loc * so->nsamps * so->nlines + icol * so->nlines + i] ;
            xdat[i] = i ;
        }
        ui->interferogramPlot->setXYData (xdat, profarr, nsamps) ;
    }

    delete [] profarr ;
    delete [] xdat ;
}

void ProcessResultsDlg::stackLineChanged(int loc) {


    plotStackedData (loc) ;
}

void ProcessResultsDlg::loadBands(){
    QString str,str1 ;
    int ib ;

    int nwaves = gp->nfinWaves ;
    ui->displayBandCB->clear() ;
    for (ib=gp->startFinWave; ib<=gp->stopFinWave; ib++){
        str = QString::number (gp->wavelen[ib]) ;
        str1=QString ("%1 microns").arg(str) ;
        ui->displayBandCB->addItem (str1) ;
        ui->displayEmissBandCB->addItem(str1) ;
    }

}


void ProcessResultsDlg::radClicked (QPoint qpt){
    int x, y, ib, sampnum, xloc, yloc ;
    xloc = qpt.x() ;
    yloc = qpt.y() ;

    int nwaves = gp->nfinWaves ;
    float *xvals = new float [nwaves] ;
    float *yvals = new float [nwaves] ;
    float *yyvals = new float [nwaves] ;
    for (ib=gp->startFinWave; ib<=gp->stopFinWave; ib++){
        sampnum = ib - gp->startFinWave ;
        yvals [sampnum] = gp->radArray [yloc * gp->nsamps * nwaves +  xloc * nwaves + ib-gp->startFinWave] ;
        if (emissFlag)
            yyvals [sampnum] = gp->emissArr [yloc * gp->nsamps * nwaves +  xloc * nwaves + ib-gp->startFinWave] ;
        xvals [sampnum] = gp->wavelen[ib] ;
     }

    ui->radiancePlot->setXYData (xvals, yvals, nwaves) ;
    if (emissFlag)
        ui->emissivityPlot->setXYData (xvals, yyvals, nwaves) ;
    delete [] xvals ;
    delete [] yvals ;
    delete [] yyvals ;
}

void ProcessResultsDlg::emissClicked (QPoint qpt){
    int x, y, ib, sampnum, xloc, yloc ;
    float *dataArr ;


    xloc = qpt.x() ;
    yloc = qpt.y() ;
    if (ui->emissRadButton->isChecked()){
        dataArr = gp->emissArr ;
        ui->emissivityPlot_2->setAxesNames ("Wavelength (microns)", "Apparent Emissivity") ;
    } else {
        dataArr = gp->tempArr ;
        ui->emissivityPlot_2->setAxesNames ("Wavelength (microns)", "Temp (Deg C)") ;
    }
    if (!emissFlag) return ;
    int nwaves = gp->nfinWaves ;
    float *xvals = new float [nwaves] ;
    float *yvals = new float [nwaves] ;
    for (ib=gp->startFinWave; ib<=gp->stopFinWave; ib++){
        sampnum = ib - gp->startFinWave ;
        yvals [sampnum] = dataArr [yloc * gp->nsamps * nwaves +  xloc * nwaves + ib-gp->startFinWave] ;
        xvals [sampnum] = gp->wavelen[ib] ;
     }

    ui->emissivityPlot_2->setXYData (xvals, yvals, nwaves) ;
    delete [] xvals ;
    delete [] yvals ;

}

void ProcessResultsDlg::on_displayBandCB_currentIndexChanged(int index)
{


    loadRadianceImage (index) ;
}

void ProcessResultsDlg::on_FrameCB_currentIndexChanged(int index)
{
    plotOffsets (index) ;
}


void ProcessResultsDlg::on_waveplotCB_activated(int index)
{
    this->plotWavecal (index) ;
}

void ProcessResultsDlg::on_radianceDisplayCB_toggled(bool checked)
{
    // determine whether to display radiance or emissivity, if toggled

}

void ProcessResultsDlg::on_displayEmissBandCB_currentIndexChanged(int index)
{
    ui->emissImWidget->userFlag = false ;
    if (ui->emissRadButton->isChecked() )
        loadEmissivityImage(index);
    else
        loadTemperatureImage (index) ;
    emissBand = index ;
}

void ProcessResultsDlg::on_emissRadButton_toggled(bool checked)
{
    int ind ;
    ui->emissImWidget->userFlag = false ;
    if (ui->emissRadButton->isChecked()){

        loadEmissivityImage (emissBand) ;
    }
    else loadTemperatureImage (emissBand) ;
}

void ProcessResultsDlg::on_updateEmissButton_clicked()
{

    bool emissDFlag = false ;
    int band ;
    float minval, maxval ;
    // need to check if temp or emissButton toggled
    band = ui->displayBandCB->currentIndex () ;

    if (ui->emissRadButton->isChecked() ) {
            emissDFlag = true ;
            QString str = ui->emissMinLE->text() ;
            minval = str.toFloat() ;
            str = ui->emissMaxLE->text() ;
            maxval = str.toFloat() ;
            ui->emissImWidget->setMinMax(minval, maxval) ;

            loadEmissivityImage (band) ;
    }
    else {
        QString str = ui->emissMinLE->text() ;
        minval = str.toFloat() ;
        str = ui->emissMaxLE->text() ;
        maxval = str.toFloat() ;
        ui->emissImWidget->setMinMax(minval, maxval) ;
        loadTemperatureImage(band) ;
    }

}


void ProcessResultsDlg::redispScan () {
    int frame = ui->frameSlider->value() ;
    loadScanImage(frame) ;
}

void ProcessResultsDlg::emissMinMaxSet (float *vals) {

    QString s0 = QString ("%1").arg(vals[0]) ;
    ui->emissMinLE->setText (s0) ;
    s0 = QString ("%1").arg(vals[1]) ;
    ui->emissMaxLE->setText (s0) ;

}

void ProcessResultsDlg::redispRadiance () {
    int bandNum ;
    bandNum = ui->displayBandCB->currentIndex() ;

    loadRadianceImage(bandNum) ;
}
