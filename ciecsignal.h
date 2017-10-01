#ifndef CIECSIGNAL_H
#define CIECSIGNAL_H
#include <QVariant>
#include <QString>
#include <cp56time.h>

class CIECSignal
{
public:
    CIECSignal();

    int address;    // адрес МЭК-104
    QVariant value; //значение
    unsigned long quality;  //качество
    unsigned int typeID;      //идентификатор

    CP56Time timestamp;
   static QList<CIECSignal> ParseData(QByteArray &data, uint *APCInum);
   static QByteArray lostBytes;
};

bool IsStartDTCon(QByteArray &data);
bool IsTestFRCon(QByteArray &data);

#endif // CIECSIGNAL_H
