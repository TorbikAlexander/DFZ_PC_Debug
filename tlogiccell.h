#ifndef TLOGICCELL_H
#define TLOGICCELL_H

#include "qgraphicsitem.h"
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QVector>
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QCursor>

#include "tflexconnection.h"

#define MAX_IN_SIGNAL_COUNT		8
#define MAX_FLEX_LOGIC_COUNT	255

//#pragma pack(push, 1)

union _flex_state_union
{
    unsigned char state[2];
    struct
    {
        unsigned in1 : 1;
        unsigned in2 : 1;
        unsigned in3 : 1;
        unsigned in4 : 1;
        unsigned in5 : 1;
        unsigned in6 : 1;
        unsigned in7 : 1;
        unsigned in8 : 1;
        unsigned out : 1;

        unsigned res : 7;
    } part_state;
};

struct _flex_logic_config
{
    unsigned short num_element;
    unsigned char logic_func_type;//привязка логической функции
    unsigned char count_in;
    unsigned char inv_inp_signals; //инверсия входного сигнала
    bool inv_output_signal;
    unsigned short num_global_signals[MAX_IN_SIGNAL_COUNT]; //привязка сигналов
    unsigned int  param_element;
    unsigned char reserv[6];
    char comment_str[16];
} __attribute__((packed, aligned(1)));

//#pragma pack(pop)

class TLogicCell: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit TLogicCell(QObject *parent, float x,float y);
    ~TLogicCell(){};

    void SetNumberLogicCell(quint16 number);
    quint16 GetNumberLogicCell(void);
    void SetTypeLogicCell(quint8 type);
    quint8 GetTypeLogicCell(void);
    void SetLogicCellWirePtr(quint8 wtype, TFlexConnection *pWire);
    void SetSettingsFlexLC(_flex_logic_config sett);
    struct _flex_logic_config GetSettingsFlexLC(void);
    void SetInSRCCell(quint8 num_in,quint16 src);
    quint16 GetOUTSRCCell(void);
    QVector <quint32> GetWMInstrBuff(void);
    void SetWMInstrBuff(QVector <quint32> instr_buff);
    void SetSizeWMInstrBuff(quint8 buff_size);
    void ClearWirePtr(QObject *pWire);

    TFlexConnection* GetInWirePtr(quint8 wnumber);
    TFlexConnection* GetOutWirePtr(quint8 wnumber);

    void SetLEState(quint8 state_h,quint8 state_l);

signals:
    void signalSendCoord(QObject *pCell, bool finish_coord, QPointF coord,quint16 num_input);
    void signalNeedSceneUpdate(void);
    void signalEditFlexLC(QObject *pCell,_flex_logic_config sett);

private slots:
    void slotEdit(void);
    void slotRemove(void);
    void slotWireConnect(void);
    void slotUpdateSceneTimerProcedure();
    void slotSetDebugMode(bool);

public slots:
    void slotProcessWireConnection(bool);

private:

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    //virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);

    QObject *m_parent = nullptr;
    QMenu *m_menu; // контекстное меню
    QAction * commentDevice;
    QAction * editDevice;
    QAction * deleteDevice;

    QMenu *m_add_menu;
    QAction * m_conn_wire[8];

    TFlexConnection *m_in_wire[8] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    TFlexConnection *m_out_wire[8]= {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};

    QVector <quint32> m_wm_instructions;

    QTimer *m_UpdateSceneTimer;
    QPointF mouse_m_last_position;

    bool m_wire_connect_enable = false;

    bool m_debug_mode = false;

    union _flex_state_union m_flex_state_union = {{0x00}};

    struct _flex_logic_config m_LogicCellSett; //размещена в конце из-за __attribute__((packed, aligned(1))); Временный костыль!
};

enum _LC_TYPE_ENUM
{
    _LOGIC_WIRE,
    _LOGIC_OR,
    _LOGIC_AND,
    _LOGIC_XOR,

    _LOGIC_2OR4_AND,
    _LOGIC_2AND4_OR,
    _LOGIC_4OR_4AND_AND,
    _LOGIC_4OR_4AND_OR,

    _LOGIC_RS_LATCH,
    _LOGIC_SR_LATCH,
    _LOGIC_2OR_RS_LATCH,
    _LOGIC_2AND_RS_LATCH,
    _LOGIC_4OR_4AND_RS_LATCH,
    _LOGIC_4AND_4OR_RS_LATCH,

    _LOGIC_T_LATCH,
    _LOGIC_OR_T_LATCH,
    _LOGIC_AND_T_LATCH,

    _LOGIC_D_LATCH,

    _LOGIC_DELAY_TIMER,
    _LOGIC_PROLONG_TIMER,
    _LOGIC_IMP_TIMER,
    _LOGIC_COUNTER_TIMER,

    _LOGIC_FLEX_VM,

    _LOGIC_OUTPUT_LOG,
    _LOGIC_OUTPUT_LED,
    _LOGIC_OUTPUT_SMART_LED,
    _LOGIC_OUTPUT_RELE,
    _LOGIC_OUTPUT_GOOSE,
    _LOGIC_OUTPUT_MZ_REGS,
    _LOGIC_OUTPUT_WR_OSC,
    _LOGIC_OUTPUT_PUSK_PRD,
    _LOGIC_OUTPUT_EXT_RESET,
    _LOGIC_OUTPUT_SET_GROUP_SETT,
    _LOGIC_OUTPUT_AVAR_PREDUPR,

    _LOGIC_COUNT
};

const QString logic_names[_LOGIC_COUNT] =
{
    "WIRE",
    "OR",
    "AND",
    "XOR",

    "2OR4_AND",
    "2AND4_OR",
    "4OR_4AND_AND",
    "4OR_4AND_OR",

    "RS_LATCH",
    "SR_LATCH",
    "2OR_RS_LATCH",
    "2AND_RS_LATCH",
    "4OR_4AND_RS_LATCH",
    "4AND_4OR_RS_LATCH",

    "T_LATCH",
    "OR_T_LATCH",
    "AND_T_LATCH",

    "D_LATCH",

    "DELAY",
    "PROLONG",
    "IMPULSE",
    "COUNTER",

    "FLEX_VM",

    "LOG",
    "LED",
    "SLED",
    "RELE",
    "GOOSE",
    "MZ_REGS",
    "WR OSC",
    "PUSK PRD",
    "EXT RESET",
    "SETT_GROUP",
    "SIGN_A_P"
};

//количество входов, используемых логическим элементом
const quint8 lc_in_count[_LOGIC_COUNT] = {1, 2,2,2, 8,8,8,8, 2,2, 8,8,8,8, 1, 8,8, 2, 2,2,2,2, 8, 1,2,2,1,1,1,1,1,1,1,1 };

union _prog_instruction_flex
{
    unsigned int int_data;
    unsigned short short_data[2];
    struct
    {
        unsigned instr:     5; //инструкция
        unsigned op1:		6; //операнд 1
        unsigned op2:		6; //операнд 2
        unsigned op3:		6; //операнд 3
        unsigned op4:		6; //операнд 4
        unsigned res1:      3; //резерв
    } parts;
};

union _prog_param_flex
{
    unsigned int int_data;
    struct
    {
        unsigned instr_count:	8; //инструкция
        unsigned instr_offset:	24; //операнд 1
    } parts;
};

#endif // TLOGICCELL_H
