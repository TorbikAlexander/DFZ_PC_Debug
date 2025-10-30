#ifndef TFLEXPORT_H
#define TFLEXPORT_H

#include "qaction.h"
#include "qcombobox.h"
#include "tlogiccell.h"
#include <QGraphicsItem>

enum port_type_enum
{
    _PORT_TYPE_IN,
    _PORT_TYPE_OUT,
    _PORT_TYPE_COUNT
};

#define MAX_CON_WIRE_COUNT  8


class TFlexPort : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    TFlexPort(QObject *parent, quint32 sign_type, QString sign_name,float x, float y);
    ~TFlexPort();

    QString GetFuncString(void);
    quint16 GetOUTSRCCell(void);
    void SetNumber(quint16 num);
    quint16 GetNumber(void);
    void SetLogicCellWirePtr(quint8 wtype, TFlexConnection *pWire);
    void ClearWirePtr(QObject *pWire);
    //_flex_logic_config GetSettingsPortFlex(void);

private:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    QObject *m_parent = nullptr;
    QMenu *m_menu; // контекстное меню
    QAction * editDevice;
    QAction * deleteDevice;

    QComboBox *m_signalsbox;
    quint32 m_signaltype = 0;
    QString m_signaltext = "GND";
    quint16 m_number =0;

    //_flex_logic_config m_port_config;

    TFlexConnection *m_out_wire[MAX_CON_WIRE_COUNT] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};

    QTimer *m_UpdateSceneTimer;
    QPointF mouse_m_last_position;


private slots:
    void slotRemove(void);
    void slotUpdateSceneTimerProcedure();

signals :
    void signalSendCoord(QObject *pPort, bool finish_coord, QPointF coord,quint16 num_input);
    void signalNeedSceneUpdate(void);
};

#endif // TFLEXPORT_H
