#ifndef TMODBUSDFZ_H
#define TMODBUSDFZ_H

#include "qeventloop.h"
#include "qmodbusdataunit.h"
#include "qmodbustcpclient.h"
#include "qmutex.h"
#include "qsemaphore.h"
#include <QObject>
#include <QQueue>

enum _modbus_thread_states
{
    MODBUS_STATE_IDLE,
    MODBUS_STATE_RUN
};

class TModbusDFZ : public QObject
{
    Q_OBJECT
public:
    explicit TModbusDFZ(QObject *parent = nullptr);

    void SetServerIP(QString ip);
    void SendMessage(QModbusDataUnit mess);

private:
    QObject *m_parent;
    QString m_server_ip = "192.168.7.200"; //"127.0.0.1";
    quint8 m_state=MODBUS_STATE_IDLE;
    QMutex m_QueueMutex;
    QMutex m_Mutex;
    QQueue<QModbusDataUnit> m_MessageQueue;
    QModbusTcpClient *m_modbus_client;
    QModbusDataUnit m_modbus_unit;
    QEventLoop m_event_loop;

signals:

    void signalProcessMessage(void);
    void signalRcvData(QList<quint16>);

protected slots:
    void onClientStateChanged(int state);
private slots:

    void slotProcessExchange(void);

};

#endif // TMODBUSDFZ_H
