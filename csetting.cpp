/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "csetting.h"

CSetting::CSetting()
{
    IP = QString("127.0.0.1");
    Port = 2404;

  //  t0 = 10;
    t1 = 15;
 //   t2 = 10;
    t3 = 5;

    k = 12;
    w = 8;

    asdu = 1;
    autoCreateTags = false;
}

CSetting::CSetting(QString _ip, quint16 _asdu, quint16 _port, bool autoCreate, bool _autoStart)
{
    IP = _ip;
    Port = _port;
    asdu = _asdu;
    autoStart = _autoStart;

 //   t0 = 10;
    t1 = 15;
  //  t2 = 10;
    t3 = 5;

    k = 12;
    w = 8;

    autoCreateTags = autoCreate;
}
