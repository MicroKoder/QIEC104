#ifndef CIECSIGNAL_H
#define CIECSIGNAL_H
#include <QVariant>
#include <QString>
#include "cp56time.h"

class CIECSignal
{
    ///unique key for signal depends of IOA and type
    uint key;

    ///IOA
    quint32 address;
    ///IEC104 type
    uchar typeID;
public:
    CIECSignal();
    CIECSignal(uint16_t addr, uchar type);
 //   CIECSignal(uint16_t addr, uchar type, QString description);
    ///
    /// \brief GetKey
    /// returns unique key that depend of type and IOA
    /// \return
    ///
    uint GetKey()
    {
        return key;
    }
    ///
    /// \brief SetAddress
    /// set IOA address to signal
    /// \param ioa
    ///
    void SetAddress(quint32 ioa)
    {
        address = ioa;
        key &= 0xFF000000;
        key |= ioa;
    }

    ///
    /// \brief SetType
    /// set IEC-104 type to signal
    /// \param type
    ///
    void SetType(uchar type)
    {
        typeID = type;
        key &=0x00FFFF;
        key |= type<<24;
    }
    ///
    /// \brief GetAddress
    /// return IOA address
    /// \return
    ///
    quint32 GetAddress()
    {
        return address;
    }
    ///
    /// \brief GetType
    /// return IEC104 type
    /// \return
    ///
    uint GetType()
    {
        return typeID;
    }

    ///value
    QVariant value;

    ///IEC104 quality
    uchar quality;

    ///description
  //  QString descr;
    ///7-byte time
    CP56Time timestamp;

    uint ASDU;
    ///
    /// \brief GetValueString
    /// \return value as string
    ///
    QString GetValueString();
};

bool IsStartDTCon(QByteArray &data);
bool IsTestFRCon(QByteArray &data);

#endif // CIECSIGNAL_H
