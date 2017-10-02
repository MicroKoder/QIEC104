#ifndef IEC104DRIVER_H
#define IEC104DRIVER_H
#include <QObject>
#include <QTcpSocket>
#include <QTImer>
#include <QDataStream>
#include <csetting.h>
#include <ctools.h>

namespace IEC104
{
    class IEC104Driver;
}

///Класс - синглтон
class IEC104Driver:public QObject
{
    Q_OBJECT
private:
    QTimer *testTimer;  //таймер отправки тестового пакета
    QTcpSocket *sock;
    uint count;
    uint lastAPCICount;
    quint16 N_R;
    QByteArray buf;
    CSetting* settings;

    IEC104Driver();
    IEC104Driver(IEC104Driver *other);
    static IEC104Driver *instance;

    void SendFullRequest();
    void SendRequestCounter();
    void SendRequestSingle();
    void SendSyncTime();
    void SendTestAct();
    void SendTestCon();
    void Send_ConfirmPacks();


    void SendStart();
    bool isStartAct(QByteArray data);
    bool isStartCon(QByteArray data);
    bool isTestAct(QByteArray data);
    bool isTestCon(QByteArray data);

public:
    static IEC104Driver* GetInstance();
    void SetSettings(CSetting* settings);
    CSetting* GetSettings();

    void CloseConnection();
signals:
    void Connected();
    void Disconnected();

    ///вывод сообщения в лог
    void Message(QString);

    /// декодированный сигнал
    void IECSignalReceived(CIECSignal*);
private slots:
    void OnConnected();
    void OnDisconnected();
    void OnSockReadyRead(int);
    void displayError(QAbstractSocket::SocketError);
    void OnTestTimer();
public slots:
    void OpenConnection(CSetting *settings);
    void Interrogation();
    void ClockSynch();

};

#endif // IEC104DRIVER_H
