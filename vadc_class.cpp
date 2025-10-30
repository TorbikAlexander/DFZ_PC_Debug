#include "vadc_class.h"
#include <QDebug>
#include <QObject>
#include <QGraphicsItem>

#include "ExchangeThread.h"
#include "ExchangeParams.h"
#include "crc16.h"


VADC_Class::VADC_Class(QObject *parent)
{
    m_parent = parent;

    QThread *thread= new QThread;
    mExchangeThread = new ExchangeThread(parent);
    mExchangeThread->moveToThread(thread);
    thread->start();
}


void VADC_Class::VADC_SendData(QByteArray buff)
{

    tconvert_short buff_crc;
    buff_crc.shortval = CRC16(&buff,buff.size());
    buff.append(buff_crc.charval[_LOW]);
    buff.append(buff_crc.charval[_HIGH]);

    mExchangeThread->SendMessage(buff);

}

void VADC_Class::VADC_CLRData(void)
{
    mExchangeThread->SetCLRQueue();
}

static inline void PutShortToBuff(QByteArray *data_buff, quint16 sdata)
{
    union tconvert_short data;
    data.shortval = sdata;
    data_buff->push_back(data.charval[_HIGH]);
    data_buff->push_back(data.charval[_LOW]);
}

static inline void PutFloatToBuff(QByteArray *data_buff, float sdata)
{
    union tconvert_short data;
    data.shortval = static_cast<quint16>(sdata*100.0f);
    data_buff->push_back(data.charval[_HIGH]);
    data_buff->push_back(data.charval[_LOW]);
}

void VADC_Class::VADC_SetU( struct _vadc_params *VADC_Params)
{
    QByteArray data_buff;
    qint16 deg;
    quint8 vadc_en_param = 0;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);//
    data_buff.push_back((quint8)_LP_RD_CONFIG_VIRT_ADC);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x18);

    if(VADC_Params->en_VADC) vadc_en_param = 1;
    if(VADC_Params->syncUa) vadc_en_param |= 0x10;

    data_buff.push_back(vadc_en_param);
    data_buff.push_back(_VADC_OUT_VOLTAGE);

    PutFloatToBuff(&data_buff,VADC_Params->Ua);
    deg = VADC_Params->degUa;
    if(deg<0) deg = 360 + deg;
    PutShortToBuff(&data_buff, deg);

    deg = VADC_Params->degUb;
    if(deg<0) deg = 360 + deg;
    PutFloatToBuff(&data_buff,VADC_Params->Ub);
    PutShortToBuff(&data_buff,deg);

    deg = VADC_Params->degUc;
    if(deg<0) deg = 360 + deg;
    PutFloatToBuff(&data_buff,VADC_Params->Uc);
    PutShortToBuff(&data_buff,deg);

    deg = VADC_Params->deg3U0;
    if(deg<0) deg = 360 + deg;
    PutFloatToBuff(&data_buff,VADC_Params->U3U0);
    PutShortToBuff(&data_buff,deg);

    //qDebug() << data_buff;

    VADC_SendData(data_buff);

}

void VADC_Class::VADC_SetI( struct _vadc_params *VADC_Params)
{
    QByteArray data_buff;
    qint16 deg;
    quint8 vadc_en_param = 0;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_CONFIG_VIRT_ADC);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x18);

    if(VADC_Params->en_VADC) vadc_en_param = 1;
    if(VADC_Params->syncUa) vadc_en_param |= 0x10;

    data_buff.push_back(vadc_en_param);
    data_buff.push_back(_VADC_OUT_CURRENT);

    PutFloatToBuff(&data_buff,VADC_Params->Ia);
    deg = VADC_Params->degIa;
    if(deg<0) deg = 360 + deg;
    PutShortToBuff(&data_buff,deg);

    PutFloatToBuff(&data_buff,VADC_Params->Ib);
    deg = VADC_Params->degIb;
    if(deg<0) deg = 360 + deg;
    PutShortToBuff(&data_buff,deg);

    PutFloatToBuff(&data_buff,VADC_Params->Ic);
    deg = VADC_Params->degIc;
    if(deg<0) deg = 360 + deg;
    PutShortToBuff(&data_buff,deg);

    PutFloatToBuff(&data_buff,0.0f);
    PutShortToBuff(&data_buff,0);

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetU(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_VADC_PARAM);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);
    data_buff.push_back((quint8)_VADC_OUT_VOLTAGE);
    data_buff.push_back((quint8)0x00);

    VADC_SendData(data_buff);

}
void VADC_Class::VADC_GetI(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_VADC_PARAM);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);
    data_buff.push_back((quint8)_VADC_OUT_CURRENT);
    data_buff.push_back((quint8)0x00);

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_ConfigU1(float Ux, quint16 deg, _vadc_params *VADC_Params)
{
    quint16 var_deg;

    VADC_Params->Ua = Ux;
    VADC_Params->degUa = deg;

    VADC_Params->Ub = Ux;
    var_deg = deg + 240;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degUb = var_deg;

    VADC_Params->Uc = Ux;
    var_deg = deg + 120;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degUc = var_deg;
}

void VADC_Class::VADC_ConfigU2(float Ux, quint16 deg, _vadc_params *VADC_Params)
{
    quint16 var_deg;

    VADC_Params->Ua = Ux;
    VADC_Params->degUa = deg;

    VADC_Params->Ub = Ux;
    var_deg = deg + 120;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degUb = var_deg;

    VADC_Params->Uc = Ux;
    var_deg = deg + 240;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degUc = var_deg;
}

void VADC_Class::VADC_Config3U0(float Ux, quint16 deg, _vadc_params *VADC_Params)
{
    VADC_Params->U3U0 = Ux;
    VADC_Params->deg3U0 = deg;
}

void VADC_Class::VADC_ConfigI1(float Ix, quint16 deg, _vadc_params *VADC_Params)
{
    quint16 var_deg;

    VADC_Params->Ia = Ix;
    VADC_Params->degIa = deg;

    VADC_Params->Ib = Ix;
    var_deg = deg + 240;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degIb = var_deg;

    VADC_Params->Ic = Ix;
    var_deg = deg + 120;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degIc = var_deg;
}

void VADC_Class::VADC_ConfigI2(float Ix, quint16 deg, _vadc_params *VADC_Params)
{
    quint16 var_deg;

    VADC_Params->Ia = Ix;
    VADC_Params->degIa = 0;

    VADC_Params->Ib = Ix;
    var_deg = deg + 120;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degIb = var_deg;

    VADC_Params->Ic = Ix;
    var_deg = deg + 240;
    if(var_deg>359) var_deg -= 360;
    VADC_Params->degIc = var_deg;
}

void VADC_Class::VADC_Config3I0(float Ix, quint16 deg, _vadc_params *VADC_Params)
{
    VADC_Params->Ia = Ix;
    VADC_Params->degIa = deg;
    VADC_Params->Ib = 0;
    VADC_Params->degIb =0;
    VADC_Params->Ic = 0;
    VADC_Params->degIc = 240;
}

void VADC_Class::VADC_GetIzm(quint8 type_izm)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_MDFZ_IZM);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)type_izm);//--тип измерений
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetMDFZState(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_V4Z_STATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetHWSettings(quint16 start_sett, quint16 count_sett)
{
    QByteArray data_buff;

    tconvert_short start = {.shortval = start_sett};
    tconvert_short count = {.shortval = count_sett};

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_HW_SETTINGS);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x0A);//--
    data_buff.push_back((quint8)start.charval[_HIGH]);//старт
    data_buff.push_back((quint8)start.charval[_LOW]);//
    data_buff.push_back((quint8)count.charval[_HIGH]);//количество
    data_buff.push_back((quint8)count.charval[_LOW]);//

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SetHWSettings(quint16 start_sett, quint16 count_sett,QByteArray sett_buff)
{
    QByteArray data_buff;

    tconvert_short start = {.shortval = start_sett};
    tconvert_short count = {.shortval = count_sett};

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_CH_SETTINGS);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)(10+count_sett*2));//--
    data_buff.push_back((quint8)start.charval[_HIGH]);//старт
    data_buff.push_back((quint8)start.charval[_LOW]);//
    data_buff.push_back((quint8)count.charval[_HIGH]);//количество
    data_buff.push_back((quint8)count.charval[_LOW]);//

    for(quint16 i = 0; i< count_sett*2;i++ )
    {
        data_buff.push_back(sett_buff.at(i));
    }

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetTime(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_TIME);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetDate(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_DATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SetMode(quint8 mode)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_SET_MODE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x0A);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x26);//Пароль
    data_buff.push_back((quint8)0x4E);//Пароль
    data_buff.push_back((quint8)mode);

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_ClrLOG(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_CLR_LOG);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x01);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetLOG(quint16 num_log)
{
    QByteArray data_buff;
    tconvert_short num = {.shortval = num_log};

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_LOG);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)num.charval[_HIGH]);//номер события
    data_buff.push_back((quint8)num.charval[_LOW]);//номер события

    VADC_SendData(data_buff);

}

void VADC_Class::VADC_GetFW(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_MODULES_FW);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetLEDS(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_LEDS);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetLogCount()
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GETCOUNT_LOG);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendReset()
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_RESET_INF);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetOSC(quint8 num_osc, unsigned short num_sample)
{
    QByteArray data_buff;
    tconvert_short nsample = {.shortval=num_sample};

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GETOSC);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x09);//--
    data_buff.push_back((quint8)num_osc);//номер осциллограммы
    data_buff.push_back((quint8)nsample.charval[_HIGH]);//резерв
    data_buff.push_back((quint8)nsample.charval[_LOW]);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetMUHelth(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_MU_HELTH);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x00));//резерв
    data_buff.push_back((quint8)(0x00));//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetModulesState(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_MODULE_STATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x00));//резерв
    data_buff.push_back((quint8)(0x00));//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetString(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_STRING);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x01));//string id
    data_buff.push_back((quint8)(0x00));//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetGOOSEState(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_GOOSE_STATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x01));//GOOSE IN = 0, GOOSE OUT = 1
    data_buff.push_back((quint8)(0x00));//

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetGOOSEOutNames(quint8 cb_name_num)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_STRING);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x04));//string id
    data_buff.push_back((quint8)(cb_name_num));//

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendGetHF(quint8 type_hf)
{
    QByteArray data_buff;

    data_buff.clear();
    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_SEND_HF_DATA);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)(0x00));//
    data_buff.push_back((quint8)(type_hf));//

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetCompState(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GETCOMPARATORS_STATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);//--
    data_buff.push_back((quint8)0x00);//резерв
    data_buff.push_back((quint8)0x00);//резерв

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_SendVButton(quint8 type, quint8 butt, quint8 state)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_VBUTTON);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x09);//
    data_buff.push_back((quint8)type);//type
    data_buff.push_back((quint8)butt);//butt
    data_buff.push_back((quint8)state);//state

    VADC_SendData(data_buff);
}

void VADC_Class::VADC_GetLEState(void)
{
    QByteArray data_buff;

    data_buff.clear();

    data_buff.push_back((quint8)VADC_START_BYTE);
    data_buff.push_back((quint8)_LP_RD_GET_FESTATE);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x08);
    data_buff.push_back((quint8)0x00);
    data_buff.push_back((quint8)0x00);

    VADC_SendData(data_buff);
}
