/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#include "ciecsignal.h"
#include "ctools.h"
#include <QDebug>
CIECSignal::CIECSignal()
{
    description = "new tag";
}

CIECSignal::CIECSignal(uint16_t addr, uchar type, QString descr)
{
    SetAddress(addr);
    SetType(type);
    this->quality = 0;
    this->description = descr;
    //this->descr = "";
}

/*CIECSignal::CIECSignal(uint16_t addr, uchar type, QString description)
{
    SetAddress(addr);
    SetType(type);
    this->descr = description;
}*/

void CIECSignal::SetKey(uint key)
{
    this->key = key;
    address = key & 0x00FFFFFFu;
    typeID = (key &0xFF000000u)>>24;
}

void CIECSignal::SetAddress(quint32 ioa)
{
    address = ioa;
    key &= 0xFF000000;
    key |= ioa;
}

void CIECSignal::SetType(uchar type)
{
    typeID = type;
    key &=0x00FFFFu;
    key |= type<<24;
}

QString CIECSignal::GetValueString()
{
    QString result;
    result = "addr: " + QString::number(this->address) +
            " value: ";

    switch (typeID)
    {
    case 3: result += value.toUInt(); break;
    case 30: result += (value==1) ? "true" : "false"; break;
    case 31: result += value.toUInt(); break;
    case 32: result += QString::number(value.toUInt());break;
    case 33: result += QString::number( value.toUInt());break;
    case 34: result += QString::number(value.toUInt());break;
    case 35: result += QString::number(value.toInt());break;
    case 36: result += QString::number(value.toFloat());break;

    }
    //QString::number(this->value);

    result += " type: " + QString::number(this->typeID)+
            " quality: " + QString::number(this->quality) +
            " Time: " + this->timestamp.GetTimeString();
    return result;
}

