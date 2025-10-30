#ifndef VADC_CLASS_H
#define VADC_CLASS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QGraphicsItem>

#include "ExchangeThread.h"
#include "qendian.h"

enum ttype_vadcout
{
    _VADC_OUT_VOLTAGE,
    _VADC_OUT_CURRENT
};

enum tconvert_items
{
    _LOW,
    _HIGH,
    _LOW2,
    _HIGH2
};

// union tbits8
// {
//     unsigned char byte_data;
//     struct {
//         unsigned bit0:1;
//         unsigned bit1:1;
//         unsigned bit2:1;
//         unsigned bit3:1;
//         unsigned bit4:1;
//         unsigned bit5:1;
//         unsigned bit6:1;
//         unsigned bit7:1;
//     } single_bits;
// };

union tconvert_short
{
    quint16 shortval;
    quint8  charval[2];
};

union tconvert_int
{
    unsigned int int_val;
    unsigned short short_val[2];
    unsigned char  char_val[4];
};

struct _vadc_params
{
    bool en_VADC = false;
    bool syncUa  = false;

    float Ua = 0.1f;
    qint16 degUa = 0;
    float Ub = 0.0f;
    qint16 degUb = 0;
    float Uc = 0.0f;
    qint16 degUc = 0;
    float U3U0 = 0.0f;
    qint16 deg3U0 = 0;

    float Ia = 0.0f;
    qint16 degIa = 0;
    float Ib = 0.0f;
    qint16 degIb = 0;
    float Ic = 0.0f;
    qint16 degIc = 0;
};

enum _mdfz_type_izm
{
    _MDFZ_IZM_U,
    _MDFZ_IZM_I,
    _MDFZ_IZM_SIM_FILTERS,
    _MDFZ_IZM_ZX,
    _MDFZ_IZM_ADC,
    _MDFZ_IZM_COM,
    _MDFZ_IZM_TYPE_COUNT
};


class VADC_Class : public QObject
{
    Q_OBJECT
    //Q_INTERFACES(QGraphicsItem)

public:
     explicit VADC_Class(QObject *parent = nullptr);
    ~VADC_Class(){;;};

    void VADC_SetU( struct _vadc_params *VADC_Params);
    void VADC_SetI( struct _vadc_params *VADC_Params);
    void VADC_GetU( void);
    void VADC_GetI( void);
    void VADC_ConfigU1(float Ux, quint16 deg, struct _vadc_params *VADC_Params);
    void VADC_ConfigU2(float Ux, quint16 deg,struct _vadc_params *VADC_Params);
    void VADC_Config3U0(float Ux,quint16 deg, struct _vadc_params *VADC_Params);
    void VADC_ConfigI1(float Ix, quint16 deg, struct _vadc_params *VADC_Params);
    void VADC_ConfigI2(float Ix, quint16 deg,struct _vadc_params *VADC_Params);
    void VADC_Config3I0(float Ix,quint16 deg, struct _vadc_params *VADC_Params);
    void VADC_GetIzm(quint8 type_izm);
    void VADC_GetMDFZState(void);
    void VADC_GetHWSettings(quint16 start_sett, quint16 count_sett);
    void VADC_SetHWSettings(quint16 start_sett, quint16 count_sett,QByteArray sett_buff);
    void VADC_GetTime(void);
    void VADC_GetDate(void);
    void VADC_SetMode(quint8 mode);
    void VADC_ClrLOG(void);
    void VADC_GetLOG(quint16 num_log);
    void VADC_GetFW(void);
    void VADC_GetLEDS(void);
    void VADC_GetLogCount(void);
    void VADC_SendReset(void);
    void VADC_SendGetOSC(quint8 num_osc, unsigned short num_sample);
    void VADC_CLRData(void);
    void VADC_GetMUHelth(void);
    void VADC_SendGetModulesState(void);
    void VADC_SendGetString(void);
    void VADC_SendGetGOOSEState(void);
    void VADC_SendGetGOOSEOutNames(quint8 cb_name_num);
    void VADC_SendGetHF(quint8 type_hf);
    void VADC_GetCompState();
    void VADC_SendVButton(quint8 type,quint8 butt,quint8 state);
    void VADC_GetLEState(void);

signals:


private:

    QObject *m_parent;
    ExchangeThread *mExchangeThread;
    void VADC_SendData(QByteArray buff);



    //void (*m_ProcessAnswer[0x23])(QByteArray PCData);
   // void EmptyAnswer(QByteArray PCData);

};

#endif // VADC_CLASS_H
