#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdialogbuttonbox.h"
#include "ui_mainwindow.h"
#include "tlogiccell.h"
#include "tflexconnection.h"
#include "tflexport.h"
#include <QWidget>

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QList>

#include "vadc_class.h"

#include "crc16.h"

void MainWindow::ConfigScene(void)
{
    m_scene = new QGraphicsScene(this);   // Инициализируем графическую сцену
    m_scene->setItemIndexMethod(QGraphicsScene::NoIndex); // настраиваем индексацию элементов

    //connect(m_scene,SIGNAL(mousePressEvent),this,SLOT(slotScene_MousePress));

    //ui->graphicsView->resize(1031,451);  // Устанавливаем размер graphicsView
    ui->graphicsView->setScene(m_scene);  // Устанавливаем графическую сцену в graphicsView
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);    // Настраиваем рендер
    ui->graphicsView->setCacheMode(QGraphicsView::CacheBackground); // Кэш фона
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_scene->setSceneRect(  0,
                            0,
                            ui->graphicsView->viewport()->width()*2,
                            ui->graphicsView->viewport()->height()*2
                            ); // Устанавливаем размер сцены

    m_scene->setBackgroundBrush(Qt::lightGray);

    m_scene->addLine(QLineF(ui->graphicsView->viewport()->width(),
                          0,
                          ui->graphicsView->viewport()->width(),
                          ui->graphicsView->viewport()->height()*2),
                   QPen(Qt::white,1,Qt::DashDotLine)
                   );
    m_scene->addLine(QLineF(0,
                        ui->graphicsView->viewport()->height(),
                        ui->graphicsView->viewport()->width()*2,
                        ui->graphicsView->viewport()->height()),
                        QPen(Qt::white,1,Qt::DashDotLine)
                   );

    //qDebug() << ui->graphicsView->viewport()->rect();

    ui->graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    ui->graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    // ui->graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    // ui->graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded);
    ui->graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);

    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

   // connect(m_scene,SIGNAL(mousePressEvent(QGraphicsSceneMouseEvent)),this,SLOT(mouseScenePressEvent(QGraphicsSceneMouseEvent)));

    // QFont SceneInfoFont;
    // SceneInfoFont.setPointSize(36);
    // m_SceneInfo = m_scene->addText("Выделено элементов : 0",SceneInfoFont);
    // m_SceneInfo->setPos(-1000,450);
    // m_SceneInfo->setDefaultTextColor(Qt::white);

    m_scene->update();

    ui->InpSigncomboBox1->addItems(m_input_sign_names);
    ui->InpSigncomboBox2->addItems(m_input_sign_names);
    ui->InpSigncomboBox3->addItems(m_input_sign_names);
    ui->InpSigncomboBox4->addItems(m_input_sign_names);
    ui->InpSigncomboBox5->addItems(m_input_sign_names);
    ui->InpSigncomboBox6->addItems(m_input_sign_names);
    ui->InpSigncomboBox7->addItems(m_input_sign_names);
    ui->InpSigncomboBox8->addItems(m_input_sign_names);

    QRect frame_rect = ui->frame->geometry();
    frame_rect.setHeight(1);
    ui->frame->setGeometry(frame_rect);

}

void MainWindow::ConfigFlexLogic(void)
{
    m_input_sign_names.clear();

    m_input_sign_names.append("GND");

    for(quint16 i=1;i<MAX_FLEX_LOGIC_COUNT+1;i++)
    {
        m_input_sign_names.append("D"+QString::number(i));
    }

    m_input_sign_names.append("Gen1Hz");
    m_input_sign_names.append("Gen5Hz");

    m_input_sign_names.append("Авария");
    m_input_sign_names.append("Предупр");
    m_input_sign_names.append("L/R");

    m_input_sign_names.append("МУ Вход1");
    m_input_sign_names.append("МУ Вход2");
    m_input_sign_names.append("МУ Вход3");
    m_input_sign_names.append("МУ Вход4");

    for(quint8 i=1;i<21;i++)
    {
        m_input_sign_names.append("Вирт.накладка №"+QString::number(i));
    }

    for(quint8 i=1;i<13;i++)
    {
        m_input_sign_names.append("Кнопка F"+QString::number(i));
    }

    m_input_sign_names.append("Группа уставок 1");
    m_input_sign_names.append("Группа уставок 2");
    m_input_sign_names.append("Группа уставок 3");
    m_input_sign_names.append("Группа уставок 4");

    m_input_sign_names.append("Компаратор ВЧ основной");
    m_input_sign_names.append("Компаратор ВЧ low");
    m_input_sign_names.append("Компаратор ВЧ high");
    m_input_sign_names.append("Компаратор ВЧ ak");

    m_input_sign_names.append("Нет ответа от ПРМД №1");
    m_input_sign_names.append("Нет ответа от ПРМД №2");
    m_input_sign_names.append("Нет ответа от ПРМД №3");
    m_input_sign_names.append("Нет ответа от ПРМД №4");
    m_input_sign_names.append("Снижение уровня от ПРМД №1");
    m_input_sign_names.append("Снижение уровня от ПРМД №2");
    m_input_sign_names.append("Снижение уровня от ПРМД №3");
    m_input_sign_names.append("Снижение уровня от ПРМД №4");
    m_input_sign_names.append("Неисправность УМ");
    m_input_sign_names.append("Неисправность ОСФ");
    m_input_sign_names.append("Селективная помеха");
    m_input_sign_names.append("Смена инициатора АК");

    m_input_sign_names.append("Сброс информации");
    m_input_sign_names.append("Ввод пароля");
    m_input_sign_names.append("Снижение уровня +5В");
    m_input_sign_names.append("Снижение уровня +24В");
    m_input_sign_names.append("Блокировка ВЧ ПРД");
    m_input_sign_names.append("Длительный ОСТАНОВ");

    for(quint8 i=1;i<33;i++)
    {
        m_input_sign_names.append("МВ"+QString::number(i));
    }

    for(quint8 i=1;i<33;i++)
    {
        m_input_sign_names.append("GND МВ"+QString::number(i));
    }
    for(quint8 i=1;i<33;i++)
    {
        m_input_sign_names.append("GOOSE_"+QString::number(i));
    }

    m_input_sign_names.append("Пуск ПРД");
    m_input_sign_names.append("Пуск ДФЗ");
    m_input_sign_names.append("Работа ДФЗ");
    m_input_sign_names.append("Работа ДФЗ на сигнал");
    m_input_sign_names.append("Пуск ПРД по I1");
    m_input_sign_names.append("Пуск ПРД по I2");
    m_input_sign_names.append("Пуск ПРД по 3I0");
    m_input_sign_names.append("Пуск ПРД по dI1");
    m_input_sign_names.append("Пуск ПРД по dI2");
    m_input_sign_names.append("Пуск ПРД по FDL");
    m_input_sign_names.append("Внешний пуск ПРД");
    m_input_sign_names.append("Пуск ДФЗ по I1");
    m_input_sign_names.append("Пуск ДФЗ по I2");
    m_input_sign_names.append("Пуск ДФЗ по 3I0");
    m_input_sign_names.append("Пуск ДФЗ по dI1");
    m_input_sign_names.append("Пуск ДФЗ по dI2");
    m_input_sign_names.append("Пуск ДФЗ по FDH");
    m_input_sign_names.append("Состояние ZCA ДФЗ");
    m_input_sign_names.append("Уровень I1 для манипуляции");
    m_input_sign_names.append("Наличие манипуляции");
    m_input_sign_names.append("Сигнал вызова по ВЧ каналу");

    m_LEInput.clear();
    for(quint16 i=0;i<m_input_sign_names.count();i++)
    {
        m_LEInput.insert(m_input_sign_names.at(i),i);
    }

    ConfigScene();


    m_DebugLETimer= new QTimer(this);
    connect(m_DebugLETimer,SIGNAL(timeout()),this,SLOT(slotDebugLETimerProcedure()));
    m_DebugLETimer->stop();


    //ui->AddANDpushButton->setVisible(false);
}

void MainWindow::slotDebugLETimerProcedure()
{
    mVADC->VADC_GetLEState();
}

void MainWindow::slotRcvLEState(QByteArray le_state)
{
    qDebug() << le_state;

    quint8 state_h,state_l;
    union tbits16 count_le;

    count_le.byte_data[_HIGH] = le_state.at(4);
    count_le.byte_data[_LOW]  = le_state.at(5);

    count_le.short_data = (count_le.short_data >>1);

    for(quint16 i=0;i<count_le.short_data;i++)
    {
        state_h = le_state.at(6+2*i);
        state_l = le_state.at(7+2*i);

        if(m_le_ptrs.at(i)!=nullptr)
        {
            m_le_ptrs.at(i)->SetLEState(state_h,state_l);
        }

    }

    m_scene->update();
}

void MainWindow::on_checkBox_2_checkStateChanged(const Qt::CheckState &arg1)
{
    if(m_le_ptrs.size() == 0)
    {
        ui->checkBox_2->setCheckState(Qt::Unchecked);

        emit signalSetDebugMode(false);
        return;
    }

    if(arg1 == Qt::Checked)
    {
        m_DebugLETimer->start(500);
        emit signalSetDebugMode(true);
    } else
    {
        m_DebugLETimer->stop();
        emit signalSetDebugMode(false);
    }
}

TLogicCell* MainWindow::AddLogicCell(quint16 number,quint8 type, float x,float y)
{
    if(m_LogicCellsCount>=MAX_FLEX_LOGIC_COUNT) return nullptr;


    TLogicCell *LCell= new TLogicCell(this,x,y);
    LCell->setFlag(QGraphicsItem::ItemIsMovable,true);
    LCell->setFlag(QGraphicsItem::ItemIsSelectable,true);
    LCell->SetNumberLogicCell(number);
    LCell->SetTypeLogicCell(type);
    m_scene->addItem(LCell);

    m_scene->clearSelection();
    m_scene->update();

    ui->FlexLCcountlabel->setText("Количество элементов: "+ QString::number(m_LogicCellsCount));

    return LCell;
}

TFlexPort* MainWindow::AddPort(quint16 sign_type,QString sign_name, float x,float y)
{
    TFlexPort *FPort= new TFlexPort(this,sign_type,sign_name,x,y);
    FPort->setFlag(QGraphicsItem::ItemIsMovable,true);
    FPort->setFlag(QGraphicsItem::ItemIsSelectable,true);
    m_scene->addItem(FPort);
    m_scene->update();

    return FPort;
}

void MainWindow::on_AddORpushButton_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Тип элемента"));

    QComboBox *combo = new QComboBox(&dlg);

    for(quint8 i = 0; i< _LOGIC_OUTPUT_LOG; i++)
    {
        combo->addItem(logic_names[i]);
    }

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Тип элемента:"), combo);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);

    // В случае, если пользователь нажал "Ok".
    if(dlg.exec() == QDialog::Accepted)
    {
        quint32 index = combo->currentIndex();
        m_LogicCellsCount++;
        AddLogicCell(m_LogicCellsCount,index,0.0f,0.0f);
    }
}

void MainWindow::on_AddANDpushButton_clicked()
{
    ui->graphicsView->scale(2.0,2.0);

}


void MainWindow::on_AddXORpushButton_clicked()
{
    ui->graphicsView->scale(0.5,0.5);

}

void MainWindow::on_WirepushButton_clicked()
{
 //
}

void MainWindow::on_pushButton_15_clicked()
{

    QDialog dlg(this);
    dlg.setWindowTitle(tr("Тип входа"));

    QComboBox *combo = new QComboBox(&dlg);

    combo->addItem("GND");
    for(quint16 i=MAX_FLEX_LOGIC_COUNT+1;i<m_input_sign_names.count();i++) //убираем номера выходов логических элементов
    {
        combo->addItem(m_input_sign_names.at(i));
    }

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Тип входа:"), combo);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);

    // В случае, если пользователь нажал "Ok".
    if(dlg.exec() == QDialog::Accepted)
    {
        quint32 sign_type = combo->currentIndex();
        QString sign_name = combo->currentText();

        // QString sign_name = combo->currentText();
        // quint32 sign_type = m_LEInput[sign_name];

        AddPort(sign_type,sign_name,0.0f,0.0f);
    }
}

void MainWindow::on_PortpushButton_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Тип выхода"));

    QComboBox *combo = new QComboBox(&dlg);

    combo->addItem("LOG");
    combo->addItem("LED");
    combo->addItem("SMART LED");
    combo->addItem("RELE");
    combo->addItem("GOOSE");
    combo->addItem("MZ REG");
    combo->addItem("WR OSC");
    combo->addItem("PUSK PRD");
    combo->addItem("RESET INF");
    combo->addItem("GROUP_SETT");
    combo->addItem("AVAR_PREDUPR");

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    layout->addRow(tr("Тип выхода:"), combo);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);

    // В случае, если пользователь нажал "Ok".
    if(dlg.exec() == QDialog::Accepted)
    {
        quint32 index = combo->currentIndex();
        m_LogicCellsCount++;
        AddLogicCell(m_LogicCellsCount,index + _LOGIC_OUTPUT_LOG,0.0f,0.0f);
    }

}

void MainWindow::slotSetLineCoord(QObject *pXCell, bool finish_coord, QPointF coord,quint16 num_input)
{
    static QObject *LC[2];
    static QPointF LineCoord[2];
    static quint16 num_element;

    m_scene->clearSelection();
    m_scene->update();

    if(ui->WirepushButton->isChecked())
    {
        LC[finish_coord] = pXCell;
        LineCoord[finish_coord] = coord;

        if(finish_coord)
        {
            if((LC[0]!= nullptr) && (LC[0]!=LC[1]))
            {
                TFlexConnection *FlexConnection = new TFlexConnection(this,LineCoord[0],LineCoord[1],LC[0],LC[1]);

                if(FlexConnection)
                {
                    FlexConnection->SetNumInpLC(quint8 (num_input & 0x0F ));
                }

                m_scene->addItem(FlexConnection);
                ui->WirepushButton->setChecked(false);
                emit signalProcessWireConnection(false);

                //выход
                TLogicCell *pCell = dynamic_cast <TLogicCell *>( LC[0]);
                TFlexPort *pPort  = dynamic_cast <TFlexPort *>(  LC[0]);

                if(pCell)
                {
                    pCell->SetLogicCellWirePtr(0,FlexConnection);
                    num_element = pCell->GetOUTSRCCell();
                }

                if(pPort)
                {
                    pPort->SetLogicCellWirePtr(0,FlexConnection);
                    num_element = pPort->GetOUTSRCCell();
                }

                //вход
                pCell = dynamic_cast <TLogicCell *>( LC[1]);

                if(pCell)
                {
                    pCell->SetLogicCellWirePtr(num_input+1,FlexConnection);
                    pCell->SetInSRCCell(num_input,num_element);
                }
            }
        }

    } else
    {
        LC[0] = nullptr;
        LC[1] = nullptr;
    }

}

void MainWindow::slotSceneUpdate(void)
{
     m_scene->update();
}

void MainWindow::DrawLCPicture(quint8 LCType)
{
    QString image[_LOGIC_COUNT];

    for (int i = 0; i < _LOGIC_COUNT; ++i)
    {
        image[i] = ":/res/FlexLogic/Pictures/AND_gate_RU.jpg";
    }

    image[_LOGIC_WIRE] = ":/res/FlexLogic/Pictures/Da-yes.jpg";
    image[_LOGIC_OR] = ":/res/FlexLogic/Pictures/OR_gate_RU.jpg";
    image[_LOGIC_XOR] = ":/res/FlexLogic/Pictures/XOR_gate_RU.jpg";

    image[_LOGIC_OUTPUT_LOG] = ":/res/FlexLogic/Pictures/PORT_OUT.jpg";
    image[_LOGIC_OUTPUT_LED] = ":/res/FlexLogic/Pictures/PORT_OUT.jpg";
    image[_LOGIC_OUTPUT_RELE] = ":/res/FlexLogic/Pictures/PORT_OUT.jpg";
    image[_LOGIC_OUTPUT_GOOSE] = ":/res/FlexLogic/Pictures/PORT_OUT.jpg";
    image[_LOGIC_OUTPUT_MZ_REGS] = ":/res/FlexLogic/Pictures/PORT_OUT.jpg";

    QPixmap pict= QPixmap (image[LCType]);
    ui->LCPicturelabel->setPixmap(pict);
}

void MainWindow::on_LogicSigncomboBox_currentIndexChanged(int index)
{
    DrawLCPicture(index);
}

void MainWindow::FillLCSettings(_flex_logic_config sett)
{
    union tbits8 inv_state;
    inv_state.byte_data = sett.inv_inp_signals;

    ui->InvInput1checkBox->setChecked(inv_state.single_bits.bit0);
    ui->InvInput2checkBox->setChecked(inv_state.single_bits.bit1);
    ui->InvInput3checkBox->setChecked(inv_state.single_bits.bit2);
    ui->InvInput4checkBox->setChecked(inv_state.single_bits.bit3);
    ui->InvInput5checkBox->setChecked(inv_state.single_bits.bit4);
    ui->InvInput6checkBox->setChecked(inv_state.single_bits.bit5);
    ui->InvInput7checkBox->setChecked(inv_state.single_bits.bit6);
    ui->InvInput8checkBox->setChecked(inv_state.single_bits.bit7);

    ui->InpSigncomboBox1->setCurrentIndex(sett.num_global_signals[0]);
    ui->InpSigncomboBox2->setCurrentIndex(sett.num_global_signals[1]);
    ui->InpSigncomboBox3->setCurrentIndex(sett.num_global_signals[2]);
    ui->InpSigncomboBox4->setCurrentIndex(sett.num_global_signals[3]);
    ui->InpSigncomboBox5->setCurrentIndex(sett.num_global_signals[4]);
    ui->InpSigncomboBox6->setCurrentIndex(sett.num_global_signals[5]);
    ui->InpSigncomboBox7->setCurrentIndex(sett.num_global_signals[6]);
    ui->InpSigncomboBox8->setCurrentIndex(sett.num_global_signals[7]);

    ui->OutInvcheckBox->setChecked(sett.inv_output_signal);

    ui->FlexNumInpspinBox->setValue(sett.count_in);

    ui->LogicSigncomboBox->setCurrentIndex(sett.logic_func_type);

    QString ElemPD;
    ElemPD.clear();

    if(sett.logic_func_type > _LOGIC_FLEX_VM)
    {
        ElemPD.append('S');
    } else
    {
        ElemPD.append('D');
    }

    ui->DNumlabel->setText(ElemPD + QString::number(sett.num_element));

    QString sparam;
    sparam = QString("%1").arg(sett.param_element, 8, 16, QLatin1Char( '0' ));
    ui->LCParamlineEdit->setText( sparam );

    ui->FlexLCCommentlineEdit->setText(sett.comment_str);

    union tconvert_int lc_param = {0};

    lc_param.int_val = sett.param_element;
    ui->LCPram4spinBox->setValue(lc_param.char_val[_HIGH2]);
    ui->LCPram3spinBox->setValue(lc_param.char_val[_LOW2]);
    ui->LCPram2spinBox->setValue(lc_param.char_val[_HIGH]);
    ui->LCPram1spinBox->setValue(lc_param.char_val[_LOW]);

    ui->LCPram4Label->setText("---");
    ui->LCPram3Label->setText("---");
    ui->LCPram2Label->setText("---");
    ui->LCPram1Label->setText("---");

    if(sett.logic_func_type == _LOGIC_OUTPUT_LOG)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("I2");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_LED)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("xRGB");
        ui->LCPram1Label->setText("LED №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_SMART_LED)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("blink speed");
        ui->LCPram2Label->setText("xRGB");
        ui->LCPram1Label->setText("LED №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_RELE)
    {
        ui->LCPram4Label->setText("t imp");
        ui->LCPram3Label->setText("delay off");
        ui->LCPram2Label->setText("delay on");
        ui->LCPram1Label->setText("RELE №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_GOOSE)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("GOOSE №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_MZ_REGS)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("MZ SIGNAL №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_WR_OSC)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("OSC SIGNAL №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_PUSK_PRD)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("PUSK TYPE");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_EXT_RESET)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("---");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_SET_GROUP_SETT)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("SETT GROUP №");
    }

    if(sett.logic_func_type == _LOGIC_OUTPUT_AVAR_PREDUPR)
    {
        ui->LCPram4Label->setText("---");
        ui->LCPram3Label->setText("---");
        ui->LCPram2Label->setText("---");
        ui->LCPram1Label->setText("SIGNAL TYPE");
    }

    bool isWM = (bool)(sett.logic_func_type == _LOGIC_FLEX_VM);
    ui->ConfigWMpushButton->setVisible(isWM);
    ui->LCParamlineEdit->setEnabled(!isWM);
    ui->label_33->setVisible(!isWM);
    ui->wm_inst_count_label->setVisible(isWM);
    ui->wm_inst_count_spinBox->setVisible(isWM);
    ui->wm_inst_count_spinBox->setValue(sett.param_element & 0x3F);

    ui->OutInvcheckBox->setVisible((sett.logic_func_type<=_LOGIC_FLEX_VM));

    DrawLCPicture(sett.logic_func_type);
}

_flex_logic_config MainWindow::ReadLCSettings(void)
{
    _flex_logic_config sett;

    union tbits8 inv_state;

    inv_state.single_bits.bit0 = ui->InvInput1checkBox->isChecked();
    inv_state.single_bits.bit1 = ui->InvInput2checkBox->isChecked();
    inv_state.single_bits.bit2 = ui->InvInput3checkBox->isChecked();
    inv_state.single_bits.bit3 = ui->InvInput4checkBox->isChecked();
    inv_state.single_bits.bit4 = ui->InvInput5checkBox->isChecked();
    inv_state.single_bits.bit5 = ui->InvInput6checkBox->isChecked();
    inv_state.single_bits.bit6 = ui->InvInput7checkBox->isChecked();
    inv_state.single_bits.bit7 = ui->InvInput8checkBox->isChecked();

    sett.num_global_signals[0] = ui->InpSigncomboBox1->currentIndex();
    sett.num_global_signals[1] = ui->InpSigncomboBox2->currentIndex();
    sett.num_global_signals[2] = ui->InpSigncomboBox3->currentIndex();
    sett.num_global_signals[3] = ui->InpSigncomboBox4->currentIndex();
    sett.num_global_signals[4] = ui->InpSigncomboBox5->currentIndex();
    sett.num_global_signals[5] = ui->InpSigncomboBox6->currentIndex();
    sett.num_global_signals[6] = ui->InpSigncomboBox7->currentIndex();
    sett.num_global_signals[7] = ui->InpSigncomboBox8->currentIndex();

    sett.inv_inp_signals = inv_state.byte_data;

    sett.inv_output_signal = ui->OutInvcheckBox->isChecked();

    sett.count_in = ui->FlexNumInpspinBox->value();

    sett.logic_func_type = ui->LogicSigncomboBox->currentIndex();

    QByteArray strArray = ui->FlexLCCommentlineEdit->text().toLocal8Bit();//toUtf8();//

    memcpy(sett.comment_str,strArray.data(),15);
    sett.comment_str[15] = '\0';

    if(sett.logic_func_type == _LOGIC_WIRE) sett.count_in = 1;
    if(sett.logic_func_type == _LOGIC_XOR)  sett.count_in = 2;

    if(
        sett.logic_func_type == _LOGIC_COUNTER_TIMER ||
        sett.logic_func_type == _LOGIC_DELAY_TIMER   ||
        sett.logic_func_type == _LOGIC_PROLONG_TIMER ||
        sett.logic_func_type == _LOGIC_IMP_TIMER
      )
    {
        sett.count_in = 2;
    }

    bool res;
    sett.param_element = ui->LCParamlineEdit->text().toUInt(&res,16);

    sett.num_element = m_saveLCsett.num_element;

    for(quint8 i = 0; i< sizeof(sett.reserv); i++)
    {
        sett.reserv[i] = 0xE0;
    }

    return sett;
}

void MainWindow::slotEditFlexLC(QObject *pCell, _flex_logic_config sett)
{
    m_WorkLogCell = static_cast<TLogicCell *>(pCell);
    QRect frame_rect = ui->frame->geometry();
    frame_rect.setHeight(560);
    ui->frame->setGeometry(frame_rect);

    m_saveLCsett = sett;

    FillLCSettings(sett);
}

void MainWindow::on_FlexSettCloseupushButton_clicked()
{
    QRect frame_rect = ui->frame->geometry();
    frame_rect.setHeight(1);
    ui->frame->setGeometry(frame_rect);
    m_WorkLogCell = nullptr;
    m_scene->update();
}

void MainWindow::on_FlexSettOKpushButton_clicked()
{
    _flex_logic_config sett;
    sett = ReadLCSettings();
    m_WorkLogCell->SetSettingsFlexLC(sett);
    m_scene->update();

    // if(sett.logic_func_type==_LOGIC_FLEX_VM)
    // {
    //     QVector <quint32> debug_data;
    //     debug_data = m_WorkLogCell->GetWMInstrBuff();

    //     qDebug() << debug_data;
    // }

}


void MainWindow::on_FlexSettCancelpushButton_clicked()
{
     FillLCSettings(m_saveLCsett);
}


void MainWindow::on_FlexSeqSizespinBox_valueChanged(int arg1)
{
    Q_UNUSED(arg1);
}


void MainWindow::on_UpdateFlexSeqpushButton_clicked()
{
    if(m_scene==nullptr) return;


    QList <QGraphicsItem*>objl;
    objl=m_scene->items();

    ui->LEtableWidget->setRowCount(0);
    m_MCFlexLogicConfig.clear();
    m_MCFlexWMCode.clear();

    for(int i=0; i<objl.size(); i++)
    {
        TLogicCell * cell = dynamic_cast <TLogicCell *> (objl[i]);
        if (cell)
        {
            quint16 num_row = ui->LEtableWidget->rowCount();
            ui->LEtableWidget->insertRow(num_row);

            struct _flex_logic_config lc_config = cell->GetSettingsFlexLC();

            m_MCFlexLogicConfig.append(lc_config);

            if(lc_config.logic_func_type == _LOGIC_FLEX_VM)
            {
                m_MCFlexWMCode.append(cell->GetWMInstrBuff());
            }

            if(lc_config.logic_func_type>_LOGIC_FLEX_VM)
            {
            ui->LEtableWidget->setItem(num_row, 0, new QTableWidgetItem("S" + QString::number(cell->GetNumberLogicCell())));
            } else
            {
            ui->LEtableWidget->setItem(num_row, 0, new QTableWidgetItem("D" + QString::number(cell->GetNumberLogicCell())));
            }

            ui->LEtableWidget->setItem(num_row, 1, new QTableWidgetItem(logic_names[lc_config.logic_func_type]));
             ui->LEtableWidget->setItem(num_row, 2, new QTableWidgetItem(QString::number(lc_config.count_in)));
            for(quint8 i=0;i<5;i++)
            {
                quint16 ns = lc_config.num_global_signals[i];
                ui->LEtableWidget->setItem(num_row, 3+i, new QTableWidgetItem(m_input_sign_names.at(ns)));
            }   

            continue;
        }
    }

    ui->FlexSeqSizespinBox->setValue(ui->LEtableWidget->rowCount());
}

void MainWindow::on_pushButton_27_clicked() //create file
{
    QFile SettFile;
    QByteArray data_sett;
    quint16 sett_count = 0;
    char *pWrStruct;
    quint16 wm_code_offset;
    quint16 wm_lc_offset = 0;
    union _prog_param_flex wm_param;
    union tconvert_short file_size = {.shortval=16};
    union tconvert_short le_count;
    union tconvert_short crc16;

    data_sett.clear();
    data_sett.append(0x64); //ver flex fw

    sett_count = m_MCFlexLogicConfig.count();
    data_sett.append(file_size.charval[_LOW]);
    data_sett.append(file_size.charval[_HIGH]); //размер файла

    le_count.shortval = sett_count;
    data_sett.append(le_count.charval[_LOW]);
    data_sett.append(le_count.charval[_HIGH]); //количество элементов схемы

    quint8 rnd = (rand() % (255 + 1)); //случайное число для изменения контролльной суммы при регенерации схемы
    data_sett.append(rnd);

    for(quint8 i = 6; i< 16; i++) data_sett.append(0xE0); //резерв - empty

    wm_code_offset = sett_count * 48 + 16; //32 байта на настройку+16 на заголовок файла

    for(quint16 i = 0; i<sett_count; i++ )
    {
        pWrStruct = ((char *)&m_MCFlexLogicConfig.at(i));

        if(m_MCFlexLogicConfig.at(i).logic_func_type == _LOGIC_FLEX_VM) //корректируем данные по смещению кода в файле для wm
        {
            wm_param.int_data = m_MCFlexLogicConfig.at(i).param_element;
            wm_param.parts.instr_offset = wm_code_offset + wm_lc_offset;
            m_MCFlexLogicConfig[i].param_element = wm_param.int_data;

            wm_lc_offset+=wm_param.parts.instr_count*4;
        }

        data_sett.append(pWrStruct, sizeof(struct _flex_logic_config));
    }

    if(m_MCFlexWMCode.length()>0)
    {
        pWrStruct = (char *) m_MCFlexWMCode.data();
        data_sett.append(pWrStruct, m_MCFlexWMCode.length()*4);
    }

    //корректируем размер файла
    file_size.shortval = data_sett.length() + 2;
    data_sett[1] = file_size.charval[_LOW];
    data_sett[2] = file_size.charval[_HIGH];

    //добавляем контрольную сумму в конец файла
    crc16.shortval = CRC16(&data_sett,file_size.shortval-2);
    data_sett.append(crc16.charval[_LOW]);
    data_sett.append(crc16.charval[_HIGH]);

    SettFile.setFileName("settings.flexlogic");
    SettFile.open(QIODeviceBase::WriteOnly);
    SettFile.write(data_sett);
    SettFile.close();

}

void MainWindow::on_ConfigWMpushButton_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Редактирование кода WM"));

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    QFormLayout *layout = new QFormLayout();
    QTableWidget *ConfigTable = new QTableWidget(&dlg);
    QStringList hdr, combo_items;
    QComboBox *combo_rd;
    quint8 instr_count = ui->wm_inst_count_spinBox->value();
    QVector <quint32>  wm_instructions;
    wm_instructions = m_WorkLogCell->GetWMInstrBuff();
    union _prog_instruction_flex instr;

    hdr.clear();
    hdr.append("Код инструкции");
    hdr.append("Вход 1");
    hdr.append("Вход 2");
    hdr.append("Вход 3");
    hdr.append("Вход 4");

    ConfigTable->setColumnCount(5);
    ConfigTable->setRowCount(instr_count);
    ConfigTable->setHorizontalHeaderLabels(hdr);

    combo_items.clear();
    combo_items.append("NOP");
    combo_items.append("NOT");
    combo_items.append("XOR");
    combo_items.append("OR2");
    combo_items.append("OR3");
    combo_items.append("OR4");
    combo_items.append("AND2");
    combo_items.append("AND3");
    combo_items.append("AND4");
    combo_items.append("NAND2");
    combo_items.append("NAND3");
    combo_items.append("NAND4");
    combo_items.append("AND4_NOT4");
    combo_items.append("RS");

     for(quint8 i=0;i<instr_count;i++)
     {
        instr.int_data = 0;

        if(i < wm_instructions.length())
        {
            instr.int_data = wm_instructions.at(i);
        }

        QComboBox *combo = new QComboBox();
        combo->addItems(combo_items);
        combo->setCurrentIndex(instr.parts.instr);
        ConfigTable->setCellWidget(i,0,combo);
     }

    combo_items.clear();

    for(quint8 i=1;i<9;i++)
    {
        combo_items.append("CELL_INP"+QString::number(i));
    }

    for(quint8 i=8;i<64;i++)
    {
        combo_items.append("VD"+QString::number(i-7));
    }

    for(quint8 i=0;i<instr_count;i++)
    {
        instr.int_data = 0;

        if(i < wm_instructions.length())
        {
            instr.int_data = wm_instructions.at(i);
        }

        QComboBox *combo2 = new QComboBox();
        combo2->addItems(combo_items);
        combo2->setCurrentIndex(instr.parts.op1);
        ConfigTable->setCellWidget(i,1,combo2);

        QComboBox *combo3 = new QComboBox();
        combo3->addItems(combo_items);
        combo3->setCurrentIndex(instr.parts.op2);
        ConfigTable->setCellWidget(i,2,combo3);

        QComboBox *combo4 = new QComboBox();
        combo4->addItems(combo_items);
        combo4->setCurrentIndex(instr.parts.op3);
        ConfigTable->setCellWidget(i,3,combo4);

        QComboBox *combo5 = new QComboBox();
        combo5->addItems(combo_items);
        combo5->setCurrentIndex(instr.parts.op4);
        ConfigTable->setCellWidget(i,4,combo5);
    }

    layout->addWidget(ConfigTable);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);
    dlg.setMinimumHeight(700);
    dlg.setMinimumWidth(580);

    if(dlg.exec() == QDialog::Accepted)
    {
        QVector <quint32> wm_code;
        wm_code.clear();

        for(quint8 i=0;i<instr_count;i++)
        {
            instr.int_data = 0;

            combo_rd = (QComboBox *)ConfigTable->cellWidget(i,0);
            instr.parts.instr = combo_rd->currentIndex();
            combo_rd = (QComboBox *)ConfigTable->cellWidget(i,1);
            instr.parts.op1 = combo_rd->currentIndex();
            combo_rd = (QComboBox *)ConfigTable->cellWidget(i,2);
            instr.parts.op2 = combo_rd->currentIndex();
            combo_rd = (QComboBox *)ConfigTable->cellWidget(i,3);
            instr.parts.op3 = combo_rd->currentIndex();
            combo_rd = (QComboBox *)ConfigTable->cellWidget(i,4);
            instr.parts.op4 = combo_rd->currentIndex();

            wm_code.append(instr.int_data);
            //qDebug() << instr.int_data;
        }

        m_WorkLogCell->SetWMInstrBuff(wm_code);

        //qDebug() << "wm cell code create complete";
    }

}

void MainWindow::on_wm_inst_count_spinBox_valueChanged(int arg1)
{
    union _prog_param_flex param;

    param.int_data = ui->LCParamlineEdit->text().toUInt();
    param.parts.instr_count = ui->wm_inst_count_spinBox->value();
    ui->LCParamlineEdit->setText(QString::number(param.int_data));

    if(m_WorkLogCell!= nullptr)
    {
        m_WorkLogCell->SetSizeWMInstrBuff(arg1);
    }
}

void MainWindow::on_graphicsView_rubberBandChanged(const QRect &viewportRect, const QPointF &fromScenePoint, const QPointF &toScenePoint)
{
    static QRect save_rect;

    if(viewportRect.left()==0 && viewportRect.top()==0 && viewportRect.width()==0 && viewportRect.height() == 0)
    {
        //QList si = m_scene->selectedItems();

        // if(m_SceneInfo) delete m_SceneInfo;

        // QFont SceneInfoFont;
        // SceneInfoFont.setPointSize(36);

        // m_SceneInfo = m_scene->addText("Выделено элементов : " + QString::number(si.length()),SceneInfoFont);
        // m_SceneInfo->setPos(-1000,450);
        // m_SceneInfo->setDefaultTextColor(Qt::white);
    }

    save_rect = viewportRect;

    Q_UNUSED(fromScenePoint);
    Q_UNUSED(toScenePoint);
}

void MainWindow::on_SceneCleapushButton_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Очистить схему?"));

    QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    dlg.setFixedWidth(btn_box->width()*1.7);

    // В случае, если пользователь нажал "Ok".
    if(dlg.exec() == QDialog::Accepted)
    {
        QList <QGraphicsItem*>objl;
        objl=m_scene->items();

        for(int i=0; i<objl.size(); i++)
        {
            delete objl[i];
        }

        m_LogicCellsCount = 0;
        ui->FlexLCcountlabel->setText("Количество элементов: 0");
    }
}

void MainWindow::slotScene_MousePress(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "scene press";

    Q_UNUSED(event);
}

void MainWindow::on_pushButton_31_clicked()
{
    ui->TeminallistWidget->clear();
    ui->TeminallistWidget->addItem("Load flex settings start...");
    QCoreApplication::processEvents();
    QProcess process;
    process.start("wsl", QStringList() << "bash" << "-c" << "/mnt/d/Work/imx6/linux/DFZ330/prog_flex_sett_lan.sh");

    if (process.waitForFinished()) {
        ui->TeminallistWidget->addItem(QString::fromUtf8(process.readAllStandardOutput()));
        ui->TeminallistWidget->addItem(QString::fromUtf8(process.readAllStandardError()));
        // qDebug() << "Output:" << QString::fromUtf8(process.readAllStandardOutput());
        // qDebug() << "Сообщение об ошибке:" << process.readAllStandardError();
    } else {
        ui->TeminallistWidget->addItem(QString::fromUtf8(process.readAllStandardError()));
       // qDebug() << "Process failed to finish.";
    }

}

void MainWindow:: UpdateParamLCFromCheckBox(void)
{
    union tconvert_int lc_param = {0};

    lc_param.char_val[_HIGH2]   = ui->LCPram4spinBox->value();
    lc_param.char_val[_LOW2]    = ui->LCPram3spinBox->value();
    lc_param.char_val[_HIGH]    = ui->LCPram2spinBox->value();
    lc_param.char_val[_LOW]     = ui->LCPram1spinBox->value();

    QString sparam;
    sparam = QString("%1").arg(lc_param.int_val, 8, 16, QLatin1Char( '0' ));
    ui->LCParamlineEdit->setText( sparam );
}

void MainWindow::SaveFlexProject()
{
    QSettings *settings;
    struct _flex_logic_config logic_config;
    quint16 lc_count = 0;
    quint16 wire_count = 0;
    quint16 ports_count = 0;
    bool is_wm = false;

    if(m_scene==nullptr) return;

    settings = new QSettings(m_conf_file,QSettings::IniFormat);

    settings->clear();
    settings->setValue("Main/all_lc_count",0);
    settings->setValue("Main/all_ports_count",0);
    settings->setValue("Main/all_wire_count",0);
    settings->sync();

    QList <QGraphicsItem*>objl;
    objl=m_scene->items();

    for(int i=0; i<objl.size(); i++)
    {
        TLogicCell * cell = dynamic_cast <TLogicCell *> (objl[i]);
        if (cell)
        {
            lc_count++;
            logic_config = cell->GetSettingsFlexLC();

            settings->beginGroup("FlexCells/LC" + QString::number(logic_config.num_element));

            settings->setValue("x", cell->x());
            settings->setValue("y", cell->y());

            settings->setValue("lc_number", logic_config.num_element);
            settings->setValue("lc_type", logic_config.logic_func_type);
            settings->setValue("lc_count_in", logic_config.count_in);
            settings->setValue("lc_inv_inp", logic_config.inv_inp_signals);
            settings->setValue("lc_inv_outp", logic_config.inv_output_signal);

            settings->setValue("lc_inp1", logic_config.num_global_signals[0]);
            settings->setValue("lc_inp2", logic_config.num_global_signals[1]);
            settings->setValue("lc_inp3", logic_config.num_global_signals[2]);
            settings->setValue("lc_inp4", logic_config.num_global_signals[3]);
            settings->setValue("lc_inp5", logic_config.num_global_signals[4]);
            settings->setValue("lc_inp6", logic_config.num_global_signals[5]);
            settings->setValue("lc_inp7", logic_config.num_global_signals[6]);
            settings->setValue("lc_inp8", logic_config.num_global_signals[7]);

            settings->setValue("lc_param", logic_config.param_element);

            settings->beginWriteArray("Comment");

            for(quint8 i=0;i<sizeof(logic_config.comment_str);i++)
            {
                settings->setArrayIndex(i);
                settings->setValue("ch", logic_config.comment_str[i]);
            }

            settings->endArray();

            is_wm = false;

            if(logic_config.logic_func_type == _LOGIC_FLEX_VM)
            {
                QVector <quint32> instr = cell->GetWMInstrBuff();

                is_wm = true;

                settings->beginWriteArray("VM_Code");
                for (int i = 0; i < instr.size(); ++i) {
                    settings->setArrayIndex(i);
                    settings->setValue("code", instr.at(i));
                }
                settings->endArray();
            }

            settings->setValue("is_vm", is_wm);

            settings->endGroup();

            continue;
        }
    }

    for(int i=0; i<objl.size(); i++)
    {
        TFlexPort * port = dynamic_cast <TFlexPort *> (objl[i]);
        if (port)
        {
            ports_count++;

            port->SetNumber(ports_count); //нумеруем порты только при сохранении, чтобы учесть добавленные позже

            settings->beginGroup("FlexPorts/Port" + QString::number(ports_count));

            settings->setValue("x", port->x());
            settings->setValue("y", port->y());

            quint16 ptype = port->GetOUTSRCCell();
            if(ptype!=0) ptype-=MAX_FLEX_LOGIC_COUNT;
            settings->setValue("type", ptype );
            settings->setValue("name", port->GetFuncString());
            settings->setValue("port_num", port->GetNumber());


            settings->endGroup();

            continue;
        }
    }

    for(int i=0; i<objl.size(); i++)
    {
        TFlexConnection * connection = dynamic_cast <TFlexConnection *> (objl[i]);
        if (connection)
        {
            QObject *pInputSRC = nullptr;
            QObject *pOutputSRC = nullptr;

            wire_count++;

            settings->beginGroup("FlexConnections/Wire" + QString::number(wire_count));

            settings->setValue("start_point", connection->GetStartPoint() );
            settings->setValue("finish_point",connection->GetFinishPoint());
            settings->setValue("center_point",connection->GetCenterPoint());

            pInputSRC = connection->GetStartObj();
            pOutputSRC = connection->GetFinishObj();

            TFlexPort *port = dynamic_cast <TFlexPort *> (pInputSRC);
            if(port)
            {
                settings->setValue("inp_port",true);
                settings->setValue("port_num",port->GetNumber());
                settings->setValue("port_type",port->GetOUTSRCCell());
                settings->setValue("port_name",port->GetFuncString());
            }

            TLogicCell *inp_cell = dynamic_cast <TLogicCell *> (pInputSRC);
            if(inp_cell)
            {
                settings->setValue("inp_port",false);
                settings->setValue("inp_cell_num",inp_cell->GetNumberLogicCell());
            }

            TLogicCell *outp_cell = dynamic_cast <TLogicCell *> (pOutputSRC);
            if(outp_cell)
            {
                settings->setValue("outp_cell_num",outp_cell->GetNumberLogicCell());
                settings->setValue("cell_inp_num",connection->GetNumInpLC());
            }

            settings->endGroup();

            continue;

        }
    }

    settings->setValue("Main/all_lc_count",lc_count);
    settings->setValue("Main/all_ports_count",ports_count);
    settings->setValue("Main/all_wire_count",wire_count);

    settings->sync(); //записываем настройки
}

void MainWindow::OpenFlexProject()
{
    QSettings *settings;
    quint16 lc_count = 0;
    quint16 wire_count = 0;
    quint16 ports_count = 0;
    TLogicCell *pCell;
    float lc_x,lc_y;
    struct _flex_logic_config logic_config;
    QString port_name;
    quint16 port_src;
    quint16 port_num;

    QVector <TLogicCell *> lc_array;
    QVector <TFlexPort *> ports_array;

    bool is_wm = false;
    QVector <quint32> instr;

    lc_array.clear();
    ports_array.clear();


    // QDialog dlg(this);
    // dlg.setWindowTitle(tr("Загрузить проект из файла?"));

    // QDialogButtonBox *btn_box = new QDialogButtonBox(&dlg);
    // btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    // connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    // connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    // dlg.setFixedWidth(btn_box->width()*2.3);

    // // В случае, если пользователь нажал "Ok".
    // if(dlg.exec() == QDialog::Accepted)
    {
        QList <QGraphicsItem*>objl;
        objl=m_scene->items();

        for(int i=0; i<objl.size(); i++)
        {
            delete objl[i];
        }

        m_LogicCellsCount = 0;
        ui->FlexLCcountlabel->setText("Количество элементов: 0");



        settings = new QSettings(m_conf_file,QSettings::IniFormat);

        lc_count = settings->value("Main/all_lc_count").toUInt();

        ports_count = settings->value("Main/all_ports_count").toUInt();

        wire_count = settings->value("Main/all_wire_count").toUInt();

        m_LogicCellsCount = lc_count;

        for(quint16 i=0;i<lc_count;i++)
        {
            settings->beginGroup("FlexCells/LC" + QString::number(i+1));

            lc_x = settings->value("x").toFloat();
            lc_y = settings->value("y").toFloat();

            logic_config.num_element = settings->value("lc_number").toUInt();
            logic_config.logic_func_type = settings->value("lc_type").toUInt();
            logic_config.count_in = settings->value("lc_count_in").toUInt();
            logic_config.inv_inp_signals = settings->value("lc_inv_inp").toUInt();
            logic_config.inv_output_signal = settings->value("lc_inv_outp").toBool();

            logic_config.num_global_signals[0] = settings->value("lc_inp1").toUInt();
            logic_config.num_global_signals[1] = settings->value("lc_inp2").toUInt();
            logic_config.num_global_signals[2] = settings->value("lc_inp3").toUInt();
            logic_config.num_global_signals[3] = settings->value("lc_inp4").toUInt();
            logic_config.num_global_signals[4] = settings->value("lc_inp5").toUInt();
            logic_config.num_global_signals[5] = settings->value("lc_inp6").toUInt();
            logic_config.num_global_signals[6] = settings->value("lc_inp7").toUInt();
            logic_config.num_global_signals[7] = settings->value("lc_inp8").toUInt();

            logic_config.param_element = settings->value("lc_param").toUInt();

            int size = settings->beginReadArray("Comment");

            for (int i = 0; i < size; i++)
            {
                settings->setArrayIndex(i);
                qint8 data_char = settings->value("ch").toInt();
                logic_config.comment_str[i] = data_char & 0xFF;
            }

            settings->endArray();


            is_wm = settings->value("is_vm").toBool();
            if(is_wm)
            {
                instr.clear();

                int size = settings->beginReadArray("VM_Code");

                for (int i = 0; i < size; i++)
                {
                    settings->setArrayIndex(i);
                    instr.append(settings->value("code").toUInt());
                }

                settings->endArray();
            }
            //добавить элемент на сцену с координатами x,y и параметрами logic_config

            pCell = AddLogicCell(logic_config.num_element,logic_config.logic_func_type,lc_x,lc_y);
            if(pCell)
            {
                pCell->SetNumberLogicCell(logic_config.num_element);
                pCell->SetTypeLogicCell(logic_config.logic_func_type);
                pCell->SetSettingsFlexLC(logic_config);

                if(is_wm)
                {
                    pCell->SetWMInstrBuff(instr);
                }

                lc_array.append(pCell);
            }

            settings->endGroup();
        }

        for(quint16 i=0;i<ports_count;i++)
        {
            settings->beginGroup("FlexPorts/Port" + QString::number(i+1));

            lc_x = settings->value("x").toFloat();
            lc_y = settings->value("y").toFloat();

            port_name = settings->value("name").toString();
            port_src = settings->value("type").toUInt();
            port_num = settings->value("port_num").toUInt();

            TFlexPort* pPort = AddPort(port_src,port_name,lc_x,lc_y);

            if(pPort)
            {
                pPort->SetNumber(port_num);
                ports_array.append(pPort);
            }

            settings->endGroup();
        }

        for(quint16 i=0;i<wire_count;i++)
        {
            settings->beginGroup("FlexConnections/Wire" + QString::number(i+1));

            QPointF point_start = settings->value("start_point").toPointF();
            QPointF point_finish = settings->value("finish_point").toPointF();
            QObject *pStartObj;
            QObject *pFinishObj;
            float wire_center;

            quint16 obj_index = 0;
            quint16 num_input = 0;

            bool inp_port = settings->value("inp_port").toBool();

            if(inp_port)
            {
                obj_index = settings->value("port_num").toUInt();
                pStartObj = ports_array.at(obj_index-1);

            } else
            {
                obj_index = settings->value("inp_cell_num").toUInt();
                pStartObj = lc_array.at(obj_index-1);
            }

            obj_index = settings->value("outp_cell_num").toUInt();
            pFinishObj = lc_array.at(obj_index-1);

            wire_center = settings->value("center_point",(float)((point_finish.x()-point_start.x())/2.0f + point_start.x())).toFloat();

            TFlexConnection *FlexConnection = new TFlexConnection(this,point_start,point_finish,pStartObj,pFinishObj);

            if(FlexConnection)
            {
                m_scene->addItem(FlexConnection);

                FlexConnection->SetCenterPoint(wire_center);

                quint16 num_element;

                if(inp_port)
                {
                    TFlexPort *sPort  = dynamic_cast <TFlexPort *>( pStartObj );

                    sPort->SetLogicCellWirePtr(0,FlexConnection);
                    num_element = sPort->GetOUTSRCCell();

                } else
                {
                    TLogicCell *sCell  = dynamic_cast <TLogicCell *>( pStartObj );

                    sCell->SetLogicCellWirePtr(0,FlexConnection);
                    num_element = sCell->GetOUTSRCCell();
                }

                TLogicCell *outpCell = dynamic_cast <TLogicCell *>( pFinishObj);
                num_input = settings->value("cell_inp_num").toUInt();
                FlexConnection->SetNumInpLC(num_input & 0x0F);
                outpCell->SetLogicCellWirePtr(num_input+1,FlexConnection);
                outpCell->SetInSRCCell(num_input,num_element);
            }

            settings->endGroup();
        }

    }
}

void MainWindow::on_LCPram4spinBox_valueChanged(int arg1)
{
    UpdateParamLCFromCheckBox();
    Q_UNUSED(arg1);
}

void MainWindow::on_LCPram3spinBox_valueChanged(int arg1)
{
    UpdateParamLCFromCheckBox();
    Q_UNUSED(arg1);
}

void MainWindow::on_LCPram2spinBox_valueChanged(int arg1)
{
    UpdateParamLCFromCheckBox();
    Q_UNUSED(arg1);
}


void MainWindow::on_LCPram1spinBox_valueChanged(int arg1)
{
    UpdateParamLCFromCheckBox();
    Q_UNUSED(arg1);
}


void MainWindow::on_SaveprojpushButton_clicked()
{
    SaveFlexProject();
}

void MainWindow::on_SaveAspushButton_clicked()
{
    QString filename= QFileDialog::getSaveFileName(this, tr("Сохранить файл конфигурации как ..."), "Settings", tr("Файл конфигурациии (*.ini)") );

    if(!filename.isNull())
    {
        m_conf_file = filename;
        SaveFlexProject();
    }

}


void MainWindow::on_OpenProjpushButton_clicked()
{

    QString filename= QFileDialog::getOpenFileName(this, tr("Открыть файл конфигурации"), "Settings",tr("Файл конфигурациии (*.ini)") );

    if(!filename.isNull())
    {
        m_conf_file = filename;
        OpenFlexProject();
    }
}

// анализ схемы методом обратного распространения сигнала (аналог бинарного поиска)
void  MainWindow::FillProgInstructionsByStack(void)
{
    QList<TLogicCell *> prog_outs;
    QList <QGraphicsItem*>objl;
    struct _flex_logic_config StackConfig;
    QStack <TLogicCell * > prog_stack;

    objl=m_scene->items();

    for(int i=0; i<objl.size(); i++)
    {
        TLogicCell * cell = dynamic_cast <TLogicCell *> (objl[i]);
        if ((cell) && (cell->GetTypeLogicCell()>_LOGIC_FLEX_VM)) //получаем сначала все выходы
        {
            bool need_add = false;

            struct _flex_logic_config config = cell->GetSettingsFlexLC(); //небольшая оптимизация по количеству входов в выходном элементе

            for(quint8 j=0;j<config.count_in;j++)
            {
                need_add |= (bool)(cell->GetInWirePtr(j) != nullptr);
            }

            if(need_add)
            {
                prog_outs.append(cell); //добавляем все выходы, к которым что-то подключено
            }
        }
    }

    //сортируем (пузырьковая сортировка) выходы по возрастанию координаты y (приоритет сверху вниз)
    for(int i=0; i<prog_outs.count(); i++)
    {
        for(int j=0; j<prog_outs.count()-1; j++)
        {
            qreal py1;
            qreal py2;
            TLogicCell * pcell;

            py1 = prog_outs.at(j)->y();
            py2 = prog_outs.at(j+1)->y();

            if(py2>py1)
            {
                pcell = prog_outs.at(j);
                prog_outs[j] = prog_outs[j+1];
                prog_outs[j+1] = pcell;
            }
        }
    }
    //------------------------------------------

    prog_stack.clear();
    m_prog_elements.clear();

    for(int i=0; i<prog_outs.count(); i++)
    {
        GetPrevSRC(prog_outs.at(i),&prog_stack); //перебрать все элементы цепи рекурсивной функцией
    }

    ui->LEtableWidget->setRowCount(0);
    m_MCFlexLogicConfig.clear();
    m_MCFlexWMCode.clear();
    QStringList Cellslist;
    Cellslist.clear();

    quint16 stack_size = prog_stack.count();

    m_le_ptrs.clear();

    for(int i=0; i<stack_size; i++)
    {
        TLogicCell *pLC = prog_stack.pop();

        m_le_ptrs.append(pLC);

        StackConfig = pLC->GetSettingsFlexLC();

        //предотвращаем повторное добавление элемента в очередь
        quint16 queue_size = m_prog_elements.size();

        quint16 lc_num = StackConfig.num_element;
        bool no_add = false;

        for(quint16 i = 0; i<queue_size;i++ )
        {
            no_add |= (bool)(lc_num == m_prog_elements.at(i));
        }

        if(no_add)
        {
            continue;
        }

        m_prog_elements.append(lc_num);
        m_MCFlexLogicConfig.append(StackConfig);

        if(StackConfig.logic_func_type==_LOGIC_FLEX_VM)
        {
            m_MCFlexWMCode.append(pLC->GetWMInstrBuff());
        }

        //=====================================================

        //======================Вывод результата===============================

        quint16 num_row = ui->LEtableWidget->rowCount();
        ui->LEtableWidget->insertRow(num_row);

        if(StackConfig.logic_func_type>_LOGIC_FLEX_VM)
        {
            ui->LEtableWidget->setItem(num_row, 0, new QTableWidgetItem("S" + QString::number(StackConfig.num_element)));
            Cellslist.append("S" + QString::number(pLC->GetNumberLogicCell()));
        } else
        {
            ui->LEtableWidget->setItem(num_row, 0, new QTableWidgetItem("D" + QString::number(StackConfig.num_element)));
            Cellslist.append("D" + QString::number(pLC->GetNumberLogicCell()));
        }

        ui->LEtableWidget->setItem(num_row, 1, new QTableWidgetItem(logic_names[StackConfig.logic_func_type]));
        ui->LEtableWidget->setItem(num_row, 2, new QTableWidgetItem(QString::number(StackConfig.count_in)));
        for(quint8 i=0;i<5;i++)
        {
            quint16 ns = StackConfig.num_global_signals[i];
            ui->LEtableWidget->setItem(num_row, 3+i, new QTableWidgetItem(m_input_sign_names.at(ns)));
        }

    }

    ui->FlexSeqSizespinBox->setValue(Cellslist.length());

}

TLogicCell* MainWindow::GetPrevSRC(QObject *src_elem, QStack <TLogicCell * > *prog_stack )
{
    TFlexConnection *in_wire[8] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    struct _flex_logic_config config;

    TLogicCell * src_cell = dynamic_cast <TLogicCell *> (src_elem);

    if(src_cell)
    {
        config = src_cell->GetSettingsFlexLC();

        prog_stack->push(src_cell);

        for(quint8 j=0;j<config.count_in;j++)
        {
            in_wire[j]  = src_cell->GetInWirePtr(j);
            if(in_wire[j]!=nullptr)
            {
                 GetPrevSRC(in_wire[j]->GetStartObj(),prog_stack);
            }
        }
    }
    return src_cell;
}

void MainWindow::on_pushButton_34_clicked()
{
    FillProgInstructionsByStack();
}
