#include "tflexconnection.h"
#include "qmenu.h"
#include <QtGlobal>
#include <QCursor>
#include <QGraphicsDropShadowEffect>

#include "tflexport.h"
#include "tlogiccell.h"

TFlexConnection::TFlexConnection(QObject *parent, QPointF start,QPointF finish, QObject *pStartObj, QObject *pFinishObj)
{
    m_parent = parent;
    m_pStartObj  = pStartObj;
    m_pFinishObj = pFinishObj;

    m_xmin = qMin(start.x(),finish.x());
    m_xmax = qMax(start.x(),finish.x());

    m_ymin = qMin(start.y(),finish.y());
    m_ymax = qMax(start.y(),finish.y());

    m_center = (finish.x() - start.x()) /2.0 + start.x();
    m_start = start;
    m_finish = finish;

    m_menu = new QMenu();
    m_deleteWire = new QAction(tr("Удалить"), this);

    connect(this,SIGNAL(signalNeedSceneUpdate()),parent,SLOT(slotSceneUpdate()));
    connect(m_deleteWire, SIGNAL(triggered()), this, SLOT(slotRemove())); // Обработчик удаления

    m_menu->addAction(m_deleteWire);

    //this->setFlag(QGraphicsItem::ItemIsMovable,true);
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);

    //QGraphicsDropShadowEffect *pShadow = new QGraphicsDropShadowEffect;
    //this->setGraphicsEffect(pShadow);

}

void TFlexConnection::SetStartPoint(QPointF start)
{
    m_start = start;
    m_center = (m_finish.x() - start.x()) /2.0 + start.x();

    m_xmin = qMin(start.x(),m_finish.x());
    m_xmax = qMax(start.x(),m_finish.x());

    m_ymin = qMin(start.y(),m_finish.y());
    m_ymax = qMax(start.y(),m_finish.y());

}
void TFlexConnection::SetFinishPoint(QPointF finish)
{
    m_finish = finish;
    m_center = (finish.x() - m_start.x()) /2.0 + m_start.x();

    m_xmin = qMin(m_start.x(),finish.x());
    m_xmax = qMax(m_start.x(),finish.x());

    m_ymin = qMin(m_start.y(),finish.y());
    m_ymax = qMax(m_start.y(),finish.y());

    //QRectF (m_xmin,m_ymin,m_xmax,m_ymax);
}

void TFlexConnection::SetCenterPoint(float center)
{
    m_center = center;
}

QPointF TFlexConnection::GetStartPoint(void)
{
    return m_start;
}

QPointF TFlexConnection::GetFinishPoint(void)
{
    return m_finish;
}

float TFlexConnection::GetCenterPoint()
{
    return m_center;
}

QObject* TFlexConnection::GetStartObj(void)
{
    return m_pStartObj;
}

QObject* TFlexConnection::GetFinishObj(void)
{
    return m_pFinishObj;
}

void TFlexConnection::SetNumInpLC(quint8 inp)
{
    m_num_inp_lc = inp;
}

quint8 TFlexConnection::GetNumInpLC(void)
{
    return m_num_inp_lc;
}

void TFlexConnection::RemoveLCEvent(void)
{
    TLogicCell *pCell = dynamic_cast <TLogicCell *>( m_pStartObj);
    TFlexPort *pPort  = dynamic_cast <TFlexPort *>(  m_pStartObj);

    if(pCell)
    {
        pCell->ClearWirePtr(this);
    }

    if(pPort)
    {
        pPort->ClearWirePtr(this);
    }

    pCell = dynamic_cast <TLogicCell *>( m_pFinishObj);
    pPort  = dynamic_cast <TFlexPort *>( m_pFinishObj);

    if(pCell)
    {
        pCell->ClearWirePtr(this);
    }

    if(pPort)
    {
        pPort->ClearWirePtr(this);
    }

    delete this;
}


QRectF TFlexConnection::boundingRect() const
{
    return QRectF (m_xmin,m_ymin,m_xmax - m_xmin,m_ymax - m_ymin);
}

void TFlexConnection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->setRenderHint(QPainter::Antialiasing,true);

    QPen pen;
    pen.setWidth(5);

    // painter->setPen(Qt::black);
    // painter->drawRect(this->boundingRect());

   // painter->drawPath();
    pen.setColor(Qt::red);

    this->setFlag(QGraphicsItem::ItemIsMovable,this->isSelected());

    //признак фокуса элемента
    if(this->isSelected())
    {
        pen.setColor(Qt::yellow);
    }

    painter->setPen(pen);
    painter->drawLine(m_start.x(),m_start.y(),m_center,m_start.y());
    painter->drawLine(m_center,m_start.y(),m_center,m_finish.y());
    painter->drawLine(m_center,m_finish.y(),m_finish.x(),m_finish.y());

   // painter->drawLine(m_start,m_finish);
}

void TFlexConnection::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qreal mouse_x = event->pos().x();

    if( (mouse_x < (m_center+10)) && (mouse_x > (m_center-10)) )
    {
        this->setCursor(Qt::SizeHorCursor);

        m_center+= event->pos().x() - event->lastPos().x();

    } else
    {
        this->setCursor(Qt::ArrowCursor);
    }

    QGraphicsItem::mouseMoveEvent(event);

    emit signalNeedSceneUpdate();
}

void TFlexConnection::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
    }

    if(event->button() == Qt::RightButton)
    {
        m_menu->popup(event->screenPos());
    }
}

void TFlexConnection::slotRemove(void)
{
    RemoveLCEvent();
}
