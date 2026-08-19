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
    // Keep 24-bit IOA; previous mask 0x00FFFF truncated high address byte
    key &= 0x00FFFFFFu;
    key |= (uint(type) << 24);
}

QString CIECSignal::GetValueString()
{
    QString result;
    result = "addr: " + QString::number(this->address) +
            " value: ";

    switch (typeID)
    {
    case 1:
    case 30:
        result += (value.toUInt() == 1) ? "true" : "false";
        break;
    case 3:
    case 31:
        result += QString::number(value.toUInt());
        break;
    case 5:
    case 32:
        result += QString::number(value.toUInt());
        break;
    case 7:
    case 33:
        result += QString::number(value.toUInt());
        break;
    case 9:
    case 34:
        result += QString::number(value.toUInt());
        break;
    case 11:
    case 35:
        result += QString::number(value.toInt());
        break;
    case 13:
    case 36:
        result += QString::number(value.toFloat());
        break;
    default:
        result += value.toString();
        break;
    }

    result += " type: " + QString::number(this->typeID)+
            " quality: " + QString::number(this->quality) +
            " Time: " + this->timestamp.GetTimeString();
    return result;
}

