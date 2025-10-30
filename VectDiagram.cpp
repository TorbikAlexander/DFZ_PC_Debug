
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vadc_class.h"
#include "settings_class.h"

#include <QtCharts/QChart>
//QT_CHARTS_USE_NAMESPACE;
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QPolarChart>
#include <QScatterSeries>


void MainWindow::ShowGraph(void)
{
    //auto *chart = new QPolarChart();
    mchart = new QChart();
    QChartView *chartView = new QChartView(this);
    ui->horizontalLayout_2->addWidget(chartView);

    mchart->setGeometry(0,0,420,420);
    mchart->setTheme(QChart::ChartThemeBlueCerulean);
    mchart->setTitle(tr("Векторная диаграмма"));
    mchart->setAnimationOptions(QChart::NoAnimation);

    //    auto angularAxis = new QValueAxis;
    //    angularAxis->setTickCount(90.0); // First and last ticks are co-located on 0/360 angle.
    //    angularAxis->setLabelFormat("%i");
    //    angularAxis->setShadesVisible(true);
    //    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    //    angularAxis->setMin(0.0);
    //    angularAxis->setMax(360.0);
    //    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    //    auto radialAxis = new QValueAxis;
    //    radialAxis->setTickCount(20.0);
    //    radialAxis->setLabelFormat("%d");
    //    radialAxis->setMin(0.0);
    //    radialAxis->setMax(100.0);
    //    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    QFont font;
    font.setPixelSize(15);
    mchart->setTitleFont(font);

    maxisX = new QValueAxis;
    maxisX->setLabelFormat("%i");
    maxisX->setTitleText(tr(" "));
    maxisX->setRange(-100.0,100.0);
    mchart->addAxis(maxisX, Qt::AlignBottom);


    maxisY = new QValueAxis;
    maxisY->setLabelFormat("%i");
    maxisY->setTitleText(tr(" "));
    maxisY->setRange(-100.0,100.0);
    mchart->addAxis(maxisY, Qt::AlignLeft);


    mUaVect = CreateVector(mchart,maxisX,maxisY,Qt::darkYellow);
    mUaVect->setName("Ua");
    mUbVect = CreateVector(mchart,maxisX,maxisY,Qt::darkGreen);
    mUbVect->setName("Ub");
    mUcVect = CreateVector(mchart,maxisX,maxisY,Qt::darkRed);
    mUcVect->setName("Uc");

    mIaVect = CreateVector(mchart,maxisX,maxisY,Qt::yellow);
    mIaVect->setName("Ia");
    mIbVect = CreateVector(mchart,maxisX,maxisY,Qt::green);
    mIbVect->setName("Ib");
    mIcVect = CreateVector(mchart,maxisX,maxisY,Qt::red);
    mIcVect->setName("Ic");


    DrawVector(mUaVect,57.0,0);
    DrawVector(mUbVect,57.0,240);
    DrawVector(mUcVect,57.0,120);

    DrawVector(mIaVect,10.0,0);
    DrawVector(mIbVect,10.0,240);
    DrawVector(mIcVect,10.0,120);

    chartView->setChart(mchart);
}

QLineSeries * MainWindow::CreateVector(QChart *xchart,QValueAxis *xA,QValueAxis *yA,QColor color)
{
    QLineSeries *Vect = new QLineSeries();

    xchart->addSeries(Vect);

    Vect->setColor(color);
    Vect->clear();
    Vect->attachAxis(xA);
    Vect->attachAxis(yA);

    return Vect;
}

void MainWindow::DrawVector(QLineSeries *Vect,float Ampl, qint16 deg)
{
    float x,y;
    float xs,ys,Ampls;
    const float Pi_div_180 = 3.1415/180.0;

    deg += 90;
    if(Ampl > 100.0f) Ampl = 100.0f;
    x = Ampl*cos(deg*Pi_div_180);
    y = Ampl*sin(deg*Pi_div_180);
    Vect->clear();
    Vect->append(0.0f,0.0f);
    Vect->append(x,y);

    //стрелка
    if(Ampl>3.0)
    {
        Ampls = Ampl - 3.0;
        xs = Ampls*cos((deg+1)*Pi_div_180);
        ys = Ampls*sin((deg+1)*Pi_div_180);
        Vect->append(xs,ys);

        if(deg>0)
        {
            xs = Ampls*cos((deg-1)*Pi_div_180);
            ys = Ampls*sin((deg-1)*Pi_div_180);
        } else
        {
            xs = Ampls*cos((359)*Pi_div_180);
            ys = Ampls*sin((359)*Pi_div_180);
        }
        Vect->append(xs,ys);
        Vect->append(x,y);
    }
}

void MainWindow::ShowIZMGraph(void)
{
    m_izmchart = new QChart();
    QChartView *chartView = new QChartView(this);
    ui->horizontalLayout_3->addWidget(chartView);

    m_izmchart->setGeometry(0,0,420,420);
    m_izmchart->setTheme(QChart::ChartThemeBlueCerulean);
    m_izmchart->setTitle(tr("Измерения"));
    m_izmchart->setAnimationOptions(QChart::NoAnimation);

    QFont font;
    font.setPixelSize(15);
    m_izmchart->setTitleFont(font);

    m_izmaxisX = new QValueAxis;
    m_izmaxisX->setLabelFormat("%i");
    m_izmaxisX->setTitleText(tr(" "));
    m_izmaxisX->setRange(-100.0,100.0);
    m_izmchart->addAxis(m_izmaxisX, Qt::AlignBottom);


    m_izmaxisY = new QValueAxis;
    m_izmaxisY->setLabelFormat("%i");
    m_izmaxisY->setTitleText(tr(" "));
    m_izmaxisY->setRange(-100.0,100.0);
    m_izmchart->addAxis(m_izmaxisY, Qt::AlignLeft);


    m_izmUaVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::darkYellow);
    m_izmUaVect->setName("Ua");
    m_izmUbVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::darkGreen);
    m_izmUbVect->setName("Ub");
    m_izmUcVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::darkRed);
    m_izmUcVect->setName("Uc");
    m_izmIaVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::yellow);
    m_izmIaVect->setName("Ia");
    m_izmIbVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::green);
    m_izmIbVect->setName("Ib");
    m_izmIcVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::red);
    m_izmIcVect->setName("Ic");


    m_izmZabVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::yellow);
    m_izmZabVect->setVisible(false);
    m_izmZbcVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::green);
    m_izmZbcVect->setVisible(false);
    m_izmZcaVect= CreateVector(m_izmchart,m_izmaxisX,m_izmaxisY,Qt::red);
    m_izmZcaVect->setVisible(false);


    DrawVector(m_izmUaVect,0.0,0);
    DrawVector(m_izmUbVect,0.0,0);
    DrawVector(m_izmUcVect,0.0,0);

    DrawVector(m_izmIaVect,0.0,0);
    DrawVector(m_izmIbVect,0.0,0);
    DrawVector(m_izmIcVect,0.0,0);

    chartView->setChart(m_izmchart);
}

void MainWindow::ShowZxGraph(void)
{
    m_Zxchart = new QChart();
    QChartView *chartView = new QChartView(this);
    ui->horizontalLayout_4->addWidget(chartView);

    m_Zxchart->setGeometry(0,0,500,500);
    m_Zxchart->setTheme(QChart::ChartThemeBlueCerulean);
    m_Zxchart->setTitle(tr("Характеристика РС/РНМ"));
    m_Zxchart->setAnimationOptions(QChart::NoAnimation);

    QFont font;
    font.setPixelSize(15);
    m_izmchart->setTitleFont(font);

    m_ZxaxisX = new QValueAxis;
    m_ZxaxisX->setLabelFormat("%i");
    m_ZxaxisX->setTitleText(tr(" R, Ом "));
    m_ZxaxisX->setRange(-100.0,100.0);
    m_Zxchart->addAxis(m_ZxaxisX, Qt::AlignBottom);


    m_ZxaxisY = new QValueAxis;
    m_ZxaxisY->setLabelFormat("%i");
    m_ZxaxisY->setTitleText(tr(" jX, Ом "));
    m_ZxaxisY->setRange(-100.0,100.0);
    m_Zxchart->addAxis(m_ZxaxisY, Qt::AlignLeft);


    m_ZabVect= CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::yellow);
    m_ZabVect->setName("Z_AB");
    m_ZabVect->setVisible(true);
    m_ZbcVect= CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::green);
    m_ZbcVect->setName("Z_BC");
    m_ZbcVect->setVisible(false);
    m_ZcaVect= CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::red);
    m_ZcaVect->setName("Z_CA");
    m_ZcaVect->setVisible(false);
    m_PhiM4Vect = CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::darkRed);
    m_PhiM4Vect->setName("φ м.ч.");
    m_PhiM4Vect->setVisible(false);
    m_RM0RM2Vect = CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::darkBlue);
    m_RM0RM2Vect->setName("РНМх");
    m_RM0RM2Vect->setVisible(false);

    m_ErrorPVect = CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::gray);
    m_ErrorPVect->setName("ErrP");
    m_ErrorPVect->setVisible(false);
    m_ErrorMVect = CreateVector(m_Zxchart,m_ZxaxisX,m_ZxaxisY,Qt::gray);
    m_ErrorMVect->setName("ErrM");
    m_ErrorMVect->setVisible(false);

    m_PatternVect = new QScatterSeries();
    m_Zxchart->addSeries(m_PatternVect);

    m_PatternVect->setColor(Qt::cyan);
    m_PatternVect->setBorderColor(Qt::cyan);
    m_PatternVect->setName("IOx work");
    m_PatternVect->setMarkerSize(3);
    m_PatternVect->clear();
    m_PatternVect->attachAxis(m_ZxaxisX);
    m_PatternVect->attachAxis(m_ZxaxisY);

    DrawZxVector(m_ZabVect,0.0,0);
    DrawZxVector(m_ZbcVect,0.0,0);
    DrawZxVector(m_ZcaVect,0.0,0);

    chartView->setChart(m_Zxchart);
}

void MainWindow::DrawZxVector(QLineSeries *Vect,float Ampl, qint16 deg)
{
    float x,y;
    float xs,ys,Ampls;

    const float Pi_div_180 = 3.1415/180.0;

    if(Ampl > 100.0f) Ampl = 100.0f;
    x = Ampl*cos(deg*Pi_div_180);
    y = Ampl*sin(deg*Pi_div_180);
    Vect->clear();
    Vect->append(0.0f,0.0f);
    Vect->append(x,y);

    //стрелка
    if(Ampl>3.0)
    {
        Ampls = Ampl - 3.0;
        xs = Ampls*cos((deg+1)*Pi_div_180);
        ys = Ampls*sin((deg+1)*Pi_div_180);
        Vect->append(xs,ys);

        if(deg>0)
        {
        xs = Ampls*cos((deg-1)*Pi_div_180);
        ys = Ampls*sin((deg-1)*Pi_div_180);
        } else
        {
        xs = Ampls*cos((359)*Pi_div_180);
        ys = Ampls*sin((359)*Pi_div_180);
        }
        Vect->append(xs,ys);
        Vect->append(x,y);
    }
}

void MainWindow::DrawPatternPoint(QScatterSeries *series,float Ampl, qint16 deg)
{
    float x,y;
    if(Ampl > 100.0f) Ampl = 100.0f;
    x = Ampl*cos(deg*3.1415/180);
    y = Ampl*sin(deg*3.1415/180);
    series->append(x,y);
}
