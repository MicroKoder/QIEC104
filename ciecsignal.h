#ifndef CIECSIGNAL_H
#define CIECSIGNAL_H
#include <QVariant>
#include <QString>
#include "cp56time.h"

class CIECSignal
{
    ///unique key for signal depends of IOA and type
    uint key=0;

    ///IOA
    quint32 address=0;
    ///IEC104 type
    uchar typeID=0;
public:
    CIECSignal();
    CIECSignal(uint16_t addr, uchar type, QString descr=QString("new tag"));
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

    void SetKey(uint key);

    ///
    /// \brief SetAddress
    /// set IOA address to signal
    /// \param ioa
    ///
    void SetAddress(quint32 ioa);

    ///
    /// \brief SetType
    /// set IEC-104 type to signal
    /// \param type
    ///
    void SetType(uchar type);
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

    ///описание сигнала
    QString description = QString("");
    ///7-byte time
    CP56Time timestamp;

    uint ASDU;
    ///
    /// \brief GetValueString
    /// \return value as string
    ///
    QString GetValueString();

    ///флаг означает что с момента подключения или добавления в список сигнал еще ни разу не обновлялся
    bool bNeverUpdated = true;

};

bool IsStartDTCon(QByteArray &data);
bool IsTestFRCon(QByteArray &data);

#endif // CIECSIGNAL_H
