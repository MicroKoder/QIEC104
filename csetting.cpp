#include "csetting.h"

CSetting::CSetting()
{
    IP = QString("127.0.0.1");
    Port = 2404;

    t0 = 10;
    t1 = 15;
    t2 = 10;
    t3 = 5;

    k = 12;
    w = 8;

    asdu = 1;
}

CSetting::CSetting(QString _ip, quint8 _asdu=1, quint16 _port=2404)
{
    IP = _ip;
    Port = _port;
    asdu = _asdu;

    t0 = 10;
    t1 = 15;
    t2 = 10;
    t3 = 5;

    k = 12;
    w = 8;
}
