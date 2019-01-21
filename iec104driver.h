#ifndef IEC104DRIVER_H
#define IEC104DRIVER_H
#include <QObject>
#include <QTcpSocket>
#include <QTImer>
#include <QDataStream>
#include <csetting.h>
#include <qsettings>
#include <ctools.h>

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
    QTimer *testTimer=nullptr;  //таймер отправки тестового пакета
    QTcpSocket *sock=nullptr;
    uint count;
    uint lastAPCICount;
    ///
    /// \brief N_R
    /// count of recieved packeges from server
    quint16 N_R;

    ///
    /// \brief N_T
    ///count of thansfered packages to server
    quint16 N_T;

    ///
    /// \brief settings
    ///connection settings
    CSetting* settings=nullptr;

    IEC104Driver();
    IEC104Driver(IEC104Driver *other);
    static IEC104Driver *instance;


    void SendRequestCounter();  //101
    void SendRequestSingle();   //
    void SendSyncTime();        //103
    void SendTestAct();
    void SendTestCon();
    void Send_ConfirmPacks();


    void SendStart();
    bool isStartAct(QByteArray data);
    bool isStartCon(QByteArray data);
    bool isTestAct(QByteArray data);
    bool isTestCon(QByteArray data);

public:
    void SendFullRequest(quint8 requestDescription); //general interrogation [100]
    static IEC104Driver* GetInstance();
    //void SetSettings(CSetting* settings);
    void SetSettings(QSettings* settings);
    CSetting* GetSettings();
    void CloseConnection();

    ///
    /// \brief SendCommand
    /// \param type
    /// \param ioa
    /// \param value
    ///
    void SendCommand(quint16 type, quint32 ioa, quint8 value);
    void SetPoint(quint16 type, quint32 ioa, QVariant value);
signals:
    void Connected();
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
public slots:
    void OpenConnection(CSetting *_settings=NULL);
    void Interrogation();
    void ClockSynch();

};

#endif // IEC104DRIVER_H
