#ifndef IEC104DRIVER_H
#define IEC104DRIVER_H
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <csetting.h>
#include <QSettings>
#include <ctools.h>
#include <QTimer>

namespace IEC104
{
    class IEC104Driver;
}

///
/// \brief The IEC104Driver class
/// This singleton class takes over iec104 protocol interaction.
///  Use slot "OpenConnection" to start. Driver automatically maintain connection by sending test packages.
///  When the signal received the IECSignalReceived signal will be activated.
///  For sending commands use methods SendCommand or SetPoint.
///
class IEC104Driver:public QObject
{
    Q_OBJECT
private:
    QTimer *testTimer=nullptr;  // t3: test / confirm idle timer
    QTimer *conTimer=nullptr;   // t1: connection / ack timeout
    QTimer *t2Timer=nullptr;    // t2: delayed S-format confirm
    QTcpSocket *sock=nullptr;
    QByteArray rxBuffer;        // TCP stream reassembly

    /// Last N(R) we already confirmed to the peer (next-expected sequence)
    quint16 lastConfirmed = 0;

    /// Last N(R) received from peer (how far our sends are acknowledged)
    quint16 peerAcked = 0;

    ///
    /// \brief N_R
    /// Next expected receive sequence number (IEC N(R))
    quint16 N_R;

    ///
    /// \brief N_T
    /// Next send sequence number (IEC N(S) to use on next I-format)
    quint16 N_T;

    bool needSendGI = false;
    bool needSendTC = false;

    ///
    /// \brief settings
    ///connection settings
    CSetting* settings=nullptr;

    IEC104Driver();
    IEC104Driver(IEC104Driver *other);
    static IEC104Driver *instance;


    void SendRequestCounter();  //101
    void SendRequestSingle();   //
    void SendTestAct();
    void SendTestCon();
    void Send_ConfirmPacks();

    void SendStart();
    bool isStartAct(QByteArray data);
    bool isStartCon(QByteArray data);
    bool isTestAct(QByteArray data);
    bool isTestCon(QByteArray data);

    bool CanSendIFormat() const;
    bool SendIFormat(const QByteArray &buf, const QString &logMessage);
    void UpdatePeerAck(quint16 nr);
    void ProcessAPDU(const QByteArray &apdu);
    void ScheduleConfirm();
    static quint16 SeqDiff(quint16 a, quint16 b);

public:
    // команда общего опроса, requestDescription - описатель общего опроса
    void SendFullRequest(quint8 requestDescription);
    static IEC104Driver* GetInstance();
    void SetSettings(QSettings* settings);
    CSetting* GetSettings();
    void CloseConnection();

    //отправить команду (одно- или двухпозиционную)
    void SendCommand(quint16 type, quint32 ioa, quint8 value);
    //отправить уставку (INT16, UINT16, FLOAT)
    void SetPoint(quint16 type, quint32 ioa, QVariant value);

signals:
    //попытка установки соединения
    void Connecting();
    //соединение установлено
    void Connected();
    //соединение разорвано
    void Disconnected();

    ///вывод сообщения в лог
    void Message(QString);

    /// декодированный сигнал
    void IECSignalReceived(CIECSignal);
private slots:
    void OnConnected();
    void OnDisconnected();
    void OnSockReadyRead();
    void displayError(QAbstractSocket::SocketError);
    void OnTestTimer();
    void OnConnectionTimer();
    void OnT2Timer();
public slots:

    //подключиться
    void OpenConnection(/*CSetting *_settings=NULL*/);

    // отправить команду синхронизации времени (102)
    void ClockSynch();

    //отправить команду чтения
    void ReadIOA(quint32 ioa);

};

#endif // IEC104DRIVER_H
