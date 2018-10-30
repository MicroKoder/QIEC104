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

///Класс - синглтон
class IEC104Driver:public QObject
{
    Q_OBJECT
private:
    QTimer *testTimer;  //таймер отправки тестового пакета
    QTcpSocket *sock;
    uint count;
    uint lastAPCICount;
    quint16 N_R;    //счетчик вх. пакетов
    quint16 N_T;    //счетчик исх. пактов
    //QByteArray buf;
    CSetting* settings;

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
    void SendFullRequest(quint16 ASDU, quint8 requestDescription); //general interrogation [100]
    static IEC104Driver* GetInstance();
    void SetSettings(CSetting* settings);
    void SetSettings(QSettings* settings);
    CSetting* GetSettings();
    void CloseConnection();
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
