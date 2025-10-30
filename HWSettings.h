#ifndef HWSETTINGS_H
#define HWSETTINGS_H


#include <QDir>
#include <QFileDialog>
#include <QListWidget>
#include <QtCore>
#include <QObject>
#include <QTableWidgetItem>

const QStringList Settheaders =
{
    "Общие настройки УПЗА",
    "Центр. частота",
    "Номер аппарата",
    "Сервисные пуски",
    "Компаратор ОСН",
    "Компаратор HIGH",
    "Компаратор LOW",
    "Компаратор АК",
    "Тип фильтра ПРМ",
    "Настройки ПРМ",
    "Настройки ПРД",
    "Настройки МС",
    "Блок. входов МВ",
    "Защита от дреб. МВ",
    "КА-Вход1"

};

#define V4Z_SETT_OFFSET			0x0100
#define V4B_SETT_OFFSET			0x0120
#define OMP_SETT_OFFSET			0x0130
#define COMMON_SETT_OFFSET      0x013F
#define RZ_TZNP_SETT_OFFSET     0x015A
#define IEC61850_SETT_OFFSET    0x0220

//*****Настройки******
enum _settings
{
    //***настройки УПЗ
    _FUNC_ENABLED,
    _FREQ_PRMD,
    _NUM_PRMD, ////количество аппаратов/номер аппарата
    _SETTINGS_COMMON,
    _COMP_OSN, //компараторы
    _COMP_HIGH,
    _COMP_LOW,
    _COMP_DOP,
    _FILTER_TYPE,
    _COMMON_PRM_SETTINGS,
    _CONTR_PRD,

    _SETTINGS_MC, //настройки сигнализации: защелки

    _MV_BLOCK,//блокированные входы
    _MV_DELAY_PROTECT,	//задержки от дребезга

    _INPUT1_KA,// Привязка вход-команда
    _INPUT2_KA,
    _INPUT3_KA,
    _INPUT4_KA,
    _INPUT5_KA,
    _INPUT6_KA,
    _INPUT7_KA,
    _INPUT8_KA,

    _SETTINGS_COM_KA,//общие настройки команд автоматики
    _CONF_COM1, //привязка команда - реле
    _CONF_COM2, //номер бита+1 = номеру реле
    _CONF_COM3, //старший байт МУРС2
    _CONF_COM4, //младший байт МУРС1
    _CONF_COM5,
    _CONF_COM6,
    _CONF_COM7,
    _CONF_COM8,
    _CONF_COM9,
    _CONF_COM10,
    _CONF_COM11,
    _CONF_COM12,
    _CONF_COM13,
    _CONF_COM14,
    _CONF_COM15,
    _CONF_COM16,

    _IMP_RELE5_M1, //Параметры импульсного/ длительного режима работы реле
    _IMP_RELE6_M1, //реле в МУРС1
    _IMP_RELE7_M1,
    _IMP_RELE8_M1,
    _IMP_RELE1_M2, //реле в МУРС2
    _IMP_RELE2_M2,
    _IMP_RELE3_M2,
    _IMP_RELE4_M2,
    _IMP_RELE5_M2,
    _IMP_RELE6_M2,
    _IMP_RELE7_M2,
    _IMP_RELE8_M2,

    _SETTINGS_ACONTROL, //настройки автоконтроля
    _TIME_ACONTROL_HIGH,
    _TIME_ACONTROL_LOW,
    _TIME_REPEAT_ACONTROL_HIGH,
    _TIME_REPEAT_ACONTROL_LOW,
    _PASSWORD, //пароль
    _SERIAL_NUMBER,//заводской номер аппарата
    _MANUFACTURE_DATE,//Дата производства аппарата месяц/год
    _SETTINGS_MODBUS,//настройки 485 интерфейса
    _IP_COMMON, //настройки IP протокола
    _IP_HIGH,
    _IP_LOW,
    _IP_MASK_HIGH,
    _IP_MASK_LOW,
    _IP_GW_HIGH,
    _IP_GW_LOW,

    _HOST_NAME12, //имя аппарата в локальной сети (Ethernet)
    _HOST_NAME34,
    _HOST_NAME56,
    _HOST_NAME78,

    _SETTINGS_RTC, //настройки часов, синхронизации времени
    _SETTINGS_IP_MAIN_SNTP_H,
    _SETTINGS_IP_MAIN_SNTP_L,
    _SETTINGS_IP_RESERV_SNTP_H,
    _SETTINGS_IP_RESERV_SNTP_L,
    _SNTP_PORT,
    _SNTP_SYNC_PERIOD,

    _LP_SETTINGS, //настройки ЛП: тип мА, язык...
    _PS1_NAME1,   //названия подстанций в канале
    _PS1_NAME2,
    _PS1_NAME3,
    _PS1_NAME4,
    _PS1_NAME5,
    _PS2_NAME1,
    _PS2_NAME2,
    _PS2_NAME3,
    _PS2_NAME4,
    _PS2_NAME5,
    _PS3_NAME1,
    _PS3_NAME2,
    _PS3_NAME3,
    _PS3_NAME4,
    _PS3_NAME5,
    _PS4_NAME1,
    _PS4_NAME2,
    _PS4_NAME3,
    _PS4_NAME4,
    _PS4_NAME5,

    _LCD_ECO_TIME,
    _RESERV2,
    _RESERV3,
    _RESERV4,

    _UPZA_SETTINGS_COUNT,

    //****Настройки защиты
    _V4Z_COMMON = V4Z_SETT_OFFSET,
    _V4Z_EXTERNAL_INPUTS,
    _V4Z_DFZ_SETTINGS,
    _V4Z_K2K1_MANIP,
    _V4Z_COMMON_MANIP,
    _V4Z_PUSK_PRD_I2,
    _V4Z_PUSK_DFZ_I2,
    _V4Z_PUSK_PRD_I1,
    _V4Z_PUSK_DFZ_I1,
    _V4Z_PUSK_PRD_I0,
    _V4Z_PUSK_DFZ_I0,
    _V4Z_DEBLOCK_PUSK_RZ_I0,
    _V4Z_COMP_I_M2,
    _V4Z_COMP_U_M2,
    _V4Z_COMP_I_M0,
    _V4Z_COMP_U_M0,
    _V4Z_DFZ_IZM_BLOCKS,
    _V4Z_TIMES_PUSK_PRD,
    _V4Z_TIMES_PUSK_DFZ,
    _V4Z_PARAM_OSF,
    _V4Z_TIMES_OSF,
    _V4Z_IZM_CONTROL,
    _V4Z_COMP_CONTROL_3U0,
    _V4Z_COMP_CONTROL_3I0,
    _V4Z_MANIP_LEVEL_I1,
    _V4Z_MANIP_NULL_LEVEL,
    _V4Z_COMMOM_UROV,
    _V4Z_COMP_PHASE_I,
    _V4Z_TIMES_ADAPTIVE_DELAY,
    _V4Z_M2_PHI_SETT,
    _V4Z_M0_PHI_SETT,
    _V4Z_TEST_PHI_BLOCK,

    _V4Z_V4B_SETTINGS = V4B_SETT_OFFSET,
    _V4Z_V4B_LEVEL_BLOCK_U2,
    _V4Z_V4B_LEVEL_BLOCK_I2,
    _V4Z_V4B_LEVEL_OTKL_U2,
    _V4Z_V4B_LEVEL_OTKL_I2,
    _V4Z_V4B_LEVEL_OTKL_I2T,
    _V4Z_V4B_LEVEL_PUSK_I2T,
    _V4Z_V4B_COMP_OTKL_I0_KZ_ZONE,
    _V4Z_V4B_COMP_DI_DT,
    _V4Z_V4B_COMP_REVERS_LONG,
    _V4Z_V4B_DELAY_TIME_SWING,
    _V4Z_V4B_LONG_TIME_SWING,
    _V4Z_V4B_DELAY_WORK,
    _V4Z_V4B_LONG_SIGN_OTKL_OPROB_VL,
    _V4Z_V4B_LONG_TIME_SEND_AK_TO,

    _V4Z_OMP_COMMON_SETTINGS = OMP_SETT_OFFSET,
    _V4Z_OMP_X0,
    _V4Z_OMP_X0_DEG,
    _V4Z_OMP_X1,
    _V4Z_OMP_X1_DEG,
    _V4Z_OMP_RE_Z1,
    _V4Z_OMP_IM_Z1,
    _V4Z_OMP_K0_AMPL,
    _V4Z_OMP_K0_DEG,
    _V4Z_OMP_THR_I1_3F,
    _V4Z_OMP_THR_I1_12F,
    _V4Z_OMP_THR_I2_12F,
    _V4Z_OMP_THR_I0_12F,
    _V4Z_OMP_START_DELAY,

    _V4Z_CAPASITOR_XC			= COMMON_SETT_OFFSET,
    _V4Z_PRIMARY_VOLTAGE,
    _V4Z_NOMINAL_CURRENT,
    _V4Z_PRIMARY_CURRENT,
    _V4Z_RELE_Z_AB_VALUE,
    _V4Z_RELE_Z_AB_SHIFT,
    _V4Z_RELE_Z_AB_ELLIPT,
    _V4Z_RELE_Z_AB_MAX_DEG,
    _V4Z_RELE_Z_BC_VALUE,
    _V4Z_RELE_Z_BC_SHIFT,
    _V4Z_RELE_Z_BC_ELLIPT,
    _V4Z_RELE_Z_BC_MAX_DEG,
    _V4Z_RELE_Z_CA_VALUE,
    _V4Z_RELE_Z_CA_SHIFT,
    _V4Z_RELE_Z_CA_ELLIPT,
    _V4Z_RELE_Z_CA_MAX_DEG,
    _V4Z_RELE_Z_CA2_VALUE,
    _V4Z_RELE_Z_CA2_SHIFT,
    _V4Z_RELE_Z_CA2_ELLIPT,
    _V4Z_RELE_Z_CA2_MAX_DEG,
    _V4Z_FPGA_LEDS_TP12,
    _V4Z_TEST_MODES_CONTROL,
    _V4Z_KIN_THR,
    _V4Z_KIN_DELAY,
    _V4Z_FPGA_TP34,
    _V4Z_FPGA_INPUTS12,
    _V4Z_FPGA_INPUTS34,
    //_V4Z_SETTINGS_COUNT,

    _RZ_COMMON_TZNP = RZ_TZNP_SETT_OFFSET,
    _RZ_ACCELERATE_TIME,
    _RZ_3U0_COMPARATOR,
    _RZ_3U0_FAULT_TIMEOUT,
    _RZ_ENABLE_ONM_PHI,
    _RZ_ENABLE_ONM_COMP_U,
    _RZ_ENABLE_ONM_COMP_I,
    _RZ_BLOCK_ONM_PHI,
    _RZ_BLOCK_ONM_COMP_U,
    _RZ_BLOCK_ONM_COMP_I,
    _RZ_LEVEL1_TZNP_COMPARATOR,
    _RZ_LEVEL2_TZNP_COMPARATOR,
    _RZ_LEVEL3_TZNP_COMPARATOR,
    _RZ_LEVEL4_TZNP_COMPARATOR,
    _RZ_MFO_IA_COMPARATOR,
    _RZ_MFO_IC_COMPARATOR,
    _RZ_MFO_DELAY,
    _RZ_LEVEL1_DELAY,
    _RZ_LEVEL2_DELAY,
    _RZ_LEVEL3_DELAY,
    _RZ_LEVEL4_DELAY,
    _RZ_ACCELERATE_DELAY,


    _RZ_DZ_COMMON,
    _RZ_DZ_TIMEOUT_OUT_FAST,
    _RZ_DZ_TIMEOUT_OUT_SLOW,
    _RZ_DI1DT_SENSIT_COMPARATOR,
    _RZ_DI1DT_COARSE_COMPARATOR,
    _RZ_DI2DT_SENSIT_COMPARATOR,
    _RZ_DI2DT_COARSE_COMPARATOR,
    _RZ_DZ_TIME_LEVEL1,
    _RZ_DZ_TIME_LEVEL2_FAST,
    _RZ_DZ_TIME_LEVEL1_SLOW,
    _RZ_DZ_TIME_LEVEL3,
    _RZ_DZ_TIME_ACCELERATE,
    _RZ_DZ_TYPE_RELE_Z,
    _RZ_RELE_Z_1LEV_VALUE,
    _RZ_RELE_Z_1LEV_SHIFT,
    _RZ_RELE_Z_1LEV_ELLIPT,
    _RZ_RELE_Z_1LEV_MAX_DEG,
    _RZ_RELE_Z_2LEV_VALUE,
    _RZ_RELE_Z_2LEV_MAX_DEG,
    _RZ_RELE_Z_2LEV_SHIFT,
    _RZ_RELE_Z_2LEV_INCLINE,
    _RZ_RELE_Z_3LEV_VALUE,
    _RZ_RELE_Z_3LEV_MAX_DEG,
    _RZ_RELE_Z_3LEV_INCLINE,
    _RZ_RELE_Z_3LEV_SHIFT,
    //_RZ_RELE_Z_3LEV_SHIFT_TEST_REG,

    _RZ_SETTINGS_COUNT,

    _IEC_COMMON = IEC61850_SETT_OFFSET,
    _IEC_ETH1_IP_H,
    _IEC_ETH1_IP_L,
    _IEC_ETH1_MASK,
    _IEC_ETH1_GW_H,
    _IEC_ETH1_GW_L,
    _IEC_ETH2_IP_H,
    _IEC_ETH2_IP_L,
    _IEC_ETH2_MASK,
    _IEC_ETH2_GW_H,
    _IEC_ETH2_GW_L,

    _IEC_NTP_IP_MAIN_H,
    _IEC_NTP_IP_MAIN_L,
    _IEC_NTP_IP_RESERV_H,
    _IEC_NTP_IP_RESERV_L,

    _IEC_PTP_DOMEN,
    _IEC_PTP_VLAN_ID,

    _IEC_MMS_PSWD_B8,
    _IEC_MMS_PSWD_B7,
    _IEC_MMS_PSWD_B6,
    _IEC_MMS_PSWD_B5,
    _IEC_MMS_PSWD_B4,
    _IEC_MMS_PSWD_B3,
    _IEC_MMS_PSWD_B2,
    _IEC_MMS_PSWD_B1,
    _IEC_TIMEZONE,

    //----
    //----

    _IEC_SETTINGS_COUNT,

    _POLY_PATTERN1_DATA_START = 0x250,
    _POLY_PATTERN1_DATA_STOP  = 0x279,

    _POLY_PATTERN2_DATA_START = 0x27A,
    _POLY_PATTERN2_DATA_STOP  = 0x2A3,

    _POLY_PATTERN3_DATA_START = 0x2A4,
    _POLY_PATTERN3_DATA_STOP  = 0x2CD,

    _POLY_PATTERN4_DATA_START = 0x2CE,
    _POLY_PATTERN4_DATA_STOP  = 0x2F7,

    _CRC,

    _SETTINGS_COUNT  //количество настроек аппарата, используется для задания размера массива настроек при компиляции. Всегда размещается в конце.
};



#endif // HWSETTINGS_H
