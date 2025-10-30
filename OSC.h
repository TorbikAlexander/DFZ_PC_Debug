#ifndef OSC_H
#define OSC_H

#include <QMainWindow>

#define MDFZ_OSC_MAX_SAMPLES	22527
#define MDFZ_OSC_MAX_MESS_COUNT 2816
#define MDFZ_OSC_MAX_SAMPLE_NUMB  22520
#define MDFZ_OSC_COUNT          10
struct _osc_buff
{
    QByteArray data;
    bool    isLoad = false;
};


#pragma pack(1)

struct _part_osc_sample_type
{
    unsigned char mess_header[4];

    unsigned char num_osc;

    unsigned char year_osc;
    unsigned char month_osc;
    unsigned char date_osc;
    unsigned char hour_osc;
    unsigned char min_osc;
    unsigned char sec_osc;
    quint16_be msec_osc;

    quint16_be start_num_sample_osc;

    quint16_be data1_osc[16];
    quint16_be data2_osc[16];
    quint16_be data3_osc[16];
    quint16_be data4_osc[16];
    quint16_be data5_osc[16];
    quint16_be data6_osc[16];
    quint16_be data7_osc[16];
    quint16_be data8_osc[16];

    unsigned short crc16_osc;

};

struct _OMP_data
{
    quint16_be type_kz;
    quint16_be length_T1;
    quint16_be length_T2;
    quint16_be val_3U0;
    quint16_be val_3I0;
    quint16_be val_U2;
    quint16_be val_I2;
    quint16_be kz_2side_OMP;

    unsigned num_part_T1 : 4;
    unsigned num_part_T2 : 4;
    unsigned num_part_2side : 4;
    unsigned reserv2 : 4;
};

union _osc_compressed_data
{
    unsigned int full_data;
    struct
    {
        unsigned byte   :8;
        unsigned length : 24;
    } part_data;
};

#endif // OSC_H
