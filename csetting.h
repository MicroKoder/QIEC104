#ifndef CSETTING_H
#define CSETTING_H
#include <QString>
///
/// \brief class for connection settins used by iec104driver.h
///
class CSetting
{
public:
    QString IP;

    ///
    /// \brief Port
    /// default is 2404
    quint16 Port;

    ///
    /// \brief t0
    /// time in seconds
 //   quint16 t0;
    ///
    /// \brief t1
    /// таймаут контроля связи в сек
    quint16 t1;
    ///
    /// \brief t2
    ///time in seconds
//    quint16 t2;
    ///
    /// \brief t3
    /// таймаут отправки тестовых пакетов в сек
    quint16 t3;
    ///
    /// \brief k
    /// count
    quint16 k;
    ///
    /// \brief w
    /// count
    quint16 w;
    quint16 asdu;



    bool autoCreateTags;
    bool SendGIOnStart;
    bool SendTCOnStart;
    CSetting();
    CSetting(QString _ip, quint16 _asdu=1, quint16 _port=2404, bool autoCreate=false);
};

#endif // CSETTING_H
