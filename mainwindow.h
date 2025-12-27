#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "OSC.h"
#include "HWSettings.h"
#include "qcheckbox.h"
#include "tflexport.h"
#include "vadc_class.h"
#include "settings_class.h"
//#include "HWSettings.h"
#include "tlogiccell.h"

#include <QtCharts/QChart>
//QT_CHARTS_USE_NAMESPACE;
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLineSeries>
#include <QPolarChart>
#include <QListWidget>
#include <QObject>
#include <QScatterSeries>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;}
QT_END_NAMESPACE


struct _mdfz_state
{
    quint16 mdfz_reg0;
    quint16 mdfz_reg1;
    quint16 mdfz_reg2;
    quint16 mdfz_reg2E;
    quint16 mdfz_reg3A;
    quint16 mdfz_reg3B;
    quint16 mdfz_reg3C;
    quint16 mdfz_regKT;
};

struct _mdfz_Zx_state
{
    float Zab;
    quint16 Deg_ab;
    float Zbc;
    quint16 Deg_bc;
    float Zca;
    quint16 Deg_ca;

};

enum _ZX_Pattern
{
    _DFZ_AB_PATTERN,
    _DFZ_BC_PATTERN,
    _DFZ_CA_PATTERN,
    _RZ_1ST_PATTERN,
    _RZ_2ST_PATTERN,
    _RZ_3ST_PATTERN
};

union _led_status
{
    unsigned char sr[5];
    struct sr_bits
    {
        unsigned  LED_24V:2;
        unsigned  LED_PUSK_UM:2;
        unsigned  LED_COMP_OSN:2;
        unsigned  LED_AK:2;
        unsigned  LED_VVOD_V4Z:2;
        unsigned  LED_VVOD_RZ:2;
        unsigned  LED_VVOD_KA:2;
        unsigned  LED_PREDUPR:2;
        unsigned  LED_AVAR:2;
        unsigned  LED_MANIPUL:2;
        unsigned  LED_OSTANOV:2;
        unsigned  LED_PUSK_PRD:2;
        unsigned  LED_PUSK_RZ:2;
        unsigned  LED_PUSK_V4Z:2;
        unsigned  LED_WORK_V4Z:2;
        unsigned  LED_WORK_RZ:2;
        unsigned  LED_KA:2;
        unsigned  LED_UROV:2;
        unsigned  LED_BLOCK_PRD:2;
        unsigned  LED_RESERV:2;
    } data_bits;
};

union _com_led_status
{
    unsigned char sr[5];
    struct sr_bits
    {
        unsigned  LED1:2;
        unsigned  LED2:2;
        unsigned  LED3:2;
        unsigned  LED4:2;
        unsigned  LED5:2;
        unsigned  LED6:2;
        unsigned  LED7:2;
        unsigned  LED8:2;
        unsigned  LED9:2;
        unsigned  LED10:2;
        unsigned  LED11:2;
        unsigned  LED12:2;
        unsigned  LED13:2;
        unsigned  LED14:2;
        unsigned  LED15:2;
        unsigned  LED16:2;
        unsigned  LED17:2;
        unsigned  LED18:2;
        unsigned  LED19:2;
        unsigned  LED20:2;
    } data_bits;
};

/*
00-не горит
01-мигает 2x
10-мигает
11-светится непрерывно
*/
enum _lp_led_states
{
    LP_LED_OFF,
    LP_LED_FAST_BLINK,
    LP_LED_BLINK,
    LP_LED_ON
};

enum _vbutt_type
{
    _VBUTTON,
    _FBUTTON
};

enum _fbutt_enum
{
    F1_BUTT,
    F2_BUTT,
    F3_BUTT,
    F4_BUTT,
    F5_BUTT,
    F6_BUTT,
    F7_BUTT,
    F8_BUTT,
    F9_BUTT,
    F10_BUTT,
    F11_BUTT,
    F12_BUTT,
    FLR_BUTT,

    FBUTT_SIGN_COUNT

};

union tbits8
{
    unsigned char byte_data;
    struct {
        unsigned bit0:1;
        unsigned bit1:1;
        unsigned bit2:1;
        unsigned bit3:1;
        unsigned bit4:1;
        unsigned bit5:1;
        unsigned bit6:1;
        unsigned bit7:1;
    } single_bits;
};

union tbits16
{
    unsigned short short_data;
    unsigned char byte_data[2];
    struct {
        unsigned bit0:1;
        unsigned bit1:1;
        unsigned bit2:1;
        unsigned bit3:1;
        unsigned bit4:1;
        unsigned bit5:1;
        unsigned bit6:1;
        unsigned bit7:1;
        unsigned bit8:1;
        unsigned bit9:1;
        unsigned bit10:1;
        unsigned bit11:1;
        unsigned bit12:1;
        unsigned bit13:1;
        unsigned bit14:1;
        unsigned bit15:1;
    } single_bits;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ReadCurrentVADCParams(struct _vadc_params *VADC_Params);
    void LoadCurrentVADCParams(struct _vadc_params *VADC_Params);

    void ShowGraph(void);
    QLineSeries * CreateVector(QChart *xchart,QValueAxis *xA,QValueAxis *yA,QColor color);
    void DrawVector(QLineSeries *Vect,float Ampl, qint16 deg);
    void DrawZxVector(QLineSeries *Vect,float Ampl, qint16 deg);
    void ShowIZMGraph(void);
    void ShowZxGraph(void);
    void DrawPatternPoint(QScatterSeries *series,float Ampl, qint16 deg);

    void CreateIZMTable(void);

    bool GetConnectionState(void);
    
    void CreateZxResTable();

    void CreateHWSettingsTable();
    QString GetLogString(quint8 I1,quint8 I2,quint8 I3,quint8 I4);

    void TestTHRProcedure(void);
    bool DrawErrorPatterns(quint16 pattern);
    void DrawCirclePatterns(quint16 Zx,quint16 phi_m4,quint8 k_ellipt,quint8 offset);
    void DrawPolyPatterns(quint16 sett_start,quint16 sett_count);

    void ConfigOSC_Chart(void);
    void ShowOMPData(quint8 num_osc);
    void DrawOSC(quint8 num_osc);

    void ConfigScene(void);
    void ConfigFlexLogic(void);
    void DrawLCPicture(quint8 LCType);
    TLogicCell* AddLogicCell(quint16 number,quint8 type, float x,float y);
    TFlexPort*  AddPort(quint16 sign_type,QString sign_name, float x,float y);
    void FillLCSettings(_flex_logic_config sett);
    _flex_logic_config ReadLCSettings(void);

    void UpdateParamLCFromCheckBox(void);
    void SaveFlexProject(void);
    void OpenFlexProject(void);


private:
    void FillProgInstructionsByStack(void);
    TLogicCell *GetPrevSRC(QObject *src_elem,QStack <TLogicCell * > *prog_stack);

public slots:
    void slotFillIZMTable(QByteArray);
    void slotFillStateMDFZTable(QByteArray);
    void slotFillHWSettingsTable(QByteArray);
    void slotConnectStateProcedure(bool);
    void slotFillHWTime(QByteArray);
    void slotFillHWDate(QByteArray);
    void slotFillHWLOG(QByteArray);
    void slotFillHWFW(QByteArray);
    void slotFillLEDS(QByteArray);
    void slotFillLogCount(QByteArray);
    void slotFillGetOsc(QByteArray);
    void slotFillGetOscAK(QByteArray);
    void slotFillGetSpectr(QByteArray osc_data);
    void slotFillGetHF(QByteArray);

    void slotFillMUHelth(QByteArray);
    void slotFillModuleStateEx(QByteArray);

    void slotFillString(QByteArray);
    void slotFillGooseState(QByteArray);
    void slotFillCompState(QByteArray);
    void slotFillVADCParam(QByteArray);

    void slotSetLineCoord(QObject *pCell,bool finish_coord, QPointF coord,quint16 num_input);
    void slotSceneUpdate(void);
    void slotEditFlexLC(QObject *pCell,_flex_logic_config sett);

    void slotScene_MousePress(QGraphicsSceneMouseEvent *event);

    void slotDebugLETimerProcedure();

    void slotRcvLEState(QByteArray);

private slots:

    void on_Save_pushButton_clicked();

    void on_Load_pushButton_clicked();

    void on_SendParams_pushButton_clicked();

    void on_Ua_doubleSpinBox_valueChanged(double arg1);

    void on_UaDeg_spinBox_valueChanged(int arg1);

    void on_Ub_doubleSpinBox_valueChanged(double arg1);

    void on_UbDeg_spinBox_valueChanged(int arg1);

    void on_Uc_doubleSpinBox_valueChanged(double arg1);

    void on_UcDeg_spinBox_valueChanged(int arg1);

    void on_Ia_doubleSpinBox_valueChanged(double arg1);

    void on_IaDeg_spinBox_valueChanged(int arg1);

    void on_Ib_doubleSpinBox_valueChanged(double arg1);

    void on_IbDeg_spinBox_valueChanged(int arg1);

    void on_Ic_doubleSpinBox_valueChanged(double arg1);

    void on_IcDeg_spinBox_valueChanged(int arg1);

    void on_UpdateModepushButton_clicked();

    void on_ModeslistWidget_itemDoubleClicked(QListWidgetItem *item);

    void slotPingTimerProcedure();

    void slotIzmTimerProcedure();

    void slotLEDSTimerProcedure();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();


    void on_checkBox_toggled(bool checked);

    void on_pushButton_6_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_7_clicked();


    void on_pushButton_9_clicked();

    void on_RNM_pushButton_clicked();

    void on_pushButton_10_clicked();

    void on_ClearModepushButton_clicked();

    void on_AddModepushButton_clicked();

    void on_DelModepushButton_clicked();

    void on_StartModepushButton_clicked();

    void on_pushButton_11_clicked();


    void on_CalcADCspinBox_valueChanged(int arg1);

    void on_CalcToIzmpushButton_clicked();

    void on_CalcUToADCpushButton_clicked();

    void on_CalcIToADCpushButton_clicked();

    void on_CalcRToADCpushButton_clicked();

    void on_GetOscpushButton_clicked();

    void on_OSC_MINhorizontalSlider_valueChanged(int value);

    void on_OSC_MAXhorizontalSlider_valueChanged(int value);

    void on_pushButton_12_clicked();

    void on_StopOSCpushButton_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_AddORpushButton_clicked();

    void on_AddANDpushButton_clicked();

    void on_AddXORpushButton_clicked();

    void on_WirepushButton_clicked();

    void on_PortpushButton_clicked();

    void on_FlexSettCloseupushButton_clicked();

    void on_FlexSettOKpushButton_clicked();

    void on_FlexSettCancelpushButton_clicked();

    void on_LogicSigncomboBox_currentIndexChanged(int index);

    void on_FlexSeqSizespinBox_valueChanged(int arg1);

    void on_UpdateFlexSeqpushButton_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_26_clicked();

    void on_pushButton_27_clicked();

    void on_pushButton_28_clicked();

    void on_pushButton_29_clicked();

    void on_WirepushButton_clicked(bool checked);

    void on_pushButton_30_clicked();

    void on_ConfigWMpushButton_clicked();

    void on_wm_inst_count_spinBox_valueChanged(int arg1);

    void on_UpdateVADCModepushButton_clicked();

    void on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint);

    void on_SceneCleapushButton_clicked();

    void on_pushButton_31_clicked();

    void on_LCPram4spinBox_valueChanged(int arg1);

    void on_LCPram3spinBox_valueChanged(int arg1);

    void on_LCPram2spinBox_valueChanged(int arg1);

    void on_LCPram1spinBox_valueChanged(int arg1);

    void on_SaveprojpushButton_clicked();

    void on_OpenProjpushButton_clicked();

    void on_pushButton_34_clicked();

    void on_SaveAspushButton_clicked();

    void on_VN1pushButton_clicked();

    void on_VN2pushButton_clicked();

    void on_VN3pushButton_clicked();

    void on_VN4pushButton_clicked();

    void on_F1pushButton_clicked();

    void on_F1pushButton_pressed();

    void on_F1pushButton_released();

    void on_F2pushButton_clicked();

    void on_F2pushButton_released();

    void on_F3pushButton_pressed();

    void on_F3pushButton_released();

    void on_F4pushButton_pressed();

    void on_F4pushButton_released();

    void on_F2pushButton_pressed();

    void on_LRpushButton_clicked();

    void on_checkBox_2_checkStateChanged(const Qt::CheckState &arg1);

    void on_checkBox_2_clicked();

    void on_pushButton_38_clicked();

    void on_CalcCurrValcheckBox_toggled(bool checked);

    void on_RBACCheckBoxProcessState(Qt::CheckState);

    void on_pushButton_39_clicked();

    void on_pushButton_40_clicked();

    void on_pushButton_41_clicked();

signals:

    void signalFinishedAll(void);
    void signalProcessWireConnection(bool);
    void signalSetDebugMode(bool);

private:
    Ui::MainWindow *ui;

    VADC_Class *mVADC;
    Settings_class *mSettings;
    bool m_usb_connect = false;
    bool m_stop_test_Zx = true;

    struct _mdfz_state m_mdfz_state;
    struct _mdfz_Zx_state m_Zx_state;
    union _led_status m_LEDs;
    union _com_led_status m_VButtLEDs;
    quint16 m_log_count;
    QSqlDatabase m_logdb;

    quint16 mHWSettings[_SETTINGS_COUNT];

    const float m_kU = (11.5 * 1.85 * 10.0)/(32768.0);
    const float m_kI = (25110.0*2.4)/(32768.0*270);
    const float m_kR = (11.5*1.85*270.0)/(2.4*2511.0*128.0);
    const float m_kNomCur[2] = {5.0,1.0};

    QChart *mchart;
    QValueAxis *maxisX;
    QValueAxis *maxisY;
    QLineSeries *mUaVect;
    QLineSeries *mUbVect;
    QLineSeries *mUcVect;
    QLineSeries *mIaVect;
    QLineSeries *mIbVect;
    QLineSeries *mIcVect;

    QChart *m_izmchart;
    QValueAxis *m_izmaxisX;
    QValueAxis *m_izmaxisY;
    QLineSeries *m_izmUaVect;
    QLineSeries *m_izmUbVect;
    QLineSeries *m_izmUcVect;
    QLineSeries *m_izmIaVect;
    QLineSeries *m_izmIbVect;
    QLineSeries *m_izmIcVect;
    QLineSeries *m_izmZabVect;
    QLineSeries *m_izmZbcVect;
    QLineSeries *m_izmZcaVect;

    QChart *m_Zxchart;
    QValueAxis *m_ZxaxisX;
    QValueAxis *m_ZxaxisY;
    QLineSeries *m_ZabVect;
    QLineSeries *m_ZbcVect;
    QLineSeries *m_ZcaVect;
    QScatterSeries   *m_PatternVect;
    QLineSeries *m_PhiM4Vect;
    QLineSeries *m_RM0RM2Vect;
    QLineSeries *m_ErrorPVect;
    QLineSeries *m_ErrorMVect;

    QChart *m_osc_chart;
    QValueAxis *osc_maxisX;
    QValueAxis *osc_maxisY;
    QLineSeries *osc_UaVect;
    QLineSeries *osc_UbVect;
    QLineSeries *osc_UcVect;
    QLineSeries *osc_IaVect;
    QLineSeries *osc_IbVect;
    QLineSeries *osc_IcVect;

    QTimer *m_PingTimer;
    QTimer *m_IZMTimer;
    QTimer *m_LEDSTimer;

    QGraphicsScene *m_scene = nullptr;

    QGraphicsTextItem *m_SceneInfo;

    double m_scene_scale = 1.0;

    quint16 m_LogicCellsCount = 0;
    TLogicCell *m_WorkLogCell = nullptr;
    _flex_logic_config m_saveLCsett;
    QVector <_flex_logic_config> m_MCFlexLogicConfig;
    QVector <quint32> m_MCFlexWMCode;
    QVector <QString> m_input_sign_names;

    QVector <quint16> m_prog_elements;
    QVector <TLogicCell * > m_le_ptrs;

    QString m_conf_file ="ul_settings.ini";

    QMap<QString,quint16> m_LElements;
    QMap<QString,quint16> m_LEInput;

    QTimer *m_DebugLETimer;

    QCheckBox *m_RBACcheckBox[128];

};

const QString RBAC_func_names[128] =
{
    "Главный экран",
    "Журнал",
    "Настройки",
    "Тесты, Сервис",
    "Дата и время",
    "Измерения",
    "Осциллограммы",
    "Оперативный обмен сигналов",
    "Виртуальные кнопки (управление)",
    "Окно аутентификации (RBAC)",
    "Стандарт 61850 (управление)",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Измерения: ДФЗ",
    "Измерения: Векторная диаграмма",
    "Измерения: ВЧ",
    "Измерения: Спектр",
    "Измерения: Манипуляция",
    "Измерения: Защиты",
    "Измерения: ДЗ",
    "Измерения: Контроль цепей напряжения",
    "Измерения: Д.Входы – Д.Выходы",
    "Измерения: GOOSE IN – GOOSE OUT",
    "Измерения: ОМП",
    "Измерения: Регистры",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Просмотрщик осциллограмм",
    "«Уставки»",
    "Общие настройки",
    "Приёмопередатчик",
    "Автоконтроль",
    "Функция ВЧЗ",
    "Токовые и дистанционные защиты (ТДЗ)",
    "УРОВ",
    "Функция ЗНР",
    "Команды автоматики",
    "Сигнализация и выходные реле (МУРС)",
    "Дискретные входы (МВ)",
    "Калибровка измерений ВЧ",
    "Контроль цепей напряжения (КЦН)",
    "Определение места повреждения (ОМП)",
    "Настройки БАПВ",
    "Интерфейс",
    "IEC 61850",
    "Modbus",
    "Виртуальные кнопки",
    "Тип входов модуля управления",
    "Настройки осциллографа",
    "Калибровка цепей переменного тока и напряжения",
    "Изменение значения регистра",
    "Сервисный режим",
    "Настройки->IEC61850->MMS Аутентификация",
    "ДФЗ - Задержка манипуляции",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "«Тесты»",
    "Виртуальный источник",
    "Контрольные точки",
    "Контрольные точки Hex",
    "Голосовая связь",
    "Тест манипуляции и угол блокировки",
    "Заводская информация",
    "Служебная информация",
    "Проиграть промо",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Тест дискретных входов (МВ)",
    "Тест реле (МУРС)",
    "Тест входов управления от терминала РЗ",
    "Тест компараторов",
    "Тест команд автоматики",
    "Тест канала",
    "Тест светодиодов",
    "Тест бузера",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Резерв",
    "Функция 92",
    "Функция 93",
    "Функция 94",
    "Функция 95",
    "Функция 96",

    "Функция 97",
    "Функция 98",
    "Функция 99",
    "Функция 100",
    "Функция 101",
    "Функция 102",
    "Функция 103",
    "Функция 104",
    "Функция 105",
    "Функция 106",
    "Функция 107",
    "Функция 108",
    "Функция 109",
    "Функция 110",
    "Функция 111",
    "Функция 112",

    "Функция 113",
    "Функция 114",
    "Функция 115",
    "Функция 116",
    "Функция 117",
    "Функция 118",
    "Функция 119",
    "Функция 120",
    "Функция 121",
    "Функция 122",
    "Функция 123",
    "Функция 124",
    "Функция 125",
    "Функция 126",
    "Функция 127"
};


#endif // MAINWINDOW_H
