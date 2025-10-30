#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadc_class.h"
#include "HWSettings.h"
#include "settings_class.h"
#include "OSC.h"
#include "crc16.h"
#include <QFile>
#include <QDesktopServices>

struct _osc_buff OSC_Buff[MDFZ_OSC_COUNT][MDFZ_OSC_MAX_MESS_COUNT];

void MainWindow::ConfigOSC_Chart(void)
{
    m_osc_chart = new QChart();
    QChartView *chartView = new QChartView(this);
    ui->horizontalLayout_5->addWidget(chartView);

    //mchart->setGeometry(0,0,420,420);
    m_osc_chart->setTheme(QChart::ChartThemeBlueCerulean);
    m_osc_chart->setTitle(tr("Осциллограмма"));
    m_osc_chart->setAnimationOptions(QChart::NoAnimation);

    osc_maxisX = new QValueAxis;
    osc_maxisX->setLabelFormat("%i");
    osc_maxisX->setTitleText(tr(" "));
    osc_maxisX->setRange(0.0,MDFZ_OSC_MAX_SAMPLES);
    m_osc_chart->addAxis(osc_maxisX, Qt::AlignBottom);


    osc_maxisY = new QValueAxis;
    osc_maxisY->setLabelFormat("%i");
    osc_maxisY->setTitleText(tr(" "));
    osc_maxisY->setRange(-32768.0,32768.0);
    m_osc_chart->addAxis(osc_maxisY, Qt::AlignLeft);

    osc_UaVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::darkYellow);
    osc_UaVect->setName("Ua");
    osc_UbVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::darkGreen);
    osc_UbVect->setName("Ub");
    osc_UcVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::darkRed);
    osc_UcVect->setName("Uc");

    osc_IaVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::yellow);
    osc_IaVect->setName("Ia");
    osc_IbVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::green);
    osc_IbVect->setName("Ib");
    osc_IcVect = CreateVector(m_osc_chart,osc_maxisX,osc_maxisY,Qt::red);
    osc_IcVect->setName("Ic");

    chartView->setChart(m_osc_chart);
}

void MainWindow::ShowOMPData(quint8 num_osc)
{
    QMap<quint16,QString> sTypeKZ;
    struct _OMP_data *pOMPdata;

    if(num_osc==16) return; //AK

     ui->OMPlistWidget->clear();

    if(OSC_Buff[num_osc][2755].isLoad == false)
     {
         ui->OMPlistWidget->addItem("Данные ОМП не загружены");

        return;
     }

    sTypeKZ[0] = "Нет информации о типе КЗ";
    sTypeKZ[1] = "A0";
    sTypeKZ[2] = "B0";
    sTypeKZ[4] = "C0";
    sTypeKZ[8] = "AB0";
    sTypeKZ[16] = "BC0";
    sTypeKZ[32] = "CA0";
    sTypeKZ[64] = "AB";
    sTypeKZ[128] = "BC";
    sTypeKZ[256] = "CA";
    sTypeKZ[512] = "ABC";


    pOMPdata = (struct _OMP_data *) &OSC_Buff[num_osc][2755].data;


    ui->OMPlistWidget->addItem("Данные ОМП:");
    ui->OMPlistWidget->addItem(" ");

     ui->OMPlistWidget->addItem("Тип КЗ : " + sTypeKZ[pOMPdata->type_kz & 0x03FF]);
    ui->OMPlistWidget->addItem("Расстояние до Т1 = " + QString::number(pOMPdata->length_T1));
    ui->OMPlistWidget->addItem("Расстояние до Т2 = " + QString::number(pOMPdata->length_T2));
    ui->OMPlistWidget->addItem("3U0 = " + QString::number(pOMPdata->val_3U0/100.0));
    ui->OMPlistWidget->addItem("3I0 = " + QString::number(pOMPdata->val_3I0/100.0));
    ui->OMPlistWidget->addItem("U2 = " + QString::number(pOMPdata->val_U2/100.0));
    ui->OMPlistWidget->addItem("I2 = " + QString::number(pOMPdata->val_I2/100.0));
}

void MainWindow::DrawOSC(quint8 num_osc)
{
    void *pVoidPointer;
    struct _part_osc_sample_type *pOsc_sample;

    QList<QPointF> data_osc_list;
    QPointF osc_point;
    quint16 x;

    osc_maxisX->setRange(0.0,MDFZ_OSC_MAX_SAMPLES);
    osc_maxisY->setRange(-32768.0,32768.0);

    QLineSeries *ArrayOscSeries[6] = {osc_UaVect,osc_UbVect,osc_UcVect,osc_IaVect,osc_IbVect,osc_IcVect};

    ui->OSC_DrawProgrlabel->setVisible(true);

    ui->OSC_DrawProgrlabel->setText("Вывод графика 0%");

    for(quint16 j=0;j<6;j++)
    {
        ArrayOscSeries[j]->setVisible(false);
        ArrayOscSeries[j]->setUseOpenGL(ui->OpenGlcheckBox->isChecked());
        ArrayOscSeries[j]->clear();
    }



    for(quint16 i=0;i<MDFZ_OSC_MAX_MESS_COUNT;i++)
    {
        pVoidPointer = (void *)OSC_Buff[num_osc][i].data.data();
        pOsc_sample = (struct _part_osc_sample_type *) pVoidPointer;

        x = i*8;
        for(quint16 j=0;j<6;j++)
        {
            data_osc_list.clear();
            osc_point.setX(x);
            osc_point.setY((qint16_be)pOsc_sample->data1_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+1);
            osc_point.setY((qint16_be)pOsc_sample->data2_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+2);
            osc_point.setY((qint16_be)pOsc_sample->data3_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+3);
            osc_point.setY((qint16_be)pOsc_sample->data4_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+4);
            osc_point.setY((qint16_be)pOsc_sample->data5_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+5);
            osc_point.setY((qint16_be)pOsc_sample->data6_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+6);
            osc_point.setY((qint16_be)pOsc_sample->data7_osc[j]);
            data_osc_list.append(osc_point);
            osc_point.setX(x+7);
            osc_point.setY((qint16_be)pOsc_sample->data8_osc[j]);
            data_osc_list.append(osc_point);

            ArrayOscSeries[j]->append(data_osc_list);
        }

        ui->OSC_DrawProgrlabel->setText("Вывод графика " + QString::number(i*100/MDFZ_OSC_MAX_MESS_COUNT) +"%");

        if(i & 1) //уменьшаем количество вызовов processEvent
        {
            QApplication::processEvents();
        }

    }

    for(quint16 j=0;j<6;j++)
    {
        ArrayOscSeries[j]->setUseOpenGL(false);
        ArrayOscSeries[j]->setVisible(true);

    }

    ui->OSC_DrawProgrlabel->setVisible(false);

    ShowOMPData(num_osc);

    m_IZMTimer->start(500);
}


void MainWindow::slotFillGetOsc(QByteArray osc_data)
{
    quint8 num_osc;
    quint16 num_mess;
    unsigned short val_crc16 = 0;
    void *pVoidPointer;
    struct _part_osc_sample_type *pOsc_sample;

    if(osc_data.length() == 273)
    {
        pVoidPointer = (void *)osc_data.data();
        pOsc_sample = (struct _part_osc_sample_type *) pVoidPointer;

        val_crc16 = CRC16(&osc_data,271);
        if(val_crc16 == pOsc_sample->crc16_osc)
        {
            num_osc = pOsc_sample->num_osc-1;
            num_mess = pOsc_sample->start_num_sample_osc/8;

            OSC_Buff[num_osc][num_mess].data.append(osc_data);
            OSC_Buff[num_osc][num_mess].isLoad = true;

            ui->OSCprogressBar->setValue(pOsc_sample->start_num_sample_osc);

            if(pOsc_sample->start_num_sample_osc == MDFZ_OSC_MAX_SAMPLE_NUMB)
            {
               QTime time;
               ui->OsclistWidget->addItem("Стоп  " + time.currentTime().toString("hh:mm:ss"));
               DrawOSC(num_osc);
            }

        } else
        {
            ui->OsclistWidget->addItem("crc ");
            //здесь можно поставить на повтор запрос данных

            // OSC_Buff[num_osc][num_mess].data.clear();
            // OSC_Buff[num_osc][num_mess].isLoad = false;
        }
        //
    }
}

void MainWindow::slotFillGetOscAK(QByteArray osc_data)
{
    QLineSeries *ArrayOscSeries[6] = {osc_UaVect,osc_UbVect,osc_UcVect,osc_IaVect,osc_IbVect,osc_IcVect};

    union tbits8 ak_data;

    osc_maxisX->setRange(0.0,512);
    osc_maxisY->setRange(0,10.0);

    for(quint16 i=0;i<6;i++)
    {
        ArrayOscSeries[i]->clear();
    }

    for(quint16 i=0;i<500;i++)
    {
        ak_data.byte_data = osc_data.at(13+i);
        unsigned char Vch_ogib = (ak_data.single_bits.bit0+ak_data.single_bits.bit1) * ak_data.single_bits.bit0;
        osc_UaVect->append(i,Vch_ogib);
        osc_UbVect->append(i,ak_data.single_bits.bit2+2.5);// плюс смещение по высоте на графике
        osc_UcVect->append(i,ak_data.single_bits.bit3+4.0);
        osc_IaVect->append(i,ak_data.single_bits.bit4+5.5);
        osc_IbVect->append(i,ak_data.single_bits.bit5+6.0);
        osc_IcVect->append(i,ak_data.single_bits.bit6+8.5);
    }

}

void MainWindow::slotFillGetSpectr(QByteArray osc_data)
{
    ui->LOGlistWidget->addItem("Spectr data  : ");

    qDebug()  << osc_data.length();

    for(unsigned char i=0;i<osc_data.length();i++)
    {
        ui->LOGlistWidget->addItem(QString::number(osc_data.at(i)));
    }
}

void MainWindow::slotFillGetHF(QByteArray osc_data)
{
    ui->LOGlistWidget->addItem("Phase data  : ");

    qDebug()  << osc_data.length();

    for(unsigned char i=0;i<osc_data.length();i++)
    {
        ui->LOGlistWidget->addItem(QString::number(osc_data.at(i)));
    }
}

//OSC_Buff[MDFZ_OSC_COUNT][MDFZ_OSC_MAX_MESS_COUNT];

// Функция для сжатия данных с использованием алгоритма RLE
void compressOscData(char *input, unsigned char num_osc)
{
    QString work_part = qApp->applicationDirPath();
    QDir dir(work_part+"/Osc");

    QFile osc_file (work_part+"/Osc/osc"+QString::number(num_osc)+"_compressed.dat");
    osc_file.open (QIODevice :: WriteOnly);
    QDataStream out (& osc_file);


    int count, i;

    QByteArray raw_data;
    raw_data.clear();

    for(int i = 0; i< MDFZ_OSC_MAX_MESS_COUNT;i++)
    {
        raw_data.append(OSC_Buff[0][i].data);

    }

    int length = raw_data.size();

    qDebug() << length;

    union _osc_compressed_data osc_data;

    for (i = 0; i < length; i++)
    {
        // Подсчитываем количество повторений текущего символа
        count = 1;
        while ( (i < (length - 1)) && (raw_data[i] == raw_data[i + 1]) )
        {
            count++;
            i++;
        }
        // Записываем символ и количество его повторений в выходную строку

        osc_data.part_data.length = count;
        osc_data.part_data.byte = raw_data[i];
        out << osc_data.full_data;
    }

    osc_file.close();
}

void MainWindow::on_pushButton_16_clicked()
{
    QString work_part = qApp->applicationDirPath();
    QDir dir(work_part+"/Osc");

    if(!dir.exists())
    {
        dir.mkdir(work_part+"/Osc");
    }

    QFile osc_file (work_part+"/Osc/osc"+QString::number(ui->NumOscspinBox->value())+".dat");
    osc_file.open (QIODevice :: WriteOnly);
    QDataStream out (& osc_file);

    for(quint32 i=0;i<MDFZ_OSC_MAX_MESS_COUNT;i++)
    {
        out << OSC_Buff[ui->NumOscspinBox->value()-1][i].data;
    }

    osc_file.close();

    compressOscData(&OSC_Buff[ui->NumOscspinBox->value()-1][0].data[0],ui->NumOscspinBox->value());

    QDesktopServices::openUrl(QUrl(QUrl::fromLocalFile(dir.absolutePath())));

}

void MainWindow::on_pushButton_17_clicked()
{
    union _osc_compressed_data osc_data;
    union tconvert_int int_osc;

    QString work_part = qApp->applicationDirPath();

    QFile osc_file (work_part+"/Osc/osc1_compressed"+".dat");
    QFile osc_dec_file (work_part+"/Osc/osc1_decompressed"+".dat");
   // QDataStream out (&osc_dec_file);

    osc_file.open (QIODevice :: ReadOnly);
    osc_dec_file.open (QIODevice :: WriteOnly);

    int file_size = osc_file.bytesAvailable() >> 2;

    QByteArray data;

    int process = 0;

    for(int i = 0; i< file_size; i++) //
    {
        data.clear();
        data = osc_file.read(4);

        //qDebug() << data;

        int_osc.char_val[_HIGH2] = data.at(0);
        int_osc.char_val[_LOW2]  = data.at(1);
        int_osc.char_val[_HIGH]  = data.at(2);
        int_osc.char_val[_LOW]   = data.at(3);

        osc_data.full_data = int_osc.int_val;

        // qDebug() << osc_data.part_data.length;
        // qDebug() << osc_data.part_data.byte;
        // qDebug() << "+++";

        int dl =  osc_data.part_data.length;
        char raw_data = osc_data.part_data.byte;

        for(int j = 0; j< dl; j++)
        {
            osc_dec_file.putChar(osc_data.part_data.byte);

           // out.writeRawData(&raw_data,1);
            process++;
            //out << osc_data.part_data.byte;
            //qDebug() << osc_data.part_data.byte;
        }
    }


    //QThread::sleep(5);

    osc_file.close();
    osc_dec_file.close();

    qDebug() << process;

    ui->OMPlistWidget->addItem(QString::number(file_size));

}
