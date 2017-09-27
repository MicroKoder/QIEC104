#include "ctools.h"

CTools::CTools()
{

}

QString CTools::BytesToString(QByteArray *bytes)
{
    QString str="[";
    foreach(unsigned char c, (*bytes))
    {
        if (c<16)
            str += '0';

        str += QString::number(c,16)+" ";
    }
    str += "]";
    return str;
}

QString CTools::BytesToString(char *bytes, quint16 len)
{
    QByteArray b(bytes,len);
    return BytesToString(&b);
}
