#ifndef CP56TIME_H
#define CP56TIME_H
#include <QByteArray>
#include <QString>
class CP56Time{
  public:
    CP56Time();
    CP56Time(QByteArray data,int s);
    quint16 GetMS();
    char GetMinute();
    char GetHour();
    char GetDay();
    char GetMonth();
    char GetYear();
    QString GetTimeString();
    bool IsValid();
private:
    char rawData[7];
};

#endif // CP56TIME_H
