
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


void MainWindow::slotIzmTimerProcedure(void)
{
     mVADC->VADC_GetIzm(_MDFZ_IZM_U);
     mVADC->VADC_GetIzm(_MDFZ_IZM_I);
     mVADC->VADC_GetIzm(_MDFZ_IZM_SIM_FILTERS);
     mVADC->VADC_GetIzm(_MDFZ_IZM_ZX);
     mVADC->VADC_GetMDFZState();
     mVADC->VADC_GetTime();
     mVADC->VADC_GetDate();
     mVADC->VADC_GetLEDS();
     mVADC->VADC_GetMUHelth();
     mVADC->VADC_GetCompState();
}

void MainWindow::slotLEDSTimerProcedure()
{
    static quint8 ka_leds_state = 0; //столбец текущего состояния светодиода

    if(ui->tabWidget->currentIndex()==3) //обновляемся, только если находимся на вкладке "Настройки" - небольшая оптимизация
    {
        //[строка] - состояние светодиода в аппарате; [столбец] - шаг конечного автомата
        const bool led_state[4][8] =
            {
                {false, false, false, false, false, false, false, false},
                {false, true,  false, true,  false, true,  false, true },
                {false, false, true , true,  true,  true,  true,  true },
                {true,  true,  true,  true,  true,  true,  true,  true }
            }; //таблица состояний светодиода для каждого шага конечного автомата

        const QString BLUE_LED_PICT[2]      = {":/res/main/Pictures/info_off.png",  ":/res/main/Pictures/documentinfo.png"};
        const QString RED_LED_PICT[2]       = {":/res/main/Pictures/avar0.png",     ":/res/main/Pictures/avar1.png"};
        const QString YELLOW_LED_PICT[2]    = {":/res/main/Pictures/predupr0.png",  ":/res/main/Pictures/predupr1.png"};
        const QString GREEN_LED_PICT[2]     = {":/res/main/Pictures/rabota0.png",   ":/res/main/Pictures/rabota1.png"};

        //левый столбец

        ui->LED_Maniplabel->setPixmap(QPixmap(GREEN_LED_PICT[led_state[m_LEDs.data_bits.LED_MANIPUL][ka_leds_state]]));
        ui->LED_Ostanovlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_OSTANOV][ka_leds_state]]));
        ui->LED_PuskPRDlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_PUSK_PRD][ka_leds_state]]));
        ui->LED_PuskRZlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_PUSK_RZ][ka_leds_state]]));
        ui->LED_PUSKDFZlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_PUSK_V4Z][ka_leds_state]]));
        ui->LED_WorkDFZlabel->setPixmap(QPixmap(BLUE_LED_PICT[led_state[m_LEDs.data_bits.LED_WORK_V4Z][ka_leds_state]]));
        ui->LED_WorkRZlabel->setPixmap(QPixmap(BLUE_LED_PICT[led_state[m_LEDs.data_bits.LED_WORK_RZ][ka_leds_state]]));
        ui->LED_WorkKAlabel->setPixmap(QPixmap(BLUE_LED_PICT[led_state[m_LEDs.data_bits.LED_KA][ka_leds_state]]));
        ui->LED_UROVlabel->setPixmap(QPixmap(BLUE_LED_PICT[led_state[m_LEDs.data_bits.LED_UROV][ka_leds_state]]));
        ui->LED_BlockPRDlabel->setPixmap(QPixmap(RED_LED_PICT[led_state[m_LEDs.data_bits.LED_BLOCK_PRD][ka_leds_state]]));

        //правый столбец

        ui->LED_5Vlabel->setPixmap(QPixmap(GREEN_LED_PICT[led_state[(quint8)m_usb_connect*3][ka_leds_state]]));
        ui->LED_24Vlabel->setPixmap(QPixmap(GREEN_LED_PICT[led_state[m_LEDs.data_bits.LED_24V][ka_leds_state]]));
        ui->LED_PuskUMlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_PUSK_UM][ka_leds_state]]));
        ui->LED_PRMlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_COMP_OSN][ka_leds_state]]));
        ui->LED_AKlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_AK][ka_leds_state]]));
        ui->LED_VvodDFZlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_VVOD_V4Z][ka_leds_state]]));
        ui->LED_VvodRZlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_VVOD_RZ][ka_leds_state]]));
        ui->LED_VvodKAlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_VVOD_KA][ka_leds_state]]));
        ui->LED_Preduprlabel->setPixmap(QPixmap(YELLOW_LED_PICT[led_state[m_LEDs.data_bits.LED_PREDUPR][ka_leds_state]]));
        ui->LED_Avarlabel->setPixmap(QPixmap(RED_LED_PICT[led_state[m_LEDs.data_bits.LED_AVAR][ka_leds_state]]));
    }

    ka_leds_state = (ka_leds_state + 1) & 0x07;
}

void MainWindow::CreateIZMTable(void)
{
    const QStringList headers = {tr("Величина"),tr("Значение"),tr("Угол"),tr("Величина"),tr("Значение"),tr("Угол")};
    const QStringList izm_names1 = {"Ua","Ub","Uc","Uab","Ubc","Uca","Ia","Ib","Ic","Iab","Ibc","Ica"};
    const QStringList izm_names2 = {"3U0","U1","U2","I0","I1","I2","Zab","Zbc","Zca","--","--","--"};
    ui->IZMtableWidget->setColumnCount(6); // Указываем число колонок
    ui->IZMtableWidget->setRowCount(12);
    ui->IZMtableWidget->setShowGrid(true); // Включаем сетку
    ui->IZMtableWidget->setHorizontalHeaderLabels(headers);

    for(quint8 i=0;i<12;i++)
    {
        ui->IZMtableWidget->setItem(i,0,new QTableWidgetItem(izm_names1[i]));
        ui->IZMtableWidget->setItem(i,1,new QTableWidgetItem("---"));
        ui->IZMtableWidget->setItem(i,2,new QTableWidgetItem("---"));

        ui->IZMtableWidget->setItem(i,3,new QTableWidgetItem(izm_names2[i]));
        ui->IZMtableWidget->setItem(i,4,new QTableWidgetItem("---"));
        ui->IZMtableWidget->setItem(i,5,new QTableWidgetItem("---"));
    }
}

void MainWindow::slotFillIZMTable(QByteArray izm_data)//
{
    QTableWidgetItem *item;
    quint8 type_izm = izm_data.at(4);
    union tconvert_short izm;
    union tconvert_short izm_deg;

    if((type_izm == _MDFZ_IZM_U) || (type_izm == _MDFZ_IZM_I))
    {
        for(uint8_t i=0;i<6;i++)
        {
            izm.charval[_HIGH] = izm_data.at(2*i+5);
            izm.charval[_LOW]  = izm_data.at(2*i+6);

            item = ui->IZMtableWidget->item(i+6*type_izm,1);
            item->setText(QString::number(izm.shortval/100.0f));

            izm_deg.charval[_HIGH] = izm_data.at(2*i+17);
            izm_deg.charval[_LOW]  = izm_data.at(2*i+18);

            item = ui->IZMtableWidget->item(i+6*type_izm,2);
            item->setText(QString::number(izm_deg.shortval));

            if(type_izm == _MDFZ_IZM_U)
            {
                if(i==0) DrawVector(m_izmUaVect,izm.shortval/100.0f,360-izm_deg.shortval);
                if(i==1) DrawVector(m_izmUbVect,izm.shortval/100.0f,360-izm_deg.shortval);
                if(i==2) DrawVector(m_izmUcVect,izm.shortval/100.0f,360-izm_deg.shortval);
            }

            if(type_izm == _MDFZ_IZM_I)
            {
                if(i==0) DrawVector(m_izmIaVect,izm.shortval/10.0f,360-izm_deg.shortval);
                if(i==1) DrawVector(m_izmIbVect,izm.shortval/10.0f,360-izm_deg.shortval);
                if(i==2) DrawVector(m_izmIcVect,izm.shortval/10.0f,360-izm_deg.shortval);
            }
        }
    }
    if(type_izm == _MDFZ_IZM_SIM_FILTERS)
    {
        for(uint8_t i=0;i<6;i++)
        {
            izm.charval[_HIGH] = izm_data.at(2*i+5);
            izm.charval[_LOW]  = izm_data.at(2*i+6);

            item = ui->IZMtableWidget->item(i,4);
            item->setText(QString::number(izm.shortval/100.0f));
        }
    }

    if(type_izm == _MDFZ_IZM_ZX)
    {
        for(uint8_t i=0;i<3;i++)
        {
            izm.charval[_HIGH] = izm_data.at(2*i+5);
            izm.charval[_LOW]  = izm_data.at(2*i+6);

            item = ui->IZMtableWidget->item(i+6,4);
            item->setText(QString::number(izm.shortval/100.0f));

            izm_deg.charval[_HIGH] = izm_data.at(2*i+11);
            izm_deg.charval[_LOW]  = izm_data.at(2*i+12);

            item = ui->IZMtableWidget->item(i+6,5);
            item->setText(QString::number(izm_deg.shortval));

            if(i==0)
            {
                m_Zx_state.Zab = izm.shortval/100.0f;
                m_Zx_state.Deg_ab = izm_deg.shortval;
               // DrawVector(m_izmZabVect,m_Zx_state.Zab,m_Zx_state.Deg_ab);
                DrawZxVector(m_ZabVect,m_Zx_state.Zab,m_Zx_state.Deg_ab);
            }

            if(i==1)
            {
                m_Zx_state.Zbc = izm.shortval/100.0f;
                m_Zx_state.Deg_bc = izm_deg.shortval;
               // DrawVector(m_izmZbcVect,m_Zx_state.Zbc,m_Zx_state.Deg_bc);
                DrawZxVector(m_ZbcVect,m_Zx_state.Zbc,m_Zx_state.Deg_bc);
            }

            if(i==2)
            {
                m_Zx_state.Zca = izm.shortval/100.0f;
                m_Zx_state.Deg_ca = izm_deg.shortval;
              //  DrawVector(m_izmZcaVect,m_Zx_state.Zca,m_Zx_state.Deg_ca);
                DrawZxVector(m_ZcaVect,m_Zx_state.Zca,m_Zx_state.Deg_ca);
            }
        }
    }

}

void MainWindow::slotFillStateMDFZTable(QByteArray state_data)
{
    tconvert_short reg;

    reg.charval[_HIGH] = state_data.at(4);
    reg.charval[_LOW]  = state_data.at(5);
    m_mdfz_state.mdfz_reg0 = reg.shortval;

    reg.charval[_HIGH] = state_data.at(6);
    reg.charval[_LOW]  = state_data.at(7);
    m_mdfz_state.mdfz_reg1 = reg.shortval;

    reg.charval[_HIGH] = state_data.at(8);
    reg.charval[_LOW]  = state_data.at(9);
    m_mdfz_state.mdfz_reg2 = reg.shortval;

    reg.charval[_HIGH] = state_data.at(10);
    reg.charval[_LOW]  = state_data.at(11);
    m_mdfz_state.mdfz_reg2E = reg.shortval;

    reg.charval[_HIGH] = state_data.at(12);
    reg.charval[_LOW]  = state_data.at(13);
    m_mdfz_state.mdfz_reg3A = reg.shortval;

    reg.charval[_HIGH] = state_data.at(14);
    reg.charval[_LOW]  = state_data.at(15);
    m_mdfz_state.mdfz_reg3B = reg.shortval;

    reg.charval[_HIGH] = state_data.at(16);
    reg.charval[_LOW]  = state_data.at(17);
    m_mdfz_state.mdfz_reg3C = reg.shortval;

    reg.charval[_HIGH] = state_data.at(18);
    reg.charval[_LOW]  = state_data.at(19);
    m_mdfz_state.mdfz_regKT = reg.shortval;


    ui->StatelistWidget->clear();
    for(uint8_t i=0;i<8;i++)
    {
        reg.charval[_HIGH] = state_data.at(4+i*2);
        reg.charval[_LOW]  = state_data.at(5+i*2);
        //ui->StatelistWidget->addItem(QString::number(reg.shortval));
        ui->StatelistWidget->addItem(QString("0x%1").arg(reg.shortval, 4, 16, QLatin1Char( '0' )));
    };

}
