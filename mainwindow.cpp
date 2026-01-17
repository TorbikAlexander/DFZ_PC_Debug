#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadc_class.h"
#include "settings_class.h"
#include "OSC.h"
#include "tmodbusdfz.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QListWidget>
#include <QtCore>
#include <QObject>
#include <QTableWidgetItem>
#include <QProcess>
#include <QSemaphore>
#include <QMessageBox>
#include <QDesktopServices>
#include <QStack>
#include <QModbusTcpClient>

extern struct _osc_buff OSC_Buff[MDFZ_OSC_COUNT][MDFZ_OSC_MAX_MESS_COUNT];

void MainWindow::CreateZxResTable()
{
    const QStringList headers = {tr("Угол"),tr("Сопротивление")};
    ui->ResultZxtableWidget->clear();
    ui->ResultZxtableWidget->setColumnCount(2); // Указываем число колонок
    ui->ResultZxtableWidget->setRowCount(0);
    ui->ResultZxtableWidget->setShowGrid(true); // Включаем сетку
    ui->ResultZxtableWidget->setHorizontalHeaderLabels(headers);
}



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mVADC = new VADC_Class(this);
    mSettings = new Settings_class();

    QThread *modbus_thread= new QThread;
    mModbusDFZ = new TModbusDFZ(this);
    mModbusDFZ->moveToThread(modbus_thread);
    modbus_thread->start();

    ui->tabWidget->setCurrentIndex(0);

    ui->connect_label->setPixmap(QPixmap(":/res/main/Pictures/Connection Disabled.png"));

    m_logdb = QSqlDatabase::addDatabase("QSQLITE");

    QString part_db = QDir::toNativeSeparators(QApplication::applicationDirPath())+"\\dbase\\log_db.db";
    m_logdb.setDatabaseName( part_db );

    qDebug() << part_db;
    ShowGraph();

    CreateIZMTable();
    ShowIZMGraph();
    ShowZxGraph();

    CreateZxResTable();

    CreateHWSettingsTable();

    ConfigOSC_Chart();

    ui->TestZxprogressBar->setVisible(false);

    m_IZMTimer = new QTimer(this);
    connect(m_IZMTimer,SIGNAL(timeout()),this,SLOT(slotIzmTimerProcedure()));
   // m_IZMTimer->stop();
    m_IZMTimer->start(1000);

    m_PingTimer = new QTimer(this);
    connect(m_PingTimer,SIGNAL(timeout()),this,SLOT(slotPingTimerProcedure()));
   // m_PingTimer->start(5000);

    m_LEDSTimer= new QTimer(this);
    connect(m_LEDSTimer,SIGNAL(timeout()),this,SLOT(slotLEDSTimerProcedure()));
    m_LEDSTimer->start(250);

    m_LEDs.sr[0] = 0;
    m_LEDs.sr[1] = 0;
    m_LEDs.sr[2] = 0;
    m_LEDs.sr[3] = 0;
    m_LEDs.sr[4] = 0;
    ui->pushButton_9->setEnabled(false);
    ui->LogReadprogressBar->setVisible(false);


    ConfigFlexLogic();
    //ui->AddXORpushButton->setVisible(false);
    //qDebug() << m_input_sign_names.count();

    for(quint8 i=0;i<4;i++)
    {
        for(quint8 j=0;j<32;j++)
        {
            quint8 index = i*32+j;
            m_RBACcheckBox[index] = new QCheckBox(RBAC_func_names[index]);
            //connect(m_RBACcheckBox[index], &QCheckBox::checkStateChanged, this, &MainWindow::on_RBACCheckBoxProcessState);
            ui->RBACgridLayout->addWidget(m_RBACcheckBox[index],j,i);
        }
    }

    for(quint8 i=0;i<9;i++)
    {
        for(quint8 j=0;j<16;j++)
        {
            quint8 index = i*16+j;
            m_MDFZRegscheckBox[index] = new QCheckBox(MDFZModbusregisters[index]);
            //m_MDFZRegscheckBox[index].
            ui->RegsMDFZLayout->addWidget(m_MDFZRegscheckBox[index],j,i);
        }
    }

    for(quint8 i=0;i<2;i++)
    {
        for(quint8 j=0;j<8;j++)
        {
            quint8 index = i*8+j;
            m_MursReleState[index] = new QCheckBox(MURSReleNames[index]);
            ui->MURSRelegridLayout->addWidget(m_MursReleState[index],i,j);
        }
    }

}

MainWindow::~MainWindow()
{
    m_IZMTimer->stop();
    delete m_IZMTimer;
    m_PingTimer->stop();
    delete m_PingTimer;
    m_LEDSTimer->stop();
    delete m_LEDSTimer;
    delete mVADC;
    delete mSettings;

    QEventLoop loop; //задержка на окончание обмена в потоках
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timer.start(1000);
    loop.exec();
    timer.stop();
    emit signalFinishedAll();


    delete ui;
}

bool MainWindow::GetConnectionState(void)
{
    return m_usb_connect;
}

void MainWindow::slotPingTimerProcedure(void)
{
    QString nParameter = "-n";
    QString pingCount = "2"; //(int)
    QString wParameter = "-w";
    QString pingWaitTime = "10"; //(ms)
    QProcess *pingProcess = new QProcess;
    int exitCode = pingProcess->execute("ping",QStringList() << QString("192.168.3.33")<<nParameter<<pingCount<<wParameter<<pingWaitTime);

    if (exitCode==0)
    {
        m_usb_connect = true;
        ui->connect_label->setPixmap(QPixmap(":/res/main/Pictures/Connection Active.png"));
        m_IZMTimer->start(500);
        m_PingTimer->stop();

    } else
    {
        m_usb_connect = false;
        ui->connect_label->setPixmap(QPixmap(":/res/main/Pictures/Connection Disabled.png"));

        m_LEDs.sr[0] = 0;
        m_LEDs.sr[1] = 0;
        m_LEDs.sr[2] = 0;
        m_LEDs.sr[3] = 0;
        m_LEDs.sr[4] = 0;

        m_IZMTimer->stop();
    }

    delete pingProcess;
}

void MainWindow::slotConnectStateProcedure(bool state)
{

    if (state == true)
    {

        if(!m_usb_connect)  ui->connect_label->setPixmap(QPixmap(":/res/main/Pictures/Connection Active.png"));

    } else
    {
        if(m_usb_connect)
        {
            ui->connect_label->setPixmap(QPixmap(":/res/main/Pictures/Connection Disabled.png"));

            m_LEDs.sr[0] = 0;
            m_LEDs.sr[1] = 0;
            m_LEDs.sr[2] = 0;
            m_LEDs.sr[3] = 0;
            m_LEDs.sr[4] = 0;
        }

    }

    m_usb_connect = state;

}

void MainWindow::ReadCurrentVADCParams(struct _vadc_params *VADC_Params)
{
    VADC_Params->en_VADC = ui->VADCEn_checkBox->isChecked();

    VADC_Params->syncUa = ui->SyncUacheckBox->isChecked();

    VADC_Params->Ua = ui->Ua_doubleSpinBox->value();
    VADC_Params->degUa = ui->UaDeg_spinBox->value();

    VADC_Params->Ub = ui->Ub_doubleSpinBox->value();
    VADC_Params->degUb = ui->UbDeg_spinBox->value();

    VADC_Params->Uc = ui->Uc_doubleSpinBox->value();
    VADC_Params->degUc = ui->UcDeg_spinBox->value();

    VADC_Params->U3U0 = ui->U3U0_doubleSpinBox->value();
    VADC_Params->deg3U0 = ui->U3U0Deg_spinBox->value();


    VADC_Params->Ia = ui->Ia_doubleSpinBox->value();
    VADC_Params->degIa = ui->IaDeg_spinBox->value();

    VADC_Params->Ib = ui->Ib_doubleSpinBox->value();
    VADC_Params->degIb = ui->IbDeg_spinBox->value();

    VADC_Params->Ic = ui->Ic_doubleSpinBox->value();
    VADC_Params->degIc = ui->IcDeg_spinBox->value();
}

void MainWindow::LoadCurrentVADCParams(struct _vadc_params *VADC_Params)
{
    ui->VADCEn_checkBox->setChecked(VADC_Params->en_VADC);
    ui->SyncUacheckBox->setChecked(VADC_Params->syncUa);

    ui->Ua_doubleSpinBox->setValue(VADC_Params->Ua);
    ui->UaDeg_spinBox->setValue(VADC_Params->degUa);

    ui->Ub_doubleSpinBox->setValue(VADC_Params->Ub);
    ui->UbDeg_spinBox->setValue(VADC_Params->degUb);

    ui->Uc_doubleSpinBox->setValue(VADC_Params->Uc);
    ui->UcDeg_spinBox->setValue(VADC_Params->degUc);

    ui->Ia_doubleSpinBox->setValue(VADC_Params->Ia);
    ui->IaDeg_spinBox->setValue(VADC_Params->degIa);

    ui->Ib_doubleSpinBox->setValue(VADC_Params->Ib);
    ui->IbDeg_spinBox->setValue(VADC_Params->degIb);

    ui->Ic_doubleSpinBox->setValue(VADC_Params->Ic);
    ui->IcDeg_spinBox->setValue(VADC_Params->degIc);

    ui->U3U0_doubleSpinBox->setValue(VADC_Params->U3U0);
    ui->U3U0Deg_spinBox->setValue(VADC_Params->deg3U0);

}




void MainWindow::on_Save_pushButton_clicked()
{
    struct _vadc_params VADC_Params;

    QString filename= QFileDialog::getSaveFileName(this, tr("Сохранить файл конфигурации как ..."), "Settings", tr("Файл конфигурациии (*.ini)") );

    if(!filename.isNull())
    {
        ReadCurrentVADCParams(&VADC_Params);
        mSettings->SaveSettings(filename,&VADC_Params);
    }
}


void MainWindow::on_Load_pushButton_clicked()
{
    struct _vadc_params VADC_Params;

    QString filename= QFileDialog::getOpenFileName(this, tr("Открыть файл конфигурации"), "Settings",tr("Файл конфигурациии (*.ini)") );

    if(!filename.isNull())
    {
        mSettings->ReadSettings(filename,&VADC_Params);
        LoadCurrentVADCParams(&VADC_Params);
    }
}


void MainWindow::on_SendParams_pushButton_clicked()
{
    struct _vadc_params VADC_Params;

    ReadCurrentVADCParams(&VADC_Params);

    mVADC->VADC_SetU(&VADC_Params);
    mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_Ua_doubleSpinBox_valueChanged(double arg1)
{
    DrawVector(mUaVect,arg1,ui->UaDeg_spinBox->value());

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_UaDeg_spinBox_valueChanged(int arg1)
{
     DrawVector(mUaVect,ui->Ua_doubleSpinBox->value(),arg1);

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_Ub_doubleSpinBox_valueChanged(double arg1)
{
     DrawVector(mUbVect,arg1,ui->UbDeg_spinBox->value());

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_UbDeg_spinBox_valueChanged(int arg1)
{
    DrawVector(mUbVect,ui->Ub_doubleSpinBox->value(),arg1);

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_Uc_doubleSpinBox_valueChanged(double arg1)
{
    DrawVector(mUcVect,arg1,ui->UcDeg_spinBox->value());

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_UcDeg_spinBox_valueChanged(int arg1)
{
    DrawVector(mUcVect,ui->Uc_doubleSpinBox->value(),arg1);

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
    mVADC->VADC_SetU(&VADC_Params);
}


void MainWindow::on_Ia_doubleSpinBox_valueChanged(double arg1)
{
    DrawVector(mIaVect,arg1*10.0f,ui->IaDeg_spinBox->value());

    struct _vadc_params VADC_Params;
    ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_IaDeg_spinBox_valueChanged(int arg1)
{
    DrawVector(mIaVect,ui->Ia_doubleSpinBox->value()*10.0f,arg1);

     struct _vadc_params VADC_Params;
     ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_Ib_doubleSpinBox_valueChanged(double arg1)
{
    DrawVector(mIbVect,arg1*10.0f,ui->IbDeg_spinBox->value());

     struct _vadc_params VADC_Params;
     ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_IbDeg_spinBox_valueChanged(int arg1)
{
    DrawVector(mIbVect,ui->Ib_doubleSpinBox->value()*10.0f,arg1);

     struct _vadc_params VADC_Params;
     ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_Ic_doubleSpinBox_valueChanged(double arg1)
{
    DrawVector(mIcVect,arg1*10.0f,ui->IcDeg_spinBox->value());

     struct _vadc_params VADC_Params;
     ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_IcDeg_spinBox_valueChanged(int arg1)
{
    DrawVector(mIcVect,ui->Ic_doubleSpinBox->value()*10.0f,arg1);

     struct _vadc_params VADC_Params;
     ReadCurrentVADCParams(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


void MainWindow::on_UpdateModepushButton_clicked()
{
     QString work_part = qApp->applicationDirPath();
     QDir dir(work_part+"/Modes");

     ui->ModeslistWidget->clear();

     if(dir.exists())
     {
         QFileInfoList dirContent = dir.entryInfoList(QStringList()
                                                          << "*.ini",
                                                          QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

         for(quint8 i=0; i< dirContent.length();i++ )
         {
            ui->ModeslistWidget->addItem(dirContent.value(i).fileName());
         }
     } else
     {
        ui->ModeslistWidget->addItem("Папка "+ work_part+"/Modes не существует");
     }
}


void MainWindow::on_ModeslistWidget_itemDoubleClicked(QListWidgetItem *item)
{
     QString filename;
     struct _vadc_params VADC_Params;
     filename = qApp->applicationDirPath() +"/Modes/" + item->text();
     mSettings->ReadSettings(filename,&VADC_Params);
     LoadCurrentVADCParams(&VADC_Params);

     mVADC->VADC_SetU(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);
}


//характеристика РС окружность/эллипс.
void MainWindow::DrawCirclePatterns(quint16 Zx,quint16 phi_m4,quint8 k_ellipt,quint8 offset)
{
    quint8 error_val = 10;
    float X0,Y0,XP,YP;
    float arg;
    float Ra,Rb;

    error_val = ui->errorRSspinBox->value();
    m_ErrorMVect->clear();
    m_ErrorPVect->clear();

    Ra = Zx/2.0;
    Rb = Ra*k_ellipt/10;

    for (int i = 0; i < 360; ++i)
    {
        arg = i*M_PI/180;
        X0 = Ra * cos(arg) + Ra;
        Y0 = Rb * sin(arg);

        X0 = X0 - Ra* offset/100;

        XP =X0*cos(-phi_m4*M_PI/180) +  Y0 * sin(-phi_m4*M_PI/180);
        YP =Y0*cos(-phi_m4*M_PI/180) -  X0 * sin(-phi_m4*M_PI/180);
        m_ErrorPVect->append(XP + XP*error_val*0.01,YP + YP*error_val*0.01);
        m_ErrorMVect->append(XP - XP*error_val*0.01,YP - YP*error_val*0.01);
    }

    m_ErrorMVect->setVisible(true);
    m_ErrorPVect->setVisible(true);


}

//характеристика РС полигональная.
void MainWindow::DrawPolyPatterns(quint16 sett_start,quint16 sett_count)
{
    quint8 error_val = 10;
    QSemaphore *block_time_sem = new QSemaphore;

    mVADC->VADC_GetHWSettings(sett_start,sett_count);
    block_time_sem->tryAcquire(1, QDeadlineTimer(300));
    QApplication::processEvents();

    quint16 n_points = mHWSettings[sett_start];
    float XerrP,YerrP;
    float XP,YP;

    error_val = ui->errorRSspinBox->value();
    m_ErrorMVect->clear();
    m_ErrorPVect->clear();

    for (int i = 0; i < n_points; i++)
    {
        XP = float( (qint16)mHWSettings[sett_start+1+i*2]/100.0);
        XerrP = float( XP * error_val/100.0);
        YP = float((qint16)mHWSettings[sett_start+1+i*2+1]/100.0);
        YerrP = float(YP * error_val/100.0);
        m_ErrorPVect->append(XP + XerrP,YP + YerrP);
        m_ErrorMVect->append(XP - XerrP,YP - YerrP);
    }

    //соединение с последней точкой
    XP = float( (qint16)mHWSettings[sett_start+1]/100.0);
    XerrP = float( XP * error_val/100.0);
    YP = float((qint16)mHWSettings[sett_start+1+1]/100.0);
    YerrP = float(YP * error_val/100.0);
    m_ErrorPVect->append(XP + XerrP,YP + YerrP);
    m_ErrorMVect->append(XP - XerrP,YP - YerrP);

    m_ErrorMVect->setVisible(true);
    m_ErrorPVect->setVisible(true);

    delete block_time_sem;
}

bool MainWindow:: DrawErrorPatterns(quint16 pattern)
{
    quint16 sett_start=0, sett_count=1;
    bool no_poly_pattern = true;

    //добавить определение типа характеристики: эллипс/полигональная

     switch (pattern) {
     case _DFZ_AB_PATTERN:
        DrawCirclePatterns(40,70,8,10);
        no_poly_pattern = true;
        break;
     case _DFZ_BC_PATTERN:
        DrawCirclePatterns(35,70,8,10);
        no_poly_pattern = true;
        break;
     case _DFZ_CA_PATTERN:
        DrawCirclePatterns(35,70,8,10);
        no_poly_pattern = true;
        break;
     case _RZ_1ST_PATTERN:
        no_poly_pattern = false;
        sett_start = _POLY_PATTERN1_DATA_START;
        sett_count = 41;
        DrawPolyPatterns(sett_start,sett_count);
        break;
     case _RZ_2ST_PATTERN:
        no_poly_pattern = false;
        sett_start = _POLY_PATTERN2_DATA_START;
        sett_count = 41;
        DrawPolyPatterns(sett_start,sett_count);
        break;
     case _RZ_3ST_PATTERN:
        no_poly_pattern = false;
        sett_start = _POLY_PATTERN3_DATA_START;
        sett_count = 41;
        DrawPolyPatterns(sett_start,sett_count);
        break;
     default:
        break;
     }



    return no_poly_pattern;
}

void MainWindow::on_pushButton_clicked()
{
     struct _vadc_params VADC_Params;

     QSemaphore *block_time_sem = new QSemaphore;

     float *pTestU;
     qint16 *pTestDeg;
     quint16 *pTestWorkReg;
     quint16 test_mask;
     float *pTestZx;
     quint16 *pTestDegZx;

     quint16 deg_step = ui->DegStepspinBox->value();
     quint16 deg_start= ui->DegStartspinBox->value();
     quint16 deg_stop = ui->DegStopspinBox->value();

     float Ustart = ui->UmaxZxdoubleSpinBox->value();
     float Ustop  = ui->UminZxdoubleSpinBox->value();
     float Ustep  = ui->UstepdoubleSpinBox->value();
     float Itest  = ui->IZxdoubleSpinBox->value();

     float Zcur;
     float Zmax = 0.0f;
     qint16 deg_phim4;
     //float TestZ0deg = 0.01;

     QString NameZx = "не выбрано";

     quint16 pattern = _DFZ_AB_PATTERN;
     bool need_phi_m4 = false;

     ui->pushButton->setEnabled(false);
     ui->pushButton_9->setEnabled(true);
     m_stop_test_Zx = false;

//(m_mdfz_state.mdfz_reg1  & (1<<9));  //AB - 10 BC - 11 CA - 9 биты
//(m_mdfz_state.mdfz_reg3C & 4); //AB - 0 BC - 1 CA - 2 биты

     m_ZabVect->setVisible(false);
     m_ZbcVect->setVisible(false);
     m_ZcaVect->setVisible(false);
     m_PhiM4Vect->setVisible(false);
     m_RM0RM2Vect->setVisible(false);

//ДФЗ
     if(ui->DFZZabradioButton->isChecked())
     {
        pTestU = &VADC_Params.Ua;
        VADC_Params.Ia = Itest;
        pTestDeg = &VADC_Params.degUa;
        pTestWorkReg = &m_mdfz_state.mdfz_reg1;
        test_mask = (1<<10);
        m_ZabVect->setVisible(true);
        pTestZx = &m_Zx_state.Zab;
        pTestDegZx = &m_Zx_state.Deg_ab;
        NameZx = "ДФЗ АВ";
        pattern = _DFZ_AB_PATTERN;
     }

     if(ui->DFZZbcradioButton->isChecked())
     {
        pTestU = &VADC_Params.Ub;
        VADC_Params.Ib = Itest;
        pTestDeg = &VADC_Params.degUb;
        pTestWorkReg = &m_mdfz_state.mdfz_reg1;
        test_mask = (1<<11);
        m_ZbcVect->setVisible(true);
        pTestZx = &m_Zx_state.Zbc;
        pTestDegZx = &m_Zx_state.Deg_bc;
        NameZx = "ДФЗ ВС";
        pattern = _DFZ_BC_PATTERN;
     }

     if(ui->DFZZcaradioButton->isChecked())
     {
        pTestU = &VADC_Params.Uc;
        VADC_Params.Ic = Itest;
        pTestDeg = &VADC_Params.degUc;
        pTestWorkReg = &m_mdfz_state.mdfz_reg1;
        test_mask = (1<<9);
        m_ZcaVect->setVisible(true);
        pTestZx = &m_Zx_state.Zca;
        pTestDegZx = &m_Zx_state.Deg_ca;
        NameZx = "ДФЗ СА";
        pattern = _DFZ_CA_PATTERN;
     }
//ДЗ 1ст
     if(ui->DZZabradioButton->isChecked())
     {
        pTestU = &VADC_Params.Ua;
        VADC_Params.Ia = Itest;
        pTestDeg = &VADC_Params.degUa;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<0);
        m_ZabVect->setVisible(true);
        pTestZx = &m_Zx_state.Zab;
        pTestDegZx = &m_Zx_state.Deg_ab;
        NameZx = "РЗ АВ 1ст";
        pattern = _RZ_1ST_PATTERN;
     }

     if(ui->DZZbcradioButton->isChecked())
     {
        pTestU = &VADC_Params.Ub;
        VADC_Params.Ib = Itest;
        pTestDeg = &VADC_Params.degUb;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<1);
        m_ZbcVect->setVisible(true);
        pTestZx = &m_Zx_state.Zbc;
        pTestDegZx = &m_Zx_state.Deg_bc;
        NameZx = "РЗ ВС 1ст";
        pattern = _RZ_1ST_PATTERN;
     }

     if(ui->DZZcaradioButton->isChecked())
     {
        pTestU = &VADC_Params.Uc;
        VADC_Params.Ic = Itest;
        pTestDeg = &VADC_Params.degUc;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<2);
        m_ZcaVect->setVisible(true);
        pTestZx = &m_Zx_state.Zca;
        pTestDegZx = &m_Zx_state.Deg_ca;
        NameZx = "РЗ СА 1ст";
        pattern = _RZ_1ST_PATTERN;
     }

     //ДЗ 2ст
     if(ui->DZZab2radioButton->isChecked())
     {
        pTestU = &VADC_Params.Ua;
        VADC_Params.Ia = Itest;
        pTestDeg = &VADC_Params.degUa;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<3);
        m_ZabVect->setVisible(true);
        pTestZx = &m_Zx_state.Zab;
        pTestDegZx = &m_Zx_state.Deg_ab;
        NameZx = "РЗ АВ 2ст";
        pattern = _RZ_2ST_PATTERN;
     }

     if(ui->DZZbc2radioButton->isChecked())
     {
        pTestU = &VADC_Params.Ub;
        VADC_Params.Ib = Itest;
        pTestDeg = &VADC_Params.degUb;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<4);
        m_ZbcVect->setVisible(true);
        pTestZx = &m_Zx_state.Zbc;
        pTestDegZx = &m_Zx_state.Deg_bc;
        NameZx = "РЗ ВС 2ст";
        pattern = _RZ_2ST_PATTERN;
     }

     if(ui->DZZca2radioButton->isChecked())
     {
        pTestU = &VADC_Params.Uc;
        VADC_Params.Ic = Itest;
        pTestDeg = &VADC_Params.degUc;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<5);
        m_ZcaVect->setVisible(true);
        pTestZx = &m_Zx_state.Zca;
        pTestDegZx = &m_Zx_state.Deg_ca;
        NameZx = "РЗ СА 2ст";
        pattern = _RZ_2ST_PATTERN;
     }

     //ДЗ 3ст
     if(ui->DZZab3radioButton->isChecked())
     {
        pTestU = &VADC_Params.Ua;
        VADC_Params.Ia = Itest;
        pTestDeg = &VADC_Params.degUa;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<6);
        m_ZabVect->setVisible(true);
        pTestZx = &m_Zx_state.Zab;
        pTestDegZx = &m_Zx_state.Deg_ab;
        NameZx = "РЗ АВ 3ст";
        pattern = _RZ_3ST_PATTERN;
     }

     if(ui->DZZbc3radioButton->isChecked())
     {
        pTestU = &VADC_Params.Ub;
        VADC_Params.Ib = Itest;
        pTestDeg = &VADC_Params.degUb;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<7);
        m_ZbcVect->setVisible(true);
        pTestZx = &m_Zx_state.Zbc;
        pTestDegZx = &m_Zx_state.Deg_bc;
        NameZx = "РЗ ВС 3ст";
        pattern = _RZ_3ST_PATTERN;
     }

     if(ui->DZZca3radioButton->isChecked())
     {
        pTestU = &VADC_Params.Uc;
        VADC_Params.Ic = Itest;
        pTestDeg = &VADC_Params.degUc;
        pTestWorkReg = &m_mdfz_state.mdfz_reg3C;
        test_mask = (1<<8);
        m_ZcaVect->setVisible(true);
        pTestZx = &m_Zx_state.Zca;
        pTestDegZx = &m_Zx_state.Deg_ca;
        NameZx = "РЗ СА 3ст";
        pattern = _RZ_3ST_PATTERN;
     }

     need_phi_m4 = DrawErrorPatterns(pattern);

     m_IZMTimer->stop();
     block_time_sem->release(0);
     block_time_sem->tryAcquire(1, QDeadlineTimer(1));

     m_PatternVect->clear();
     m_ZabVect->clear();
     m_ZbcVect->clear();
     m_ZcaVect->clear();

     m_ZxaxisX->setMax(Ustart);
     m_ZxaxisY->setMax(Ustart);

     m_ZxaxisX->setMin(Ustart * (-1.0));
     m_ZxaxisY->setMin(Ustart * (-1.0));

     QApplication::processEvents();

     VADC_Params.en_VADC = true;
     mVADC->VADC_SetU(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);

     const QStringList headers = {tr("Угол"),tr("Сопротивление")};
     ui->ProcessZxlistWidget->clear();
     ui->ResultZxtableWidget->setRowCount(0);
     ui->ResultZxtableWidget->setHorizontalHeaderLabels(headers);

     ui->TestZxprogressBar->setValue(0);
     ui->TestZxprogressBar->setMaximum(deg_stop);
     ui->TestZxprogressBar->setVisible(true);

     for(quint16 i=deg_start; i< deg_stop; i+=deg_step )
     {

        *pTestU = Ustart;
        *pTestDeg = i;

         mVADC->VADC_SetI(&VADC_Params); //для восстановления режима, если пропала связь по USB и аппарат перезагрузили
        block_time_sem->tryAcquire(1, QDeadlineTimer(170));
        QApplication::processEvents();
         mVADC->VADC_SetU(&VADC_Params);
        block_time_sem->tryAcquire(1, QDeadlineTimer(170));
        QApplication::processEvents();

         ui->TestZxprogressBar->setValue(i);

        block_time_sem->tryAcquire(1, QDeadlineTimer(170));
        QApplication::processEvents();

        if(m_stop_test_Zx) break;

        while (*pTestU >= Ustop )
        {

            if(m_stop_test_Zx) break;

            mVADC->VADC_GetIzm(_MDFZ_IZM_ZX);
            block_time_sem->tryAcquire(1, QDeadlineTimer(170));
            QApplication::processEvents();

            mVADC->VADC_GetMDFZState();
            block_time_sem->tryAcquire(1, QDeadlineTimer(170));
            QApplication::processEvents();

            Zcur = *pTestU/Itest;
            if(*pTestWorkReg & test_mask)
            {
//                if(i==0) TestZ0deg = Zcur;
                if(Zcur>Zmax)
                {
                    Zmax = Zcur;
                    deg_phim4 = i;
                }
                DrawPatternPoint(m_PatternVect,*pTestZx,*pTestDegZx);
                //DrawPatternPoint(m_PatternVect,Zcur,i);

                ui->ResultZxtableWidget->insertRow(ui->ResultZxtableWidget->rowCount());
                ui->ResultZxtableWidget->setItem(ui->ResultZxtableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(i)));
                ui->ResultZxtableWidget->setItem(ui->ResultZxtableWidget->rowCount()-1,1,new QTableWidgetItem(QString::number(Zcur)));


                if(ui->BorderPatterncheckBox->isChecked()) break;

            }
            *pTestU -= Ustep;
            mVADC->VADC_SetU(&VADC_Params);

            ui->ProcessZxlistWidget->clear();
            ui->ProcessZxlistWidget->addItem("*** Текущие величины ***");
            ui->ProcessZxlistWidget->addItem("Текущее напряжение: " + QString::number(*pTestU) +" В" );
            ui->ProcessZxlistWidget->addItem("Текущий ток: " + QString::number(Itest) +" A" );
            ui->ProcessZxlistWidget->addItem("Текущее сопротивление: " + QString::number(Zcur) +" Ом" );
            ui->ProcessZxlistWidget->addItem("Текущий угол: " + QString::number(i) +" град" );
            block_time_sem->tryAcquire(1, QDeadlineTimer(170));
            QApplication::processEvents();
        }
     }

//Определение угла максимальной чувствительности по методу ОРГРЭС

     quint16 deg_off1 = 0;
     quint16 deg_off2 = 0;

     if(need_phi_m4)
     {
         *pTestU = Zmax*Itest*0.7f;

         *pTestDeg = deg_phim4;
         mVADC->VADC_SetU(&VADC_Params);

         block_time_sem->tryAcquire(1, QDeadlineTimer(200));
         QApplication::processEvents();

         for(quint16 i=deg_phim4; i< deg_stop; i++ )//=deg_step
         {
            if(m_stop_test_Zx) break;
            *pTestDeg = i;
            mVADC->VADC_SetU(&VADC_Params);
            block_time_sem->tryAcquire(1, QDeadlineTimer(170));
            QApplication::processEvents();
            mVADC->VADC_GetMDFZState();
         //   DrawPatternPoint(m_PatternVect,*pTestU,i);
            block_time_sem->tryAcquire(1, QDeadlineTimer(50));
            QApplication::processEvents();

            if(((*pTestWorkReg & test_mask) == 0))  {deg_off1 = i; break;}
         }

         *pTestDeg = deg_phim4;
         mVADC->VADC_SetU(&VADC_Params);

         block_time_sem->tryAcquire(1, QDeadlineTimer(200));
         QApplication::processEvents();

         mVADC->VADC_GetMDFZState();

         for(quint16 i=deg_phim4; i> 0; i-- )
         {
            if(m_stop_test_Zx) break;
            *pTestDeg = i;
            mVADC->VADC_SetU(&VADC_Params);
            block_time_sem->tryAcquire(1, QDeadlineTimer(170));
            QApplication::processEvents();
            mVADC->VADC_GetMDFZState();
         //   DrawPatternPoint(m_PatternVect,*pTestU,i);
            block_time_sem->tryAcquire(1, QDeadlineTimer(50));
            QApplication::processEvents();

            if(((*pTestWorkReg & test_mask) == 0))  {deg_off2 = i; break;}
         }

         deg_phim4 = (deg_off1+deg_off2) / 2;



         m_PhiM4Vect->clear();
         m_PhiM4Vect->setVisible(true);
         DrawZxVector(m_PhiM4Vect,Ustart,deg_phim4);

     }

     delete block_time_sem;

     ui->ProcessZxlistWidget->clear();
     ui->ProcessZxlistWidget->addItem(NameZx);
     ui->ProcessZxlistWidget->addItem("*** Результаты ***");
     ui->ProcessZxlistWidget->addItem("Максимальное сопротивление: " + QString::number(Zmax) +" Ом" );

     if(need_phi_m4)
     {
         ui->ProcessZxlistWidget->addItem("φ м.ч. (ОРГРЭС): " + QString::number(deg_phim4) +" град" );
         ui->ProcessZxlistWidget->addItem("φ выкл1: " + QString::number(deg_off1) +" град" );
         ui->ProcessZxlistWidget->addItem("φ выкл2: " + QString::number(deg_off2) +" град" );
     }

//     float deg_math = std::acos(TestZ0deg/Zmax)*180.0f/3.1415f;

//    ui->ProcessZxlistWidget->addItem("φ математ.: " + QString::number(deg_math) +" град" );

     ui->TestZxprogressBar->setVisible(false);

     VADC_Params.en_VADC = false;
     mVADC->VADC_SetU(&VADC_Params);

     m_IZMTimer->start(500);

     ui->pushButton->setEnabled(true);
     ui->pushButton_9->setEnabled(false);
}


void MainWindow::on_pushButton_3_clicked()
{

     QString filename= QFileDialog::getSaveFileName(this, tr("Сохранить файл результата как ..."), "Результат РС", tr("Файл результатов (*.txt)") );

     if(!filename.isNull())
     {
        QFile file(filename);
        quint16 n_row = ui->ResultZxtableWidget->rowCount();

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream writeStream(&file);

            writeStream << "Градусы   Сопротивление \n";
            writeStream << "----------------------- \n";

            for(quint16 i=0;i<n_row;i++)
            {
                writeStream << ui->ResultZxtableWidget->item(i,0)->text() + " ,  " + ui->ResultZxtableWidget->item(i,1)->text() + "\n";
            }
            writeStream << "----------------------- \n";
            if(ui->ProcessZxlistWidget->count() > 4)
            {
                writeStream << ui->ProcessZxlistWidget->item(1)->text() + " \n";
                writeStream << ui->ProcessZxlistWidget->item(0)->text() + " \n";
                writeStream << ui->ProcessZxlistWidget->item(2)->text() + " \n";
                writeStream << ui->ProcessZxlistWidget->item(3)->text() + " \n";
            }
            writeStream << "----------------------- \n";

        }

        file.close();
     }
}


void MainWindow::on_pushButton_4_clicked()
{
     QString work_part = qApp->applicationDirPath();
     QDir dir(work_part+"/Modes");

     if(dir.exists())
     {
        QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(dir.absolutePath())));
     }
}



void MainWindow::on_pushButton_2_clicked()
{
     mVADC->VADC_GetHWSettings(0,15);
}


void MainWindow::on_pushButton_5_clicked()
{
    QByteArray SettData;
    union tconvert_short sett;

    const quint16 count = ui->SettingstableWidget->rowCount();

    SettData.clear();

    for(uint8_t i=0;i<count;i++)
    {
        sett.shortval = ui->SettingstableWidget->item(i,1)->text().toUShort();
        SettData.append(sett.charval[_HIGH]);
        SettData.append(sett.charval[_LOW]);
    }


    mVADC->VADC_SetHWSettings(0,count,SettData);
}



void MainWindow::on_checkBox_toggled(bool checked)
{
    mVADC->VADC_SetMode(checked);
}


void MainWindow::on_pushButton_6_clicked()
{
    mVADC->VADC_ClrLOG();
}


void MainWindow::on_pushButton_8_clicked()
{
    mVADC->VADC_GetFW();
}


void MainWindow::on_pushButton_7_clicked()
{
    m_log_count = 0;
    ui->LOGlistWidget->clear();
    mVADC->VADC_GetLogCount();

    QEventLoop loop;
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timer.start(300);
    loop.exec();
    timer.stop();

    ui->LogReadprogressBar->setMaximum(m_log_count);
    ui->LogReadprogressBar->setValue(0);
    ui->LOGlistWidget->setVisible(false);

    ui->LogReadprogressBar->setVisible(true);

    for(quint16 i=0;i<m_log_count;i++)
    {
       mVADC->VADC_GetLOG(i);
       ui->LogReadprogressBar->setValue(i);
    }

    ui->LogReadprogressBar->setVisible(false);
    ui->LOGlistWidget->setVisible(true);

}



void MainWindow::on_pushButton_9_clicked()
{
    m_stop_test_Zx = true;
}


void MainWindow::on_RNM_pushButton_clicked()
{
    typedef void (VADC_Class::*pVADCfuncType)(float Ix,quint16 deg,struct _vadc_params *VADC_params);

    pVADCfuncType pCongUx = &VADC_Class::VADC_Config3U0;

    struct _vadc_params VADC_Params;

    QSemaphore *block_time_sem = new QSemaphore;

    quint16 *pTestWorkReg;
    quint16 test_mask;
    QString NameZx = "не выбрано";

    float Itest  = ui->IZxdoubleSpinBox->value();
    float Utest  = ui->UmaxZxdoubleSpinBox->value();
    quint16 deg_step = ui->DegStepspinBox->value();

    m_RM0RM2Vect->setVisible(true);
    m_ZabVect->setVisible(false);
    m_ZbcVect->setVisible(false);
    m_ZcaVect->setVisible(false);

    m_IZMTimer->stop();
    ui->pushButton->setEnabled(false);

    ui->TestZxprogressBar->setValue(0);
    ui->TestZxprogressBar->setMaximum(360);
    ui->TestZxprogressBar->setVisible(true);

    if(ui->RM0_radioButton->isChecked())
    {
       pTestWorkReg = &m_mdfz_state.mdfz_reg1;
       test_mask = (1<<7);
       NameZx = "РНМ М0";

       VADC_Params.Ia = Itest;
       VADC_Params.degIa = 0;
       VADC_Params.U3U0 = Utest;
       VADC_Params.deg3U0 = 0;

       pCongUx = &VADC_Class::VADC_Config3U0;

    }
    if(ui->RM2_radioButton->isChecked())
    {
       pTestWorkReg = &m_mdfz_state.mdfz_reg1;
       test_mask = (1<<8);
       NameZx = "РНМ М2";

       mVADC->VADC_ConfigU2(Utest,0,&VADC_Params);
       mVADC->VADC_ConfigI2(Itest,0,&VADC_Params);

       pCongUx = &VADC_Class::VADC_ConfigU2;
    }

    m_PatternVect->clear();

    const QStringList headers = {tr("Угол"),tr("Срабатывание")};
    ui->ProcessZxlistWidget->clear();
    ui->ResultZxtableWidget->setRowCount(0);
    ui->ResultZxtableWidget->setHorizontalHeaderLabels(headers);

     VADC_Params.en_VADC = true;
     mVADC->VADC_SetU(&VADC_Params);
     mVADC->VADC_SetI(&VADC_Params);

    for(quint16 i=0;i<360;i+=deg_step)
    {
       (mVADC->*pCongUx)(Utest,i,&VADC_Params);
       mVADC->VADC_SetU(&VADC_Params);
       mVADC->VADC_SetI(&VADC_Params);

       ui->ProcessZxlistWidget->clear();
       ui->ProcessZxlistWidget->addItem("*** Текущие величины ***");
       ui->ProcessZxlistWidget->addItem("Текущий угол: " + QString::number(i) +" град" );

       block_time_sem->tryAcquire(1, QDeadlineTimer(70));
       QApplication::processEvents();
       mVADC->VADC_GetMDFZState();

       DrawZxVector(m_RM0RM2Vect,Utest,i);

       QString deg_work = "-";
       if(*pTestWorkReg & test_mask)
       {
            DrawPatternPoint(m_PatternVect, Utest,i);
            deg_work = "+";
       }

       ui->ResultZxtableWidget->insertRow(ui->ResultZxtableWidget->rowCount());
       ui->ResultZxtableWidget->setItem(ui->ResultZxtableWidget->rowCount()-1,0,new QTableWidgetItem(QString::number(i)));
       ui->ResultZxtableWidget->setItem(ui->ResultZxtableWidget->rowCount()-1,1,new QTableWidgetItem(deg_work));

       ui->TestZxprogressBar->setValue(i);
    }

    VADC_Params.en_VADC = false;
    mVADC->VADC_SetU(&VADC_Params);
    mVADC->VADC_SetI(&VADC_Params);

     m_RM0RM2Vect->setVisible(false);

    ui->TestZxprogressBar->setVisible(false);
    ui->ProcessZxlistWidget->clear();

    ui->pushButton->setEnabled(true);
    m_IZMTimer->start();

    delete block_time_sem;
}


void MainWindow::on_pushButton_10_clicked()
{
    TestTHRProcedure();
}


void MainWindow::on_ClearModepushButton_clicked()
{
    ui->TestModeslistWidget->clear();
}


void MainWindow::on_AddModepushButton_clicked()
{
    quint16 n = ui->ModeslistWidget->selectedItems().count();

    for(quint16 i=0;i< n;i++)
    {
       ui->TestModeslistWidget->addItem(ui->ModeslistWidget->selectedItems().at(i)->text());
    }
}


void MainWindow::on_DelModepushButton_clicked()
{
    QList<QListWidgetItem*> items = ui->TestModeslistWidget->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
       delete ui->TestModeslistWidget->takeItem(ui->TestModeslistWidget->row(item));
    }
}


void MainWindow::on_StartModepushButton_clicked()
{
    QSemaphore *block_time_sem = new QSemaphore;
    struct _vadc_params VADC_Params;
    quint16 save_manip_state;
    QByteArray buff;
    union tconvert_short sett;

    mVADC->VADC_GetHWSettings(_CONTR_PRD,1);

    save_manip_state = mHWSettings[_CONTR_PRD];

    buff.clear();

    if(ui->ManipEnModecheckBox->isChecked())
    {
       sett.shortval = mHWSettings[_CONTR_PRD] | 1;
       buff.append(sett.charval[_HIGH]);
       buff.append(sett.charval[_LOW]);

    } else
    {
       sett.shortval = mHWSettings[_CONTR_PRD] & 0xFFFE;
       buff.append(sett.charval[_HIGH]);
       buff.append(sett.charval[_LOW]);
    }

    mVADC->VADC_SendReset();
    block_time_sem->tryAcquire(1, QDeadlineTimer(10));
    mVADC->VADC_SetMode(true);
    block_time_sem->tryAcquire(1, QDeadlineTimer(10));
    mVADC->VADC_SetHWSettings(_CONTR_PRD,1,buff);

    QString filename;

    quint16 test_time = ui->TestModespinBox->text().toShort();
    quint16 n = ui->TestModeslistWidget->count();

    ui->TestModeslistWidget->clearSelection();

    block_time_sem->tryAcquire(1, QDeadlineTimer(1));

    for(quint16 i=0;i<n;i++)
    {
       filename = qApp->applicationDirPath() +"/Modes/" + ui->TestModeslistWidget->item(i)->text();
       mSettings->ReadSettings(filename,&VADC_Params);
       mVADC->VADC_SetU(&VADC_Params);
       mVADC->VADC_SetI(&VADC_Params);
       block_time_sem->tryAcquire(1, QDeadlineTimer(1));
       ui->TestModeslistWidget->setCurrentRow(i);
       QApplication::processEvents();
       block_time_sem->tryAcquire(1, QDeadlineTimer(test_time));
    }

    ui->TestModeslistWidget->clearSelection();

    delete block_time_sem;

    buff.clear();
    sett.shortval = save_manip_state;
    buff.append(sett.charval[_HIGH]);
    buff.append(sett.charval[_LOW]);
    mVADC->VADC_SetHWSettings(_CONTR_PRD,1,buff);
    mVADC->VADC_SetMode(false);
}



void MainWindow::on_pushButton_11_clicked()
{
    mVADC->VADC_SendReset();
}


void MainWindow::on_CalcADCspinBox_valueChanged(int arg1)
{
    ui->CalcADCHexlineEdit->setText("0x"+QString::number(arg1,16));
}


void MainWindow::on_CalcToIzmpushButton_clicked()
{
    quint16 adc = ui->CalcADCspinBox->value();

    float turn_count = m_kNomCur[ui->CalcCurrValcheckBox->isChecked()];

    ui->CalcUxdoubleSpinBox->setValue(adc*m_kU);
    ui->CalcIxdoubleSpinBox->setValue(adc*m_kI/turn_count);
    ui->CalcZxdoubleSpinBox->setValue(adc*turn_count*m_kR);

}


void MainWindow::on_CalcUToADCpushButton_clicked()
{
    quint32 u_adc = quint32(quint32(ui->CalcUxdoubleSpinBox->value()/m_kU));

    quint16 thr_max = u_adc + (u_adc >> 4);
    quint16 thr_min = u_adc - (u_adc >> 4);

     ui->CalcADCspinBox->setValue(u_adc);

    ui->THRmaxlineEdit->setText(QString::number(thr_max));
    ui->THRminlineEdit->setText(QString::number(thr_min));
}


void MainWindow::on_CalcIToADCpushButton_clicked()
{
    float turn_count = m_kNomCur[ui->CalcCurrValcheckBox->isChecked()];

    quint32 i_adc = quint32(ui->CalcIxdoubleSpinBox->value()*turn_count/m_kI);

    quint16 thr_max = i_adc + (i_adc >> 4);
    quint16 thr_min = i_adc - (i_adc >> 4);

    ui->CalcADCspinBox->setValue(i_adc);

    ui->THRmaxlineEdit->setText(QString::number(thr_max));
    ui->THRminlineEdit->setText(QString::number(thr_min));
}


void MainWindow::on_CalcRToADCpushButton_clicked()
{
    float turn_count = m_kNomCur[ui->CalcCurrValcheckBox->isChecked()];
    quint32 z_adc = quint32(ui->CalcZxdoubleSpinBox->value()/turn_count/m_kR);

    quint16 thr_max = z_adc + (z_adc >> 4);
    quint16 thr_min = z_adc - (z_adc >> 4);

    ui->CalcADCspinBox->setValue(z_adc);

    ui->THRmaxlineEdit->setText(QString::number(thr_max));
    ui->THRminlineEdit->setText(QString::number(thr_min));
}


void MainWindow::on_GetOscpushButton_clicked()
{
    QTime time;

    ui->OsclistWidget->clear();
    ui->OsclistWidget->addItem("Старт " + time.currentTime().toString("hh:mm:ss"));
    quint8 num_osc = ui->NumOscspinBox->value() - 1;

    ui->OSCprogressBar->setValue(0);

    // DrawOSC(num_osc);

    // return;

    if(OSC_Buff[num_osc][MDFZ_OSC_MAX_MESS_COUNT-1].isLoad)
    {
        //выводим осц
        ui->OsclistWidget->addItem("Уже загружена ");
        DrawOSC(num_osc);
    } else
    {
        //загружаем из аппарата
        m_IZMTimer->stop();

        for(quint16 i=0;i<MDFZ_OSC_MAX_MESS_COUNT;i++)
        {
            OSC_Buff[num_osc][i].isLoad = false;
            OSC_Buff[num_osc][i].data.clear();
            mVADC->VADC_SendGetOSC(num_osc+1,i*8);
        }
    }
}


void MainWindow::on_OSC_MINhorizontalSlider_valueChanged(int value)
{
    osc_maxisX->setRange(value,ui->OSC_MAXhorizontalSlider->value());
    ui->OSC_MAXhorizontalSlider->setMinimum(value);
}


void MainWindow::on_OSC_MAXhorizontalSlider_valueChanged(int value)
{
    osc_maxisX->setRange(ui->OSC_MINhorizontalSlider->value(),value);
    ui->OSC_MINhorizontalSlider->setMaximum(value);
}


void MainWindow::on_StopOSCpushButton_clicked()
{

}

// unsigned short TestMass[3] = {0x1122,0x3344,0x5566};

void MainWindow::on_pushButton_12_clicked()
{
    // unsigned char val;

    // unsigned char *pval;

    // pval = (unsigned char *)TestMass;

    // for (int i = 0; i < 6; ++i) {

    // val = *pval++;

    // ui->OMPlistWidget->addItem(QString::number(val));
    // }
}


void MainWindow::on_pushButton_13_clicked()
{
    mVADC->VADC_CLRData();
    m_osc_chart->setVisible(true);
}


void MainWindow::on_pushButton_14_clicked()
{
     mVADC->VADC_SendGetOSC(16,0);
}



void MainWindow::on_pushButton_18_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetModulesState();

}


void MainWindow::on_pushButton_19_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetString();
}


void MainWindow::on_pushButton_20_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetGOOSEState();
}


void MainWindow::on_pushButton_21_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetGOOSEOutNames(0);
}


void MainWindow::on_pushButton_22_clicked()
{

}

void MainWindow::on_pushButton_23_clicked() //GetPhase
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetHF(2);
}


void MainWindow::on_pushButton_24_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetHF(1);
}


void MainWindow::on_pushButton_25_clicked()
{
    ui->LOGlistWidget->clear();

    mVADC->VADC_SendGetHF(0);
}


void MainWindow::on_pushButton_26_clicked()
{
    mVADC->VADC_GetDate();
}



void MainWindow::on_pushButton_28_clicked() //debug button
{
    //
}


void MainWindow::on_pushButton_29_clicked()
{
    mVADC->VADC_GetCompState();
}


void MainWindow::on_WirepushButton_clicked(bool checked)
{
    emit signalProcessWireConnection(checked);
}


void MainWindow::on_pushButton_30_clicked()
{
    QString work_part = qApp->applicationDirPath();
    QDir dir(work_part);

    if(dir.exists())
    {
        QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(dir.absolutePath())));
    }
}


void MainWindow::on_UpdateVADCModepushButton_clicked()
{
    mVADC->VADC_GetU();
    mVADC->VADC_GetI();
}


void MainWindow::on_VN1pushButton_clicked()
{
    mVADC->VADC_SendVButton(_VBUTTON,0,0);
}


void MainWindow::on_VN2pushButton_clicked()
{
    mVADC->VADC_SendVButton(_VBUTTON,1,0);
}


void MainWindow::on_VN3pushButton_clicked()
{
    mVADC->VADC_SendVButton(_VBUTTON,2,0);
}


void MainWindow::on_VN4pushButton_clicked()
{
    mVADC->VADC_SendVButton(_VBUTTON,3,0);
}


void MainWindow::on_F1pushButton_clicked()
{

}


void MainWindow::on_F1pushButton_pressed()
{
    mVADC->VADC_SendVButton(_FBUTTON,F1_BUTT,1);
}


void MainWindow::on_F1pushButton_released()
{
    mVADC->VADC_SendVButton(_FBUTTON,F1_BUTT,0);
}


void MainWindow::on_F2pushButton_clicked()
{

}

void MainWindow::on_F2pushButton_pressed()
{
    mVADC->VADC_SendVButton(_FBUTTON,F2_BUTT,1);
}

void MainWindow::on_F2pushButton_released()
{
    mVADC->VADC_SendVButton(_FBUTTON,F2_BUTT,0);
}


void MainWindow::on_F3pushButton_pressed()
{
    mVADC->VADC_SendVButton(_FBUTTON,F3_BUTT,1);
}


void MainWindow::on_F3pushButton_released()
{
    mVADC->VADC_SendVButton(_FBUTTON,F3_BUTT,0);
}


void MainWindow::on_F4pushButton_pressed()
{
    mVADC->VADC_SendVButton(_FBUTTON,F4_BUTT,1);
}


void MainWindow::on_F4pushButton_released()
{
    mVADC->VADC_SendVButton(_FBUTTON,F4_BUTT,0);
}


void MainWindow::on_LRpushButton_clicked()
{
    mVADC->VADC_SendVButton(_FBUTTON,FLR_BUTT,0);
}



void MainWindow::on_checkBox_2_clicked()
{

}


void MainWindow::on_pushButton_38_clicked()
{
    for(quint8 i=0;i<128;i++)
    {
        m_RBACcheckBox[i]->setCheckState(Qt::Checked);
    }
}

void MainWindow::on_pushButton_39_clicked()
{
    for(quint8 i=0;i<128;i++)
    {
        m_RBACcheckBox[i]->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::on_RBACCheckBoxProcessState(Qt::CheckState)
{

}

void MainWindow::on_CalcCurrValcheckBox_toggled(bool checked)
{

}

void MainWindow::on_pushButton_40_clicked()
{
    bool ok;
    quint64 RBAC_role = ui->lineEdit_h->text().toULongLong(&ok,16);

    for(quint8 i=0;i<64;i++)
    {
        bool is_checked = (bool)(RBAC_role & (quint64(1)<<i));
        m_RBACcheckBox[i]->setChecked(is_checked);
    }

    RBAC_role = ui->lineEdit_l->text().toULongLong(&ok,16);

    for(quint8 i=0;i<64;i++)
    {
        bool is_checked = (bool)(RBAC_role & (quint64(1)<<i));
        m_RBACcheckBox[i+64]->setChecked(is_checked);
    }

}


void MainWindow::on_pushButton_41_clicked()
{
    quint64 RBAC_role = 0;
    bool isCheck = false;
    for(quint8 i=0;i<64;i++)
    {
        isCheck = m_RBACcheckBox[i]->isChecked();
        RBAC_role += (isCheck * powl(2,i));
    }

    QString paddedString = QStringLiteral("%1").arg(RBAC_role,16, 16, QLatin1Char('0'));
    ui->RBACRolelineEdit->setText("0x"+ paddedString);
    // ui->RBACRolelineEdit->setText("0x"+ QString::number(RBAC_role,16));

    RBAC_role = 0;

    for(quint8 i=64;i<128;i++)
    {
        isCheck = m_RBACcheckBox[i]->isChecked();
        RBAC_role += (isCheck * powl(2,i-64));
    }
    paddedString = QStringLiteral("%1").arg(RBAC_role,16, 16, QLatin1Char('0'));
    ui->RBACRolelineEdit2->setText("0x"+ paddedString);
}



void MainWindow::on_pushButton_42_clicked()
{
    mModbusDFZ->SetServerIP(ui->ModbusIPlineEdit->text());
}

void MainWindow::slotProcessRcvDFZRegData(QList<quint16> reg_data)
{
    bool is_checked;
    bool is_change;
    const QString const_style[2] = {"QCheckBox { color: black; }","QCheckBox { color: red; }"};

    for(quint8 i=3;i<5;i++)
    {
        for(quint8 j=0;j<8;j++)
        {
            is_checked = (bool)(reg_data[i] & (1<<j));
            is_change = is_checked ^ m_MursReleState[(i-3)*8+j]->isChecked();
            m_MursReleState[(i-3)*8+j]->setChecked(is_checked);
            m_MursReleState[(i-3)*8+j]->setStyleSheet(const_style[is_change]);
        }
    }

    for(quint8 i=8;i<17;i++)
    {
        for(quint8 j=0;j<16;j++)
        {
            is_checked = (bool)(reg_data[i] & (1<<j));
            is_change = is_checked ^ m_MDFZRegscheckBox[(i-8)*16+j]->isChecked();
            m_MDFZRegscheckBox[(i-8)*16+j]->setChecked(is_checked);
            m_MDFZRegscheckBox[(i-8)*16+j]->setStyleSheet(const_style[is_change]);
        }
    }
}
