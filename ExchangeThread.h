#ifndef EXCHANGETHREAD_H
#define EXCHANGETHREAD_H

#include <QThread>
#include <QString>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>
#include <QTimer>

enum _thread_states
{
    STATE_IDLE,
    STATE_RUN
};

#define PC_TCP_PORT     8888
#define PC_TIMEOUT_MS   200

class ExchangeThread : public QObject
{
    Q_OBJECT

public:
    explicit ExchangeThread(QObject *parent = nullptr);
    virtual ~ExchangeThread();

    void SendMessage(QByteArray mess);
    void Set_IP(QString  ip);
    void Set_PORT(quint16 port);
    void Set_Timeout(quint32 timeout_ms);

    void SetCLRQueue(void);
    quint32 GetUsedQueueSize();

public slots:
    void slotFinishedThread(void);


signals:
    void signalSendDataReady(quint8,QByteArray);
    void signalSendDataError(quint8,QByteArray);
    void signalProcessMessage(void);

    void signalFillIZMTable(QByteArray); //
    void signalFillStateMDFZTable(QByteArray);
    void signalFillHWSettingsTable(QByteArray);
    void signalConnectState(bool);
    void signalFillHWDate(QByteArray);
    void signalFillHWTime(QByteArray);
    void signalFillLOG(QByteArray);
    void signalFillFW(QByteArray);
    void signalFillLEDS(QByteArray);
    void signalFillLogCount(QByteArray);
    void signalFillGetOsc(QByteArray);
    void signalFillGetOscAK(QByteArray);
    void signalFillGetHFPhase(QByteArray);
    void signalFillGetHFSpectr(QByteArray);

    void signalFillMUHelth(QByteArray);
    void signalFillModuleStateEx(QByteArray);

    void signalFillString(QByteArray);
    void signalFillCompState(QByteArray);
    void signalFillGooseState(QByteArray);
    void signalFillVADCParam(QByteArray);
    void signalFillRcvLEState(QByteArray);

private:

    QObject *m_parent;
    QQueue<QByteArray> m_MessageQueue;
    QTcpSocket  m_socket;
    QString m_ip="192.168.3.33";
    quint16 m_port=PC_TCP_PORT;
    quint32 m_timeout=PC_TIMEOUT_MS;
    quint8 m_state=STATE_IDLE;
    QMutex m_QueueMutex;
    QMutex m_Mutex;
    QTimer m_disconnect_timer;


private slots:
    void slotProcessPCExchange(void);
    void DisconnectTimerProcedure(void);

};




#endif // EXCHANGETHREAD_H
