#ifndef TIRCIS_PROC_START_H
#define TIRCIS_PROC_START_H

#include <QDialog>

#include "wavecal.h"
#include "geneprocess.h"
#include "suchi_offsets.h"
#include "suchifile.h"
#include "processresultsdlg.h"
#include "mainprocess.h"

namespace Ui {
class Tircis_Proc_start;
}

class Tircis_Proc_start : public QDialog
{
    Q_OBJECT
    
public:
    explicit Tircis_Proc_start(QWidget *parent = 0);
    ~Tircis_Proc_start();
    void readProcessFile (QString) ;
    bool lFlag, constFlag ;
    int startCol, endCol ;
    Wavecal *wavecal ;
    GeneProcess *gene ;
    suchi_offsets *so ;
    SuchiFile *suchi ;
    ProcessResultsDlg *prdlg ;
    MainProcess *mp ;
    
private slots:
    void on_browseWorkdirButton_clicked();

    void on_browseProcFileButton_clicked();

    void on_TheGoButton_clicked();

private:
    Ui::Tircis_Proc_start *ui;
};

#endif // TIRCIS_PROC_START_H
