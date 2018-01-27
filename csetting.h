#ifndef CSETTING_H
#define CSETTING_H
#include <QString>

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
};

#endif // CSETTING_H
