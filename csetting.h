#ifndef CSETTING_H
#define CSETTING_H
#include <QString>
///
/// \brief Class for IEC104 driver settings
///
class CSetting
{
public:
    QString IP;
    quint16 Port;
    quint16 t0;
    quint16 t1;
    quint16 t2;
    quint16 t3;
    quint16 k;
    quint16 w;
    quint8 asdu;
    CSetting();
    CSetting(QString _ip, quint8 _asdu, quint16 _port);
};

#endif // CSETTING_H
