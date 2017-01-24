#ifndef PROCESSRESULTSDLG_H
#define PROCESSRESULTSDLG_H


#include <QDialog>
#include "geneprocess.h"
#include "wavecal.h"
#include "suchifile.h"
#include "suchi_offsets.h"

namespace Ui {
class ProcessResultsDlg;
}

class ProcessResultsDlg : public QDialog
{
    Q_OBJECT
    GeneProcess *gp ;
    Wavecal *wc ;
    SuchiFile *scandat ;
    suchi_offsets *so ;
    int emissBand  ;
    
public:
    explicit ProcessResultsDlg(QWidget *parent = 0);
    ~ProcessResultsDlg();
    void setWavecal (Wavecal *wc) ;
    void setGeneProc (GeneProcess *gp) ;
    void setScanFile (SuchiFile *sf) ;
    void setSO (suchi_offsets *so) ;
    void loadBBImages () ;
    void loadScanImage (int frame) ;
    void loadStackedImage (int frameNum) ;
    void loadRadianceImage (int bandNum) ;

    void loadEmissivityImage (int bandNum) ;
    void loadRadianceImage (int bandNum, bool updateCB) ;
    void loadTemperatureImage (int bandNum) ;
    void plotStackedData (int loc) ;
    void plotWavecal (int type) ;
    void plotOffsets(int ) ;
    void loadBands() ;
    void setProgressButtons (int) ;
    bool emissFlag, radFlag ;
private slots:
    void on_frameSlider_valueChanged(int value);

    void on_horizontalSlider_valueChanged(int value);
    void stackLineChanged (int) ;
    void radClicked (QPoint) ;
    void emissClicked (QPoint) ;
    void emissMinMaxSet (float *) ;


    void on_displayBandCB_currentIndexChanged(int index);
    void on_FrameCB_currentIndexChanged(int index);



    void on_waveplotCB_activated(int index);

    void on_radianceDisplayCB_toggled(bool checked);

    void on_displayEmissBandCB_currentIndexChanged(int index);
    void on_emissRadButton_toggled(bool checked);

    void on_updateEmissButton_clicked();

public slots :
    void updateProgress(int state) ;
    void dispRadImage (int frames) ;
    void redispColdBB () ;
    void redispHotBB() ;
    void redispEmiss() ;
    void redispScan() ;
    void redispRadiance() ;
private:
    Ui::ProcessResultsDlg *ui;
};

#endif // PROCESSRESULTSDLG_H
