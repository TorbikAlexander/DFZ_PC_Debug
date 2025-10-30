#include "tflexport.h"
#include <QMenu>
#include "qgraphicssceneevent.h"
#include "qpainter.h"
#include "qtimer.h"

TFlexPort::TFlexPort(QObject *parent, quint32 sign_type, QString sign_name,float x, float y):
QObject(parent), QGraphicsItem()
{
    m_parent = parent;
    m_signaltype = sign_type;
    m_signaltext = sign_name;

    this->setPos(x,y);

    m_menu = new QMenu();
    deleteDevice = new QAction(tr("Удалить"), this);

    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemove())); // Обработчик удаления

    m_menu->addAction(deleteDevice);

    connect(this,SIGNAL(signalSendCoord(QObject*,bool,QPointF,quint16)),parent,SLOT(slotSetLineCoord(QObject*,bool,QPointF,quint16)));
    connect(this,SIGNAL(signalNeedSceneUpdate()),parent,SLOT(slotSceneUpdate()));

    m_UpdateSceneTimer = new QTimer(this);
    connect(m_UpdateSceneTimer,SIGNAL(timeout()),this,SLOT(slotUpdateSceneTimerProcedure()));
    m_UpdateSceneTimer->stop();

};

TFlexPort::~TFlexPort()
{
    // delete m_menu;
    // delete editDevice;
    // delete deleteDevice;
    // delete m_signalsbox;
}


QRectF TFlexPort::boundingRect() const
{
    return QRectF (-400,-150,400,104);
}

void TFlexPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QString image = ":/res/FlexLogic/Pictures/PORT_IN.png";

    painter->setPen(Qt::black);
    painter->setBrush(Qt::lightGray);
    painter->drawRect(-400,-150,400,104);

    painter->drawImage(QRect(-400, -150,400,100),QImage(image));

    QFont font;
    font.setPixelSize(26);
    painter->setFont(font);
    painter->setPen(Qt::white);
    painter->drawText(-390,-90,m_signaltext);

    if(this->isSelected())
    {
        painter->drawImage(QRect(-190,-150,70,70),QImage(":/res/main/Pictures/rabota1.png"));
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
}

void TFlexPort::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    mouse_m_last_position = event->pos();
    m_UpdateSceneTimer->start(250);

    QGraphicsItem::mouseMoveEvent(event);
}

void TFlexPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QPointF pos2 = (this->pos());

        pos2.setX(pos2.x() );
        pos2.setY(pos2.y()-100);

        emit signalSendCoord(this,_PORT_TYPE_IN,pos2,0);
    }

    if(event->button() == Qt::RightButton)
    {
        m_menu->popup(event->screenPos());
    }

}

void TFlexPort::slotRemove(void)
{
    //здесь нужно корректно обработать удаление, сделано для отладки
    for(quint8 i=0; i<8; i++)
    {
        if(m_out_wire[i]!=nullptr)
        {
            m_out_wire[i]->RemoveLCEvent();
        }
    }

    delete this;
}


QString TFlexPort::GetFuncString(void)
{
    return m_signaltext;
}

quint16 TFlexPort::GetOUTSRCCell(void)
{
    quint16 s_type=0;

    if(m_signaltype==0) s_type = 0;
    if(m_signaltype>0) s_type = m_signaltype+MAX_FLEX_LOGIC_COUNT;

    return s_type;
}

void TFlexPort::SetNumber(quint16 num)
{
    m_number = num;
}

quint16 TFlexPort::GetNumber(void)
{
    return m_number;
}

void TFlexPort::SetLogicCellWirePtr(quint8 wtype, TFlexConnection *pWire)
{

    if(wtype == 0)
    {
        for(quint8 i=0;i<MAX_CON_WIRE_COUNT;i++)
        {
            if(m_out_wire[i] == nullptr) m_out_wire[i] = pWire;
        }
    }

    emit signalNeedSceneUpdate();
}

void TFlexPort::ClearWirePtr(QObject *pWire)
{
    for(quint8 i=0; i<MAX_CON_WIRE_COUNT; i++)
    {
        if(m_out_wire[i]==pWire)
        {
            m_out_wire[i] = nullptr;
        }
    }
}


void TFlexPort::slotUpdateSceneTimerProcedure()
{
    m_UpdateSceneTimer->stop();

    for(quint8 i=0;i<MAX_CON_WIRE_COUNT;i++)
    {
        if(m_out_wire[i]!=nullptr)
        {
            QPointF pos2 = (this->pos());
            pos2.setX(pos2.x());
            pos2.setY(pos2.y()-100);
            m_out_wire[i]->SetStartPoint(pos2);
        }
    }

    emit signalNeedSceneUpdate();

}

// _flex_logic_config  TFlexPort::GetSettingsPortFlex()
// {
//     return m_port_config;
// }
