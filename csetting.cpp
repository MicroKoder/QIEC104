#include "csetting.h"

CSetting::CSetting()
{
    IP = QString("127.0.0.1");
    Port = 2404;

    t1 = 15;
    t3 = 5;

    k = 12;
    w = 8;

    asdu = 1;
    autoCreateTags = false;
}

CSetting::CSetting(QString _ip, quint16 _asdu, quint16 _port, bool autoCreate, bool _autoStart, bool _autoReconnect)
{
    IP = _ip;
    Port = _port;
    asdu = _asdu;
    autoStart = _autoStart;
    autoReconnect= _autoReconnect;

    t1 = 15;
    t3 = 5;

    k = 12;
    w = 8;

    autoCreateTags = autoCreate;
}
