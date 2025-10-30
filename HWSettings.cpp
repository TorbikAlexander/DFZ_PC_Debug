#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadc_class.h"
#include "HWSettings.h"
#include "settings_class.h"
#include "OSC.h"


#include <QDir>
#include <QFileDialog>
#include <QListWidget>
#include <QtCore>
#include <QObject>
#include <QTableWidgetItem>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include <QSqlQueryModel>

void MainWindow::CreateHWSettingsTable()
{
    const QStringList headers = {tr("Настройка"),tr("Значение")};
    ui->SettingstableWidget->clear();
    ui->SettingstableWidget->setColumnCount(2); // Указываем число колонок
    ui->SettingstableWidget->setRowCount(15);
    ui->SettingstableWidget->setShowGrid(true); // Включаем сетку
    ui->SettingstableWidget->setHorizontalHeaderLabels(headers);
    ui->SettingstableWidget->setColumnWidth(0,200);

    for(quint16 i=0;i< 15;i++)
    {
        ui->SettingstableWidget->setItem(i,0,new QTableWidgetItem(Settheaders.at(i)));
        ui->SettingstableWidget->setItem(i,1,new QTableWidgetItem("---"));
    }
}


void MainWindow::slotFillHWSettingsTable(QByteArray SettData)
{
    QTableWidgetItem *item;
    union tconvert_short start;
    union tconvert_short count;
    union tconvert_short sett;

    start.charval[_HIGH] = SettData.at(4);
    start.charval[_LOW]  = SettData.at(5);
    count.charval[_HIGH] = SettData.at(6);
    count.charval[_LOW]  = SettData.at(7);

//    if((start.shortval + count.shortval)>10)
//    {
//        qDebug() << "Старт: " + QString::number(start.shortval);
//        qDebug() << "Количество: " + QString::number(count.shortval);
//        return;
//    }

    for(uint16_t i=0;i<count.shortval;i++)
    {
        sett.charval[_HIGH] = SettData.at(2*i+8);
        sett.charval[_LOW]  = SettData.at(2*i+9);

        mHWSettings[start.shortval+i] = sett.shortval;
    }


    for(uint8_t i=0;i<15;i++)
    {
        item = ui->SettingstableWidget->item(i,1);
        item->setText(QString::number(mHWSettings[i]));
    }
}

void MainWindow::slotFillHWTime(QByteArray time_arr)
{
    union tconvert_int ak_time;
    QString hw_time_str = "Время: ";

    hw_time_str += QString::number(time_arr.at(4)) + " : ";
    hw_time_str += QString::number(time_arr.at(5)) + " : ";
    hw_time_str += QString::number(time_arr.at(6));

    ui->time_label->setText(hw_time_str);

    ak_time.char_val[_HIGH2] = time_arr.at(7);
    ak_time.char_val[_LOW2]  = time_arr.at(8);
    ak_time.char_val[_HIGH] = time_arr.at(9);
    ak_time.char_val[_LOW]  = time_arr.at(10);

    ui->ak_time_label->setText("Время АК : " + QString::number(ak_time.int_val) + " c");
}

void MainWindow::slotFillHWDate(QByteArray date_arr)
{
    QString hw_date_str = "Дата: ";

    hw_date_str += QString::number(date_arr.at(4)) + " / ";
    //hw_date_str += QString::number(date_arr.at(5)) + " / ";
    hw_date_str += QString::number(date_arr.at(6)) + " / ";
    hw_date_str += QString::number(date_arr.at(7)+2000);

    ui->date_label->setText(hw_date_str);
}

QString MainWindow::GetLogString(quint8 I1,quint8 I2,quint8 I3,quint8 I4)
{
    QString LogString = "";
    QSqlQuery query;

    if(m_logdb.open())
    {
        query.clear();

        QString sort_select="SELECT StringLOG  FROM Log WHERE ";

        sort_select += "( I1 = " + QString::number(I1) + " ) AND ";
        sort_select += "( I2 = " + QString::number(I2) + " ) AND ";
        sort_select += "( I3 = " + QString::number(I3) + " ) AND ";
        sort_select += "( I4 = " + QString::number(I4) + " ); ";

        if(query.exec(sort_select))
        {
            query.next();
            LogString = query.value(0).toString();
        }

        m_logdb.close();
    }

    if(LogString == "")
    {
        LogString = QString::number(I1)+"-" + QString::number(I2)+"-" + QString::number(I3)+"-"+ QString::number(I4);
    }

    return LogString+"-";
}

void MainWindow::slotFillHWLOG(QByteArray log_buff)
{
    union tconvert_short temp;
    QString numlog_string = "";
    QString log_string = "";
    QString time_string = "";
    quint8 I1 = log_buff.at(8);
    quint8 I2 = log_buff.at(9);
    quint8 I3 = log_buff.at(10);
    quint8 I4 = log_buff.at(11);

    for(quint8 i=12;i<18;i++)
    {
        time_string+= QString::number(log_buff.at(i)) + "-";
    }

    time_string+=QString::number(log_buff.at(18)*256+log_buff.at(19));

    temp.charval[_HIGH] = log_buff.at(4);
    temp.charval[_LOW] = log_buff.at(5);
    numlog_string = QString::number(temp.shortval)+ "-";
    log_string = numlog_string+GetLogString(I1,I2,I3,I4) + time_string;


    ui->LOGlistWidget->addItem(log_string);
}

void MainWindow::slotFillHWFW(QByteArray fw_buff)
{
    union tconvert_short module_fw;
     const QString mod_names[7] = {"МУ","МВ","МУРС","МДФЗ","МОИ","ЛП","ПК"};
    //const QString modules_name[8] = {"МВ1 : ","МУРС1 : ","МУРС2 : ","МУ М4 : ","МУ А9 : ","МУ ПЛИС : ","МК МЗ : ","ПЛИС МЗ : "};

    ui->FWlistWidget->clear();

    unsigned char modules_count = fw_buff.at(4);

    for(quint8 i=0;i<modules_count;i++)
    {
        ui->FWlistWidget->addItem("*******");
        ui->FWlistWidget->addItem("module id = "+QString::number(fw_buff.at(5+i*4)) + " - " + mod_names[fw_buff.at(5+i*4) & 0x07]);
        ui->FWlistWidget->addItem("module num = "+QString::number(fw_buff.at(6+i*4)));
        ui->FWlistWidget->addItem("module fw = "+QString::number(fw_buff.at(7+i*4)*256+fw_buff.at(8+i*4)));
        ui->FWlistWidget->addItem("      ");

    }

    // for(quint8 i=0;i<8;i++)
    // {
    //     module_fw.charval[_HIGH] = fw_buff.at(2*i+4);
    //     module_fw.charval[_LOW] = fw_buff.at(2*i+5);
    //     ui->FWlistWidget->addItem(modules_name[i] + QString::number(module_fw.shortval/100.0));
    // }

}

void MainWindow::slotFillLEDS(QByteArray leds_buff)
{
    const QString icon_str[2] = {"help-about","weather-clear"};
    for(quint8 i=0;i<5;i++)
    {
        m_LEDs.sr[i] = leds_buff.at(i+4);

        m_VButtLEDs.sr[i] = leds_buff.at(i+10);
    }

    ui->VN1pushButton->setIcon(QIcon::fromTheme(icon_str[(bool)(m_VButtLEDs.data_bits.LED1)]));
    ui->VN2pushButton->setIcon(QIcon::fromTheme(icon_str[(bool)(m_VButtLEDs.data_bits.LED2)]));
    ui->VN3pushButton->setIcon(QIcon::fromTheme(icon_str[(bool)(m_VButtLEDs.data_bits.LED3)]));
    ui->VN4pushButton->setIcon(QIcon::fromTheme(icon_str[(bool)(m_VButtLEDs.data_bits.LED4)]));

}

void MainWindow::slotFillLogCount(QByteArray log_count)
{
    union tconvert_short count;
    count.charval[_HIGH] = log_count.at(6);
    count.charval[_LOW] = log_count.at(7);
    m_log_count = count.shortval;


    ui->LOGlistWidget->addItem("Количество событий: "+QString::number(m_log_count));

    union tconvert_short osc_count;
    osc_count.charval[_HIGH] = log_count.at(4);
    osc_count.charval[_LOW] = log_count.at(5);

    ui->LOGlistWidget->addItem("Количество осциллограмм: "+QString::number(osc_count.shortval >> 4));

}

void MainWindow::slotFillMUHelth(QByteArray MUHelth)
{
    quint8 mu_load = 0;
    union tconvert_short ram_use = {0};
    union tconvert_int time_on = {0};

    time_on.char_val[_HIGH2] = MUHelth.at(4);
    time_on.char_val[_LOW2]  = MUHelth.at(5);
    time_on.char_val[_HIGH]  = MUHelth.at(6);
    time_on.char_val[_LOW]   = MUHelth.at(7);
    mu_load = MUHelth.at(8);
    ram_use.charval[_HIGH] = MUHelth.at(9);
    ram_use.charval[_LOW]  = MUHelth.at(10);

    ui->CPLoadlabel->setText("Загрузка ЦП : "+QString::number(mu_load)+" %");
    ui->MemMUlabel->setText("Память : "+QString::number(ram_use.shortval)+" кБ");
    ui->OnTimeMUlabel->setText("Время работы : "+QString::number(time_on.int_val)+" с");
}

void MainWindow::slotFillModuleStateEx(QByteArray mod_states)
{
    const QString mod_names[7] = {"МУ","МВ","МУРС","МДФЗ","МОИ","ЛП","ПК"};
    unsigned char mod_states_count = mod_states.at(4);

    ui->LOGlistWidget->addItem("Modules count : "+ QString::number(mod_states_count));

    for(unsigned char i=0;i<mod_states_count;i++)
    {
        ui->LOGlistWidget->addItem(" *****  ");
        ui->LOGlistWidget->addItem("m_type:"+ QString::number(mod_states.at(5+i*4)) + " - " + mod_names[mod_states.at(5+i*4) & 0x07]);
        ui->LOGlistWidget->addItem("m_number:"+ QString::number(mod_states.at(6+i*4)));
        ui->LOGlistWidget->addItem("m_state:"+ QString::number(mod_states.at(7+i*4)*256+mod_states.at(8+i*4)));
        ui->LOGlistWidget->addItem("   ");
    }
}

void MainWindow::slotFillString(QByteArray mod_string)
{
   // qDebug() << mod_string;

     ui->LOGlistWidget->addItem("RCV String  : ");

     for(unsigned char i=0;i<mod_string.at(6);i++)
     {
         ui->LOGlistWidget->addItem(QString::number(mod_string.at(7+i)));
     }

     if( ( mod_string.at(4)==4) && ( mod_string.at(6)>1))
     {
         mVADC->VADC_SendGetGOOSEOutNames(mod_string.at(5)+1);
     }

}

void MainWindow::slotFillGooseState(QByteArray mod_goose)
{
    qDebug() << mod_goose;

    ui->LOGlistWidget->clear();
    ui->LOGlistWidget->addItem("RCV Goose msg : ");

    for(quint8 i = 0; i< mod_goose.length(); i++)
    {
         ui->LOGlistWidget->addItem("0x" + QString::number(mod_goose.at(i),16));
    }

}

void MainWindow::slotFillCompState(QByteArray data)
{
    const QString COMP_LED_PICT[2]    = {":/res/main/Pictures/rabota0.png",   ":/res/main/Pictures/rabota1.png"};
    union tbits8 comp_state;

    comp_state.byte_data = data.at(4);

    ui->comp_osn_label->setPixmap(QPixmap(COMP_LED_PICT[comp_state.single_bits.bit0]));
    ui->comp_high_label->setPixmap(QPixmap(COMP_LED_PICT[comp_state.single_bits.bit1]));
    ui->comp_low_label->setPixmap(QPixmap(COMP_LED_PICT[comp_state.single_bits.bit2]));
    ui->comp_ak_label->setPixmap(QPixmap(COMP_LED_PICT[comp_state.single_bits.bit3]));

}

void MainWindow::slotFillVADCParam(QByteArray data)
{
    struct _vadc_params VADC_Params;
    union tconvert_short adc_data;

    ReadCurrentVADCParams(&VADC_Params);

    quint8 type_out = data.at(5);

    if(type_out == _VADC_OUT_VOLTAGE)
    {
        VADC_Params.en_VADC = data.at(4) & 1;
        VADC_Params.syncUa  = (data.at(4) >>4) & 1;

        adc_data.charval[_HIGH] = data.at(6);
        adc_data.charval[_LOW] = data.at(7);
        VADC_Params.Ua = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(8);
        adc_data.charval[_LOW] = data.at(9);
        VADC_Params.degUa = adc_data.shortval;

        adc_data.charval[_HIGH] = data.at(10);
        adc_data.charval[_LOW] = data.at(11);
        VADC_Params.Ub = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(12);
        adc_data.charval[_LOW] = data.at(13);
        VADC_Params.degUb = adc_data.shortval;

        adc_data.charval[_HIGH] = data.at(14);
        adc_data.charval[_LOW] = data.at(15);
        VADC_Params.Uc = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(16);
        adc_data.charval[_LOW] = data.at(17);
        VADC_Params.degUc = adc_data.shortval;

        adc_data.charval[_HIGH] = data.at(18);
        adc_data.charval[_LOW] = data.at(19);
        VADC_Params.U3U0 = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(20);
        adc_data.charval[_LOW] = data.at(21);
        VADC_Params.deg3U0 = adc_data.shortval;

        LoadCurrentVADCParams(&VADC_Params);
    }

    if(type_out == _VADC_OUT_CURRENT)
    {
        VADC_Params.en_VADC = data.at(4) & 1;
        VADC_Params.syncUa  = (data.at(4) >>4) & 1;

        adc_data.charval[_HIGH] = data.at(6);
        adc_data.charval[_LOW] = data.at(7);
        VADC_Params.Ia = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(8);
        adc_data.charval[_LOW] = data.at(9);
        VADC_Params.degIa = adc_data.shortval;

        adc_data.charval[_HIGH] = data.at(10);
        adc_data.charval[_LOW] = data.at(11);
        VADC_Params.Ib = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(12);
        adc_data.charval[_LOW] = data.at(13);
        VADC_Params.degIb = adc_data.shortval;

        adc_data.charval[_HIGH] = data.at(14);
        adc_data.charval[_LOW] = data.at(15);
        VADC_Params.Ic = (float)( adc_data.shortval)/100.0f;
        adc_data.charval[_HIGH] = data.at(16);
        adc_data.charval[_LOW] = data.at(17);
        VADC_Params.degIc = adc_data.shortval;

        LoadCurrentVADCParams(&VADC_Params);
    }

}


