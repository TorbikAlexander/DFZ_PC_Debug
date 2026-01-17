#include "tmodbusdfz.h"
#include "qdebug.h"
#include "qmodbusdataunit.h"
#include "qmodbustcpclient.h"
#include <QtCore>
#include <QObject>
#include <QSemaphore>


TModbusDFZ::TModbusDFZ(QObject *parent)
    : QObject{parent}
{
    m_parent = parent;
    m_state = MODBUS_STATE_IDLE;

     connect(this,SIGNAL(signalProcessMessage()),this,SLOT(slotProcessExchange()));
     connect(this,SIGNAL(signalRcvData(QList<quint16>)),parent,SLOT(slotProcessRcvDFZRegData(QList<quint16>)));

     m_modbus_client = new QModbusTcpClient(this);
     connect(m_modbus_client, &QModbusClient::stateChanged,this, &TModbusDFZ::onClientStateChanged);

}

void TModbusDFZ::SetServerIP(QString ip)
{
    m_server_ip = ip;
}

void TModbusDFZ::SendMessage(QModbusDataUnit mess)
{

    m_QueueMutex.lock();
    if(m_MessageQueue.length()<64)  { m_MessageQueue.enqueue(mess); };
    m_QueueMutex.unlock();

    if(m_state==MODBUS_STATE_IDLE) emit signalProcessMessage();
}


void TModbusDFZ::onClientStateChanged(int state)
{
    if (state == QModbusDevice::ConnectedState) {
        //qDebug() << "modbus connected";
        m_event_loop.quit();
    }
}


void TModbusDFZ::slotProcessExchange()
{
    QList<quint16> rcv_data;
    m_Mutex.lock();
    m_state = MODBUS_STATE_RUN;
    m_Mutex.unlock();

    m_modbus_client->setConnectionParameter(QModbusDevice::NetworkAddressParameter,m_server_ip);//
    m_modbus_client->setConnectionParameter(QModbusDevice::NetworkPortParameter,502);
    m_modbus_client->setNumberOfRetries(1);
    m_modbus_client->setTimeout(500);
    m_modbus_client->connectDevice();


    QTimer timer;
    timer.setInterval(2000); // 2 сек
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &m_event_loop, SLOT(quit()));
    timer.start();
    m_event_loop.exec();

    if(m_modbus_client->state()==QModbusDevice::ConnectedState)
    {
        while(!m_MessageQueue.isEmpty())
        {
            m_QueueMutex.lock();
            m_modbus_unit = m_MessageQueue.dequeue();
            m_QueueMutex.unlock();

            QModbusReply *reply = m_modbus_client->sendReadRequest(m_modbus_unit, 1);

            QEventLoop eventLoop;
            connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));

            eventLoop.exec();

            QModbusDataUnit repl_unit = reply->result();
            rcv_data.clear();
            for (uint i = 0; i < repl_unit.valueCount(); i++)
            {
                rcv_data.append(repl_unit.value(i));
            }

            emit signalRcvData(rcv_data);

            // qDebug() << repl_unit.valueCount();

            // for (uint i = 0; i < repl_unit.valueCount(); i++) {
            //     qDebug() << "Адрес" << repl_unit.startAddress() + i
            //              << "значение" << repl_unit.value(i);
            // }

        }

        m_modbus_client->disconnectDevice();
    } else
    {
     qDebug() << "modbus disconnected";
    }
    m_Mutex.lock();
    m_state = MODBUS_STATE_IDLE;
    m_Mutex.unlock();
}
