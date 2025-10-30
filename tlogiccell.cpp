#include "tlogiccell.h"
#include "qpainter.h"
#include "qtimer.h"
#include <QMenu>
#include <QMessageBox>
#include "vadc_class.h"
#include <QGraphicsDropShadowEffect>

TLogicCell::TLogicCell(QObject *parent, float x,float y):
QObject(parent), QGraphicsItem()
{
    m_parent = parent;

    this->setPos(x,y);

    m_menu = new QMenu();
    commentDevice = new QAction(tr("*"), this);
    editDevice = new QAction(tr("Редактировать"), this);
    deleteDevice = new QAction(tr("Удалить"), this);

    connect(editDevice, SIGNAL(triggered()), this, SLOT(slotEdit()));     // Обработчик вызова диалога редактирования
    connect(deleteDevice, SIGNAL(triggered()), this, SLOT(slotRemove())); // Обработчик удаления

    m_menu->addAction(commentDevice);
    m_menu->addAction(editDevice);
    m_menu->addAction(deleteDevice);

    m_add_menu = new QMenu();
    for(quint8 i=0;i<8;i++)
    {
        m_conn_wire[i] = new QAction(tr("INP") + QString::number(i+1), this);
        m_add_menu->addAction(m_conn_wire[i]);
        connect(m_conn_wire[i], SIGNAL(triggered()), this, SLOT(slotWireConnect())); //
    }

    connect(this,SIGNAL(signalSendCoord(QObject*,bool,QPointF,quint16)),parent,SLOT(slotSetLineCoord(QObject*,bool,QPointF,quint16)));
    connect(this,SIGNAL(signalNeedSceneUpdate()),parent,SLOT(slotSceneUpdate()));
    connect(this,SIGNAL(signalEditFlexLC(QObject*,_flex_logic_config)),parent,SLOT(slotEditFlexLC(QObject*,_flex_logic_config)));

    connect(parent,SIGNAL(signalProcessWireConnection(bool)),this,SLOT(slotProcessWireConnection(bool)));

    connect(parent,SIGNAL(signalSetDebugMode(bool)),this,SLOT(slotSetDebugMode(bool)));

    // QGraphicsDropShadowEffect *pShadow = new QGraphicsDropShadowEffect;
    // this->setGraphicsEffect(pShadow);

    m_LogicCellSett.count_in = lc_in_count[m_LogicCellSett.logic_func_type];
    m_LogicCellSett.inv_inp_signals = 0;
    m_LogicCellSett.inv_output_signal = false;
    m_LogicCellSett.param_element = 1;

    for(quint8 i = 0; i< MAX_IN_SIGNAL_COUNT; i++)
    {
        m_LogicCellSett.num_global_signals[i] = 0;
    }

    for(quint8 i = 0; i<  sizeof(m_LogicCellSett.reserv); i++)
    {
        m_LogicCellSett.reserv[i] = 0xE0;
    }

    char comm_str[] = "LogicElement  \0";
    memcpy(m_LogicCellSett.comment_str,comm_str,16);
    commentDevice->setText(m_LogicCellSett.comment_str);

    QFont comm_font;
    comm_font.setBold(true);
    comm_font.setUnderline(true);
    commentDevice->setFont(comm_font);

    m_wm_instructions.clear();
    m_wm_instructions.append(0);

    m_UpdateSceneTimer = new QTimer(this);
    connect(m_UpdateSceneTimer,SIGNAL(timeout()),this,SLOT(slotUpdateSceneTimerProcedure()));
    m_UpdateSceneTimer->stop();
}



void TLogicCell::slotUpdateSceneTimerProcedure()
{
    QPointF wire_pos = (this->pos());
    quint8 count_in = m_LogicCellSett.count_in;
    quint8 dh = this->boundingRect().height() / (1+count_in);

    m_UpdateSceneTimer->stop();

    for(quint8 i=0; i<8; i++)
    {
        if(m_out_wire[i]!=nullptr)
        {
            wire_pos = (this->pos());
            wire_pos.setX(wire_pos.x() + this->boundingRect().width() /2);
            m_out_wire[i]->SetStartPoint(wire_pos);
        }
    }

    for(quint8 i=0; i<count_in; i++)
    {
       if(m_in_wire[i]!=nullptr)
       {
        wire_pos = (this->pos());
        wire_pos.setX(wire_pos.x() - this->boundingRect().width() /2);
        wire_pos.setY( wire_pos.y() - this->boundingRect().height() /2 + dh * (i+1) );
        m_in_wire[i]->SetFinishPoint(wire_pos);
       }
    }

    emit signalNeedSceneUpdate();

}

void TLogicCell::slotSetDebugMode(bool debug_mode)
{
    m_debug_mode = debug_mode;
    this->update();
}

void TLogicCell::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPen pen;

    painter->setRenderHint(QPainter::Antialiasing,true);

    //общий контур
    pen.setWidth(1);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(Qt::lightGray);
    //painter->drawRect(this->boundingRect());

    //контур и заливка элемента внутри
    pen.setWidth(5);
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->setBrush(Qt::white);

    QRectF elemRect = this->boundingRect();
    elemRect.setLeft(elemRect.left()+50);
    elemRect.setRight(elemRect.right()-50);
    painter->drawRect(elemRect);

    //входные пины и инверсия
    quint8 count_in = m_LogicCellSett.count_in;
    quint8 inv_in = m_LogicCellSett.inv_inp_signals;

    const QColor le_state_color[2] = {Qt::blue,Qt::red};

    quint8 dh = this->boundingRect().height() / (1+count_in);

    for(quint8 i = 0; i< count_in; i++)
    {
        painter->setBrush(Qt::white);

        painter->drawLine(this->boundingRect().left(),elemRect.top()+i*dh+dh,elemRect.left(),elemRect.top()+i*dh+dh);

        if(inv_in & (1<<i))
        {
            painter->drawEllipse(elemRect.left(),elemRect.top()+i*dh+dh+17/2 ,-17,-17);
        }

        if(m_debug_mode)
        {
            bool color_inp = (bool)(m_flex_state_union.state[0] & (1<<i));
            painter->setBrush(le_state_color[color_inp]);
            painter->drawEllipse(elemRect.left() - 20,elemRect.top()+i*dh+dh+17/2 - 13 ,-23,-23);
        }
    }

    //выходной пин
    if(m_LogicCellSett.logic_func_type < (_LOGIC_FLEX_VM+1))
    {
        painter->setBrush(Qt::white);
        painter->drawLine(elemRect.right(),(elemRect.top()+elemRect.bottom()) /2 ,this->boundingRect().right(),(elemRect.top()+elemRect.bottom()) /2);

        if(m_LogicCellSett.inv_output_signal)
        {
            painter->drawEllipse(elemRect.right()+8,(elemRect.top()+elemRect.bottom()) /2 + 8 ,-17,-17);
        }

        if(m_debug_mode)
        {
            painter->setBrush(le_state_color[m_flex_state_union.part_state.out]);
            painter->drawEllipse(elemRect.right()+33,(elemRect.top()+elemRect.bottom()) /2 - 5 ,-23,-23);
        }
    }

    //признак фокуса элемента
    if(this->isSelected())
    {
        painter->drawImage(QRect(elemRect.left(),elemRect.top(),70,70),QImage(":/res/main/Pictures/rabota1.png"));
    }

    //признак процесса соединения
    if(m_wire_connect_enable)
    {
        painter->drawImage(QRect(elemRect.right() - 65,elemRect.top()+5,70,70),QImage(":/res/main/Pictures/mouse.png"));
    }

    //картинка выходного сигнала

    QImage img_out;
    switch (m_LogicCellSett.logic_func_type)
    {
    case _LOGIC_FLEX_VM: {img_out = QImage(":/res/FlexLogic/Pictures/cpu.png"); break;};
    case _LOGIC_OUTPUT_LOG: {img_out = QImage(":/res/FlexLogic/Pictures/log.png"); break;};
    //case _LOGIC_OUTPUT_LED: {img_out = QImage(":/res/FlexLogic/Pictures/ktip.png"); break;};
    case _LOGIC_OUTPUT_RELE: {img_out = QImage(":/res/FlexLogic/Pictures/ReleInfoOn.png"); break;};
    case _LOGIC_OUTPUT_GOOSE: {img_out = QImage(":/res/FlexLogic/Pictures/61692177-goose-icon-illustration.jpg"); break;};
    case _LOGIC_OUTPUT_MZ_REGS: {img_out = QImage(":/res/FlexLogic/Pictures/cpu.png"); break;};
    case _LOGIC_OUTPUT_WR_OSC: {img_out = QImage(":/res/FlexLogic/Pictures/cpu.png"); break;};
    case _LOGIC_OUTPUT_PUSK_PRD: {img_out = QImage(":/res/FlexLogic/Pictures/cpu.png"); break;};
    case _LOGIC_OUTPUT_EXT_RESET: {img_out = QImage(":/res/FlexLogic/Pictures/log.png"); break;};
    case _LOGIC_OUTPUT_SET_GROUP_SETT: {img_out = QImage(":/res/FlexLogic/Pictures/log.png"); break;};
    case _LOGIC_OUTPUT_AVAR_PREDUPR: {img_out = QImage(":/res/FlexLogic/Pictures/ReleInfoOn.png"); break;};
    default: break;
    }

    painter->drawImage(QRect(elemRect.left()+30,(elemRect.top()+elemRect.bottom()) /2+5,125,125),img_out);

    //номер элемента на схеме
    QFont font;
    font.setPixelSize(32);
    painter->setFont(font);
    painter->setPen(Qt::blue);

    QString ElemPD;
    ElemPD.clear();

    if(m_LogicCellSett.logic_func_type > _LOGIC_FLEX_VM)
    {
        ElemPD.append('S');
    } else
    {
        ElemPD.append('D');
    }

    painter->drawText(elemRect.right() - 2*font.pixelSize(), elemRect.bottom() - 10,ElemPD + QString::number(m_LogicCellSett.num_element));

    //тип элемента
    QString sfunc_type = logic_names[m_LogicCellSett.logic_func_type];

    if((m_LogicCellSett.logic_func_type == _LOGIC_OR) || (m_LogicCellSett.logic_func_type == _LOGIC_AND))
    {
        sfunc_type += QString::number(m_LogicCellSett.count_in);
    }

    if(m_LogicCellSett.logic_func_type > _LOGIC_FLEX_VM)
    {
        sfunc_type+= " " + QString::number(m_LogicCellSett.param_element & 0xFF);
    }

     painter->drawText(elemRect.left() + 10,(elemRect.top()+elemRect.bottom()) /2 , sfunc_type);

     // Маркировка S R для RS триггера
     if( (m_LogicCellSett.logic_func_type == _LOGIC_RS_LATCH) || (m_LogicCellSett.logic_func_type == _LOGIC_SR_LATCH) || (m_LogicCellSett.logic_func_type == _LOGIC_COUNTER_TIMER))
    {
        painter->drawText(elemRect.left() + 10,elemRect.top()+dh,"S");
        painter->drawText(elemRect.left() + 10,elemRect.top()+dh*2,"R");
    }

     // Маркировка D CLK для D триггера
     if( m_LogicCellSett.logic_func_type == _LOGIC_D_LATCH)
     {
         painter->drawText(elemRect.left() + 10,elemRect.top()+dh,"D");
         painter->drawText(elemRect.left() + 10,elemRect.top()+dh*2,"CLK");
     }

     // Маркировка Sign RES для TIMERS и для SMART светодиода
     if( m_LogicCellSett.logic_func_type == _LOGIC_DELAY_TIMER   ||
         m_LogicCellSett.logic_func_type == _LOGIC_PROLONG_TIMER ||
         m_LogicCellSett.logic_func_type == _LOGIC_IMP_TIMER     ||
         m_LogicCellSett.logic_func_type == _LOGIC_COUNTER_TIMER ||
         m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_SMART_LED
        )
     {
         painter->drawText(elemRect.left() + 10,elemRect.top()+dh,"INP");
         painter->drawText(elemRect.left() + 10,elemRect.top()+dh*2,"RES");

         if(m_LogicCellSett.logic_func_type != _LOGIC_OUTPUT_SMART_LED)
         {
             QString time_str;
             time_str = "t= ";
             time_str += QString::number(m_LogicCellSett.param_element);
             time_str += "ms";
             painter->drawText(elemRect.left() + 10,elemRect.bottom() - 50,time_str);
         }
     }

     // Маркировка 1X  2X для светодиода
    if( m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_LED)
    {
        painter->drawText(elemRect.left() + 10,elemRect.top()+dh,"1X");
        painter->drawText(elemRect.left() + 10,elemRect.top()+dh*2,"2X");
    }

    if(m_LogicCellSett.logic_func_type < (_LOGIC_FLEX_VM+1))
    {
        painter->drawText(elemRect.right() - font.pixelSize(),(elemRect.top()+elemRect.bottom()) /2,"Q");
    }

    // вывод параметров реле
    if( m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_RELE)
    {
        union tconvert_int lc_param = {.int_val = m_LogicCellSett.param_element};

        painter->drawText(elemRect.left() + 10,elemRect.top()+30,"dt on  =" + QString::number(lc_param.char_val[_HIGH]));
        painter->drawText(elemRect.left() + 10,elemRect.top()+60,"dt off =" + QString::number(lc_param.char_val[_LOW2]));
        painter->drawText(elemRect.left() + 10,elemRect.top()+90,"t imp  =" + QString::number(lc_param.char_val[_HIGH2]));
    }

    // вывод параметров светодиода
    if( ( m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_LED) || ( m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_SMART_LED))
    {
        const unsigned char const_lev_color[2] = {0,255};
        bool lev_value;
        union tconvert_int lc_param = {.int_val = m_LogicCellSett.param_element};
        QColor led_color;

        lev_value = (bool)(lc_param.char_val[_HIGH] & 1);
        led_color.setBlue((const_lev_color[lev_value]));

        lev_value = (bool)(lc_param.char_val[_HIGH] & 2);
        led_color.setGreen((const_lev_color[lev_value]));

        lev_value = (bool)(lc_param.char_val[_HIGH] & 4);
        led_color.setRed((const_lev_color[lev_value]));

        painter->setBrush(QBrush(led_color,Qt::SolidPattern));
        painter->drawEllipse(elemRect.left() +120,(elemRect.top()+100) ,70,70);
    }


    Q_UNUSED(option);
    Q_UNUSED(widget);
}

QRectF TLogicCell::boundingRect() const
{
    return QRectF (-150,-150,300,300);
}

void TLogicCell::SetNumberLogicCell(quint16 number)
{
    m_LogicCellSett.num_element = number;
}

quint16 TLogicCell::GetNumberLogicCell(void)
{
    return m_LogicCellSett.num_element;
}


void TLogicCell::SetTypeLogicCell(quint8 type)
{
    m_LogicCellSett.logic_func_type = type;
    m_LogicCellSett.count_in = lc_in_count[m_LogicCellSett.logic_func_type];

    if( (m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_LED) || (m_LogicCellSett.logic_func_type == _LOGIC_OUTPUT_SMART_LED))
    {
        m_LogicCellSett.param_element = 0x00000201;
    }

}

quint8 TLogicCell::GetTypeLogicCell(void)
{
    return m_LogicCellSett.logic_func_type;
}

void TLogicCell::SetInSRCCell(quint8 num_in,quint16 src)
{
    m_LogicCellSett.num_global_signals[num_in] = src;
}

quint16 TLogicCell::GetOUTSRCCell(void)
{
    return  m_LogicCellSett.num_element;
}

 void TLogicCell::SetLogicCellWirePtr(quint8 wtype, TFlexConnection *pWire)
{
    if(wtype==0)
    {
        for(quint8 i=0; i<8; i++)
        {
            if(m_out_wire[i]==nullptr)  m_out_wire[i] = pWire;
        }
    } else
    {
        m_in_wire[wtype-1] = pWire;
    }

    emit signalNeedSceneUpdate();
}

TFlexConnection* TLogicCell::GetInWirePtr(quint8 wnumber)
{
    TFlexConnection* ptr = nullptr;

     if(wnumber<8)
    {
        ptr = m_in_wire[wnumber];
    }

     return ptr;
}

TFlexConnection* TLogicCell::GetOutWirePtr(quint8 wnumber)
{
    TFlexConnection* ptr = nullptr;

    if(wnumber<8)
    {
        ptr = m_out_wire[wnumber];
    }

    return ptr;
}

void TLogicCell::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
     mouse_m_last_position = event->pos();
     m_UpdateSceneTimer->start(250);
     this->setSelected(false);

    QGraphicsItem::mouseMoveEvent(event);
}

void TLogicCell::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if((event->button() == Qt::LeftButton) && (m_wire_connect_enable))
    {
        QPointF mouse_pos = (event->pos());
        QPointF wire_pos  = this->pos();

        if(mouse_pos.x() > this->boundingRect().right() /2)
        {
            //первая точка
            wire_pos.setX(wire_pos.x() + this->boundingRect().width() /2);
            emit signalSendCoord(this,0,wire_pos,0);
        }

        if(mouse_pos.x() < this->boundingRect().left() +50)
        {
           //вторая точка
            m_add_menu->clear();
            for(quint8 i=0; i< m_LogicCellSett.count_in; i++)
            {
                m_conn_wire[i]->setEnabled((m_in_wire[i] == nullptr));
                m_add_menu->addAction(m_conn_wire[i]);
                //
            }
            m_add_menu->popup(event->screenPos());
        }

       //
    }

    if(event->button() == Qt::RightButton)
    {
        m_menu->popup(event->screenPos());
    }

}


void TLogicCell::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void TLogicCell::SetSettingsFlexLC(_flex_logic_config sett)
{
    m_LogicCellSett = sett;
    commentDevice->setText(m_LogicCellSett.comment_str);
    emit signalNeedSceneUpdate();
}

_flex_logic_config TLogicCell::GetSettingsFlexLC(void)
{
    return m_LogicCellSett;
}

void TLogicCell::slotEdit(void)
{

    emit signalEditFlexLC(this,m_LogicCellSett);

}
void TLogicCell::slotRemove(void)
{
    //здесь нужно корректно обработать удаление, сделано для отладки

    for(quint8 i=0; i<8; i++)
    {
        if(m_out_wire[i]!=nullptr)
        {
            m_out_wire[i]->RemoveLCEvent();
        }

        if(m_in_wire[i]!=nullptr)
        {
            m_in_wire[i]->RemoveLCEvent();
        }
    }

    delete this;
}

QVector <quint32> TLogicCell::GetWMInstrBuff(void)
{
    if(m_wm_instructions.length()==0)
    {
        m_wm_instructions.append(0);
    }
    return m_wm_instructions;
}

void TLogicCell::SetWMInstrBuff( QVector <quint32> instr_buff)
{
    m_wm_instructions.clear();
    m_wm_instructions.append(instr_buff);
}

void TLogicCell::SetSizeWMInstrBuff(quint8 buff_size)
{
    if(buff_size==0) buff_size=1;
    m_wm_instructions.resize(buff_size);
}

void TLogicCell::ClearWirePtr(QObject *pWire)
{
    for(quint8 i=0; i<8; i++)
    {
        if(m_out_wire[i]==pWire)
        {
            m_out_wire[i] = nullptr;
        }

        if(m_in_wire[i]==pWire)
        {
            m_in_wire[i]= nullptr;
            m_LogicCellSett.num_global_signals[i] = 0;
        }
    }
}

void TLogicCell::SetLEState(quint8 state_h,quint8 state_l)
{
    m_flex_state_union.state[_HIGH] = state_h;
    m_flex_state_union.state[_LOW]  = state_l;
}

void TLogicCell::slotWireConnect(void)
{
    QAction *action = (QAction *) sender();
    QString sel_row_text = action->text().at(3);
    quint8 num_inp = (sel_row_text.toShort()) & 0x0F;
    QPointF wire_pos  = this->pos();

    quint8 count_in = m_LogicCellSett.count_in;
    quint8 dh = this->boundingRect().height() / (1+count_in);

    wire_pos.setX(wire_pos.x() - this->boundingRect().width() /2);
    wire_pos.setY( wire_pos.y() - this->boundingRect().height() /2 + dh * num_inp );

    emit signalSendCoord(this,1,wire_pos,num_inp-1);
}

void TLogicCell::slotProcessWireConnection(bool enable)
{
    m_wire_connect_enable = enable;
    emit signalNeedSceneUpdate();
}
