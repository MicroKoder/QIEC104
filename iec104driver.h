#ifndef IEC104DRIVER_H
#define IEC104DRIVER_H
#include <QObject>
#include <QTcpSocket>
#include <QTImer>
#include <QDataStream>
#include <csetting.h>

namespace IEC104
{
    class IEC104Driver;
}
class IEC104Driver:public QObject
{
    Q_OBJECT
private:
    QTimer *testTimer;  //таймер отправки тестового пакета
    QTcpSocket *sock;
    uint count;
    uint lastAPCICount;
    uint N_R;

    void SendFullRequest();
    void SendRequestCounter();
    void SendRequestSingle();
    void SendSyncTime();
    void SendTestAct();
    void SendTestCon();
    void Send_ConfirmPacks();

    static IEC104Driver *hinstance;
    void SendStart();
    bool isStartAct(QByteArray data);
    bool isStartCon(QByteArray data);
    bool isTestAct(QByteArray data);
    bool isTestCon(QByteArray data);
    QByteArray buf;

public:
    quint16 t0;
    quint16 t1;
    quint16 t2;
    quint16 t3; //test timeout
    uint w;
    uint k;
    static IEC104Driver* GetInstance();

    IEC104Driver();
    IEC104Driver(IEC104Driver *other);
    void OpenConnection(CSetting *settings);
    void CloseConnection();
signals:
    void Connected();
    void Disconnected();
    void Message(QString);
public slots:

    void OnConnected();
    void OnDisconnected();
    void OnSockReadyRead(int);
    void displayError(QAbstractSocket::SocketError);
    void OnTestTimer();

};

#endif // IEC104DRIVER_H
