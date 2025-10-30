#ifndef TFLEXCONNECTION_H
#define TFLEXCONNECTION_H

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



class TFlexConnection: public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:

    explicit TFlexConnection(QObject *parent, QPointF start,QPointF finish, QObject *pStartObj, QObject *pFinishObj);
    ~TFlexConnection(){};

public:

    void SetStartPoint(QPointF start);
    void SetFinishPoint(QPointF finish);
    void SetCenterPoint(float center);

    QPointF GetStartPoint(void);
    QPointF GetFinishPoint(void);
    float   GetCenterPoint(void);

    QObject* GetStartObj(void);
    QObject* GetFinishObj(void);

    void SetNumInpLC(quint8 inp);
    quint8 GetNumInpLC(void);


    void RemoveLCEvent(void);

signals:
    void signalNeedSceneUpdate(void);

private slots:

    void slotRemove(void);

private:

    QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    //virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    QObject *m_parent = nullptr;
    quint16 m_part = 0;
    quint8 m_type = 0;
    quint8 m_num_inp_lc = 0;

    QObject *m_pStartObj = nullptr;
    QObject *m_pFinishObj = nullptr;

    QMenu *m_menu; // контекстное меню
    QAction * m_deleteWire;

    float m_xmin,m_xmax;
    float m_ymin,m_ymax;
    float m_center;
    QPointF m_start,m_finish;
};

#endif // TFLEXCONNECTION_H
