#include "cp56time.h"
#include <QDateTime>
CP56Time::CP56Time()
{
    for (int i=0; i<7; i++)
        rawData[i]=0;
}


bool CP56Time::IsValid(){
    if ((GetMinute()&128) == 0)
        return true;
    else return false;
}

CP56Time::CP56Time(QByteArray data, int offset=0){
    for(int i=0; i<7; i++){
        rawData[i] = data[i+offset];
    }
}
quint16 CP56Time::GetMS(){
    return (quint16(rawData[1])<<8)+rawData[0];

}

char CP56Time::GetHour(){
    return rawData[3]&31;
}

char CP56Time::GetMinute(){
    return rawData[2]&63;
}

char CP56Time::GetDay(){
    return rawData[4]&31;
}

char CP56Time::GetMonth(){
    return rawData[5]&15;
}
char CP56Time::GetYear(){
    return rawData[6]&127;
}

QString CP56Time::GetTimeString()
{
    return QString::number(GetDay()) + "-"+
            QString::number(GetMonth()) + "-" +
            QString::number(GetYear()) + " " +
            QString::number(GetHour())+":"+
            QString::number(GetMinute())+":"+
            QString::number((float)GetMS()/1000.f);
}

QByteArray CP56Time::GetTimestamp()
{
    QDateTime currentDT =  QDateTime::currentDateTime();
    char data[7];

    data[6] = currentDT.date().year()-2000;
    data[5] = currentDT.date().month();
    data[4] = currentDT.date().day();
    data[3] = currentDT.time().hour();
    data[2] = currentDT.time().minute();

    int msec = currentDT.time().msec() + currentDT.time().second()*1000;
    data[1] = msec >> 8;
    data[0] = msec & 0xff;

    return QByteArray(data,7);
}
