#ifndef CIECSIGNAL_H
#define CIECSIGNAL_H
#include <QVariant>
#include <QString>
#include <cp56time.h>

class CIECSignal
{
    ///уникальный номер сигнала
    uint key;

    ///IOA
    uint16_t address;

    ///идентификатор типа
    uchar typeID;
public:
    CIECSignal();
    CIECSignal(uint16_t addr, uchar type);

    uint GetKey()
    {
        return key;
    }

    void SetAddress(uint16_t ioa)
    {
        address = ioa;
        key &= 0xFF0000;
        key |= ioa;
    }
    void SetType(uchar type)
    {
        typeID = type;
        key &=0x00FFFF;
        key |= type<<16;
    }

    uint GetAddress()
    {
        return address;
    }

    uint GetType()
    {
        return typeID;
    }

    ///значение
    QVariant value;

    ///качество
    unsigned long quality;

    ///метка времени
    CP56Time timestamp;


    QString GetValueString();
};

bool IsStartDTCon(QByteArray &data);
bool IsTestFRCon(QByteArray &data);

#endif // CIECSIGNAL_H
