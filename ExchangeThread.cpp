#include "ExchangeThread.h"
#include "ExchangeParams.h"
//#include "crc16.h"

#include <QTimer>
#include <QHostAddress>
#include <QEventLoop>

ExchangeThread::ExchangeThread(QObject *parent) : m_socket(this)
{
    m_parent = parent;

    m_state = STATE_IDLE;

    connect(parent, SIGNAL(signalFinishedAll()), this, SLOT(slotFinishedThread()));

    connect(this,SIGNAL(signalProcessMessage()),this,SLOT(slotProcessPCExchange()));

    connect(this, SIGNAL(signalFillIZMTable(QByteArray)), m_parent, SLOT(slotFillIZMTable(QByteArray)));
    connect(this, SIGNAL(signalFillStateMDFZTable(QByteArray)), m_parent, SLOT(slotFillStateMDFZTable(QByteArray)));
    connect(this, SIGNAL(signalFillHWSettingsTable(QByteArray)), m_parent, SLOT(slotFillHWSettingsTable(QByteArray)));
    connect(this, SIGNAL(signalConnectState(bool)), m_parent, SLOT(slotConnectStateProcedure(bool)));
    connect(this, SIGNAL(signalFillHWDate(QByteArray)), m_parent, SLOT(slotFillHWDate(QByteArray)));
    connect(this, SIGNAL(signalFillHWTime(QByteArray)), m_parent, SLOT(slotFillHWTime(QByteArray)));
    connect(this, SIGNAL(signalFillLOG(QByteArray)), m_parent, SLOT(slotFillHWLOG(QByteArray)));
    connect(this, SIGNAL(signalFillFW(QByteArray)), m_parent, SLOT(slotFillHWFW(QByteArray)));
    connect(this, SIGNAL(signalFillLEDS(QByteArray)), m_parent, SLOT(slotFillLEDS(QByteArray)));
    connect(this, SIGNAL(signalFillLogCount(QByteArray)), m_parent, SLOT(slotFillLogCount(QByteArray)));
    connect(this, SIGNAL(signalFillGetOsc(QByteArray)), m_parent, SLOT(slotFillGetOsc(QByteArray)));
    connect(this, SIGNAL(signalFillGetOscAK(QByteArray)), m_parent, SLOT(slotFillGetOscAK(QByteArray)));

    connect(this, SIGNAL(signalFillGetHFPhase(QByteArray)), m_parent, SLOT(slotFillGetHF(QByteArray)));
    connect(this, SIGNAL(signalFillGetHFSpectr(QByteArray)), m_parent, SLOT(slotFillGetSpectr(QByteArray)));

    connect(this, SIGNAL(signalFillMUHelth(QByteArray)), m_parent, SLOT(slotFillMUHelth(QByteArray)));

    connect(this, SIGNAL(signalFillModuleStateEx(QByteArray)), m_parent, SLOT(slotFillModuleStateEx(QByteArray)));

    connect(this, SIGNAL(signalFillString(QByteArray)), m_parent, SLOT(slotFillString(QByteArray)));
    connect(this, SIGNAL(signalFillGooseState(QByteArray)), m_parent, SLOT(slotFillGooseState(QByteArray)));

    connect(this, SIGNAL(signalFillCompState(QByteArray)), m_parent, SLOT(slotFillCompState(QByteArray)));

    connect(this, SIGNAL(signalFillVADCParam(QByteArray)), m_parent, SLOT(slotFillVADCParam(QByteArray)));

    connect(this, SIGNAL(signalFillRcvLEState(QByteArray)), m_parent, SLOT(slotRcvLEState(QByteArray)));
    //


      // connect(&m_disconnect_timer,SIGNAL(timeout()),this,SLOT(quit()));
   // m_disconnect_timer.stop();

}

ExchangeThread::~ExchangeThread()
{

};

void ExchangeThread::Set_IP(QString  ip)
{
    if(ip!="") m_ip = ip;

}
void ExchangeThread::Set_PORT(quint16 port)
{
    m_port = port;
}

void ExchangeThread::Set_Timeout(quint32 timeout_ms)
{
    m_timeout = timeout_ms;
}


quint32 ExchangeThread::GetUsedQueueSize()
{
    return m_MessageQueue.count();
}
void ExchangeThread::SetCLRQueue(void)
{
    m_MessageQueue.clear();
}

void ExchangeThread::SendMessage(QByteArray mess)
{

    m_QueueMutex.lock();
    if(m_MessageQueue.length()<32767)  { m_MessageQueue.enqueue(mess); };
    m_QueueMutex.unlock();

    if(m_state==STATE_IDLE) emit signalProcessMessage();
}

void ExchangeThread::DisconnectTimerProcedure(void)
{
    m_socket.disconnectFromHost();
  //  m_disconnect_timer.stop();
    emit signalConnectState(false);
}

void ExchangeThread::slotProcessPCExchange(void)
{
    QByteArray mess;
    static quint32 connect_state = QAbstractSocket::UnconnectedState;

    m_Mutex.lock();
    m_state = STATE_RUN;
    m_Mutex.unlock();

        while(!m_MessageQueue.isEmpty())
        {
            mess.clear();
            m_QueueMutex.lock();
            mess = m_MessageQueue.dequeue();
            m_QueueMutex.unlock();

            if( m_socket.state() == QAbstractSocket::UnconnectedState)
            {
                m_socket.connectToHost(QHostAddress(m_ip), m_port);
                m_socket.waitForConnected(150);
            }

            if(connect_state != m_socket.state())
            {
                connect_state = m_socket.state();

                emit signalConnectState((bool)(connect_state & 1));
                qDebug() << m_socket.state();
            }

            if( m_socket.state() == QAbstractSocket::ConnectedState)
            {
                m_socket.write(mess);
                m_socket.waitForBytesWritten(100);

                m_socket.readAll();

                if(m_socket.waitForReadyRead(m_timeout))
                {
                    QByteArray answer;
                    m_socket.waitForReadyRead(20); //для осц 8,2мс реально (wireshark)
                    answer = m_socket.readAll();

                    if(answer.size()>6)
                    {
                        //if(answer.at(1)==_LP_RD_ACK) {qDebug() << "ack";};
                        if(answer.at(1)==_LP_SEND_V4Z_STATE) emit signalFillStateMDFZTable(answer);
                        if(answer.at(1)==_LP_SEND_V4Z_IZM) emit signalFillIZMTable(answer);
                        if(answer.at(1)==_LP_SET_SETTINGS) emit signalFillHWSettingsTable(answer);
                        if(answer.at(1)==_LP_SET_TIME) emit signalFillHWTime(answer);
                        if(answer.at(1)==_LP_SET_DATE) emit signalFillHWDate(answer);
                        if(answer.at(1)==_LP_SEND_LOG) emit signalFillLOG(answer);
                        if(answer.at(1)==_LP_SEND_FW)  emit signalFillFW(answer);
                        if(answer.at(1)==_LP_SEND_LEDSTATE)  emit signalFillLEDS(answer);
                        if(answer.at(1)==_LP_SEND_LOG_COUNT) emit signalFillLogCount(answer);
                        if(answer.at(1)==_LP_SEND_MU_HEALTH) emit signalFillMUHelth(answer);
                        if(answer.at(1)==_LP_SEND_MODULES_EXP_STATE) emit signalFillModuleStateEx(answer);

                        if(answer.at(1)==_LP_SEND_PHASE) {emit signalFillGetHFPhase(answer);};
                        if(answer.at(1)==_LP_SEND_FFT) {emit signalFillGetHFSpectr(answer);};

                        if(answer.at(1)==_LP_SEND_STRING) emit signalFillString(answer);
                        if(answer.at(1)==_LP_SEND_GOOSE_STATE) emit signalFillGooseState(answer);

                        if(answer.at(1)==_LP_SEND_MDFZ_OSC)
                        {
                            emit signalFillGetOsc(answer);
                        }
                        if(answer.at(1)==_LP_SEND_OSC)
                        {
                            emit signalFillGetOscAK(answer);
                        }

                        if(answer.at(1)==_LP_SEND_COMPARATORS_STATE) emit signalFillCompState(answer);
                        if(answer.at(1)==_LP_SEND_VADC_PARAM)   emit signalFillVADCParam(answer);
                        if(answer.at(1)==_LP_SEND_FESTATE)      emit signalFillRcvLEState(answer);

                    }
                }

                if(m_MessageQueue.isEmpty())
                {
                    m_socket.disconnectFromHost();
                    // connect_state = QAbstractSocket::UnconnectedState;
                    // qDebug() << m_socket.state();
                }

            }
        }

    m_Mutex.lock();
    m_state = STATE_IDLE;
    m_Mutex.unlock();
}

void ExchangeThread::slotFinishedThread(void)
{
    QEventLoop loop; //задержка на окончание обмена в потоках
    QTimer timer;
    connect(&timer,SIGNAL(timeout()),&loop,SLOT(quit()));
    timer.start(500);
    loop.exec();
    timer.stop();

    this->deleteLater();
}
