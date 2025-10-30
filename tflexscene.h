#ifndef TFLEXSCENE_H
#define TFLEXSCENE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>

class TFlexScene : public QGraphicsView
{
    Q_OBJECT
public:

    TFlexScene();
    ~TFlexScene();


public slots:
    void mousePressEvent(QMouseEvent * event);

private:
    QGraphicsScene *m_flex_scene = nullptr;

};

#endif // TFLEXSCENE_H
