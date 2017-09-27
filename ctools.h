#ifndef CTOOLS_H
#define CTOOLS_H

#include <qstring.h>

class CTools
{
public:
    CTools();
    static QString BytesToString(QByteArray *bytes);
    static QString BytesToString(char *bytes, quint16 len);
};

#endif // CTOOLS_H
