#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadc_class.h"
#include "settings_class.h"


#include <QDir>
#include <QFileDialog>
#include <QListWidget>
#include <QtCore>
#include <QObject>
#include <QTableWidgetItem>


void MainWindow::TestTHRProcedure(void)
{
    struct _vadc_params VADC_Params;

    QSemaphore *block_time_sem = new QSemaphore;

    QTableWidgetItem *item;

    float Ix=0.0;
    float Istep = ui->TestTHR_stepdoubleSpinBox->value();
    float PrdOn = 0.0,PrdOFF = 0.0,DFZOn = 0.0,DFZOFF = 0.0;

    bool PuskPRD = false;
    bool PuskDFZ = false;

    quint16 *test_reg = &m_mdfz_state.mdfz_reg1;
    quint16 mask_pusk_prd = 0;
    quint16 mask_pusk_dfz = 0;
    quint16 sett_prd = 0;
    quint16 sett_dfz = 0;
    quint8 Ix_izm_row=3;

    quint16 Imax = (quint16)(ui->TestTHR_MaxdoubleSpinBox->value()*100.0);
    ui->TestTHRprogressBar->setValue(0);
    ui->TestTHRprogressBar->setMaximum(Imax);

    typedef void (VADC_Class::*pVADCfuncType)(float Ix,quint16 deg,struct _vadc_params *VADC_params);

    pVADCfuncType pCongIx = &VADC_Class::VADC_Config3I0;

    if(ui->I0radioButton->isChecked())
    {
        pCongIx = &VADC_Class::VADC_Config3I0;
        mask_pusk_prd = (1<<4);
        mask_pusk_dfz = (1<<5);
        sett_prd = _V4Z_PUSK_PRD_I0;
        sett_dfz = _V4Z_PUSK_DFZ_I0;
        Ix_izm_row=3;
    }

    if(ui->I1radioButton->isChecked())
    {
        pCongIx = &VADC_Class::VADC_ConfigI1;
        mask_pusk_prd = (1<<2);
        mask_pusk_dfz = (1<<3);
        sett_prd = _V4Z_PUSK_PRD_I1;
        sett_dfz = _V4Z_PUSK_DFZ_I1;
        Ix_izm_row=4;
    }

    if(ui->I2radioButton->isChecked())
    {
        pCongIx = &VADC_Class::VADC_ConfigI2;
        mask_pusk_prd = (1<<0);
        mask_pusk_dfz = (1<<1);
        sett_prd = _V4Z_PUSK_PRD_I2;
        sett_dfz = _V4Z_PUSK_DFZ_I2;
        Ix_izm_row=5;
    }

    VADC_Params.en_VADC = true;

    mVADC->VADC_ConfigU1(57.7,0,&VADC_Params);
    mVADC->VADC_ConfigI1(0.0,0,&VADC_Params);

    mVADC->VADC_SetU(&VADC_Params);
    mVADC->VADC_SetI(&VADC_Params);

    mVADC->VADC_GetHWSettings(_V4Z_COMMON,12);

    block_time_sem->tryAcquire(1, QDeadlineTimer(70));
    QApplication::processEvents();
    mVADC->VADC_GetMDFZState();

    for(quint16 i=0;i<Imax;i++)
    {
        Ix += Istep;
        (mVADC->*pCongIx)(Ix,0,&VADC_Params);
        mVADC->VADC_SetI(&VADC_Params);
        block_time_sem->tryAcquire(1, QDeadlineTimer(100));
        QApplication::processEvents();

        mVADC->VADC_GetMDFZState();
        block_time_sem->tryAcquire(1, QDeadlineTimer(100));
        QApplication::processEvents();
        mVADC->VADC_GetIzm(_MDFZ_IZM_I);
        block_time_sem->tryAcquire(1, QDeadlineTimer(100));
        QApplication::processEvents();

        if( ( *test_reg & mask_pusk_prd) && (!PuskPRD) ) //Пуск ПРД по Ix
        {
           item = ui->TestTHRtableWidget-> item(1,1);
           item->setText(ui->IZMtableWidget->item(Ix_izm_row,4)->text() );
           PuskPRD = true;
           PrdOn = ui->IZMtableWidget->item(Ix_izm_row,4)->text().toFloat();
        }

        if((*test_reg & mask_pusk_dfz) && (!PuskDFZ) ) //Пуск ДФЗ по Ix
        {
           item = ui->TestTHRtableWidget-> item(2,1);
           item->setText(ui->IZMtableWidget->item(Ix_izm_row,4)->text() );
           PuskDFZ = true;
           DFZOn = ui->IZMtableWidget->item(Ix_izm_row,4)->text().toFloat();
        }

        if(PuskPRD && PuskDFZ) break;

        ui->TestTHRprogressBar->setValue(Ix*100);
        ui->TestTHRIxlcdNumber->display(Ix);
        QApplication::processEvents();
    }

    Ix += 1.0;
    mVADC->VADC_SetI(&VADC_Params);
    block_time_sem->tryAcquire(1, QDeadlineTimer(70));
    QApplication::processEvents();
    mVADC->VADC_GetMDFZState();
    block_time_sem->tryAcquire(1, QDeadlineTimer(70));
    QApplication::processEvents();
    mVADC->VADC_GetIzm(_MDFZ_IZM_I);
    block_time_sem->tryAcquire(1, QDeadlineTimer(70));
    QApplication::processEvents();

    for(quint16 i=1000;i>0;i--)
    {
        if(Ix<Istep) break;
        Ix -= Istep;
        (mVADC->*pCongIx)(Ix,0,&VADC_Params);
        mVADC->VADC_SetI(&VADC_Params);

        block_time_sem->tryAcquire(1, QDeadlineTimer(70));
        QApplication::processEvents();

        ui->TestTHRIxlcdNumber->display(Ix);

        mVADC->VADC_GetMDFZState();
        block_time_sem->tryAcquire(1, QDeadlineTimer(70));
        QApplication::processEvents();
        mVADC->VADC_GetIzm(_MDFZ_IZM_I);
        block_time_sem->tryAcquire(1, QDeadlineTimer(70));
        QApplication::processEvents();

        if( ((*test_reg & mask_pusk_prd) == 0) && (PuskPRD) ) //Пуск ПРД по Ix
        {
           item = ui->TestTHRtableWidget-> item(1,2);
           item->setText(ui->IZMtableWidget->item(Ix_izm_row,4)->text() );
           PuskPRD = false;
           PrdOFF = ui->IZMtableWidget->item(Ix_izm_row,4)->text().toFloat();
        }

        if( ((*test_reg & mask_pusk_dfz)==0) && (PuskDFZ) ) //Пуск ДФЗ по Ix
        {
           item = ui->TestTHRtableWidget-> item(2,2);
           item->setText(ui->IZMtableWidget->item(Ix_izm_row,4)->text() );
           PuskDFZ = false;
           DFZOFF = ui->IZMtableWidget->item(Ix_izm_row,4)->text().toFloat();
        }

        if((!PuskPRD) && (!PuskDFZ)) break;

        ui->TestTHRprogressBar->setValue(Ix*100);

    }

    VADC_Params.en_VADC = false;
    mVADC->VADC_SetI(&VADC_Params);

    item = ui->TestTHRtableWidget-> item(1,3);
    item->setText(QString::number(mHWSettings[sett_prd]/100.0));

    item = ui->TestTHRtableWidget-> item(2,3);
    item->setText(QString::number(mHWSettings[sett_dfz]/100.0));

    item = ui->TestTHRtableWidget-> item(1,4);
    item->setText(QString::number(mHWSettings[sett_prd]/PrdOn-100) + "%");

    item = ui->TestTHRtableWidget-> item(2,4);
    item->setText(QString::number(mHWSettings[sett_dfz]/DFZOn-100) + "%");

    item = ui->TestTHRtableWidget-> item(1,5);
    item->setText(QString::number(PrdOFF/PrdOn));

    item = ui->TestTHRtableWidget-> item(2,5);
    item->setText(QString::number(DFZOFF/DFZOn));

    ui->TestTHRprogressBar->setValue(0);

    delete block_time_sem;
}
