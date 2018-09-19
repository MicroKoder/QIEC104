#include "ciecsignal.h"
#include "ctools.h"
#include <Qdebug>
CIECSignal::CIECSignal()
{

}

CIECSignal::CIECSignal(uint16_t addr, uchar type)
{
    SetAddress(addr);
    SetType(type);
    this->quality = 0;
    //this->descr = "";
}

/*CIECSignal::CIECSignal(uint16_t addr, uchar type, QString description)
{
    SetAddress(addr);
    SetType(type);
    this->descr = description;
}*/

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

