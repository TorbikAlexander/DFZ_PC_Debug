#include "tflexscene.h"

TFlexScene::TFlexScene()
{
    m_flex_scene = new QGraphicsScene();
    this->setSceneRect(50, 50, 350, 350);
    this->setScene(m_flex_scene);
}

void TFlexScene::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "mouse press on scene";
    QPointF pos = event->pos();
    qDebug() << pos;
}
