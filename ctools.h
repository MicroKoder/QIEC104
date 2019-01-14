#ifndef CTOOLS_H
#define CTOOLS_H

#include <qstring.h>
#include "ciecsignal.h"
#include "cp56time.h"

///
/// \brief The IEC104Tools class
/// static functions used by iec104 driver for decoding packages
class IEC104Tools
{
    static QByteArray lostBytes;
    //декодирование одного сигнала из пакета данных
    static CIECSignal ParseSignal(QByteArray &data, uchar typeID, int n);
    //декодирование фрейма APCI
    static QList<CIECSignal> ParseFrame(QByteArray &data, quint16 *APCInum);
    /// выделение счетчика пакетов
   // static uint ParseAPCInum(QByteArray &data);
public:
    IEC104Tools();

    ///функции для отображения последовательности байт в виде строки
    static QString BytesToString(QByteArray *bytes);
    static QString BytesToString(char *bytes, quint16 len);

    ///парсинг пакета данных
    /// data может быть целым фреймом APCI, началом фрейма APCI, либо целым фреймом APCI с началом следующего фрейма
    /// гарантируется что любая последовательность байт будет обработана
    static QList<CIECSignal>* ParseData(QByteArray &data, quint16 *APCInum);
};

#endif // CTOOLS_H
