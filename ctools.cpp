#include "ctools.h"

IEC104Tools::IEC104Tools()
{

}


QByteArray IEC104Tools::lostBytes=QByteArray();

QString IEC104Tools::BytesToString(QByteArray *bytes)
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

QString IEC104Tools::BytesToString(char *bytes, quint16 len)
{
    QByteArray b(bytes,len);
    return BytesToString(&b);
}

uint IEC104Tools::ParseAPCInum(QByteArray &data){
   return ((unsigned char)data[2] + (((unsigned char)data[3])<<8));
}

///чтение одного сигнала из пакета байт
CIECSignal IEC104Tools::ParseSignal(QByteArray &data, uchar typeID, int n){
    CIECSignal iecSignal;

    iecSignal.SetAddress( data[n]+((data[n+1]<<8)&0xff00));
    iecSignal.SetType(typeID);

    switch (typeID){

      case 30:{  iecSignal.value = QVariant(((uchar)data[n+3])&0x01);
                  iecSignal.timestamp = CP56Time(data,n+4);
                   iecSignal.quality = data[n+3]&0xFFFFFFFE;
              }
              break;
        //DWORD
      case 33:{
                  uint val = ((uchar)data[n+3] +((uchar)data[n+4]<<8) +((uchar)data[n+5]<<16) +((uchar)data[n+6]<<24));
                  iecSignal.value = QVariant(val);
                  iecSignal.quality = data[n+7];
                   iecSignal.timestamp = CP56Time(data,n+8);
              }
               break;
        //FLOAT
      case 36:{
                  uint *bytes = new uint();
                         (*bytes)= (uchar)data[n+3] +((uchar)data[n+4]<<8) +((uchar)data[n+5]<<16) +((uchar)data[n+6]<<24);
                  float *val = reinterpret_cast<float*>(bytes);
                  iecSignal.value = QVariant(*val);
                  iecSignal.quality = data[n+7];
                   iecSignal.timestamp = CP56Time(data,n+8);
                   delete bytes;
              }
               break;
        //UINT
      case 34:{
                  uint val = ((uchar)data[n+3] +(((uchar)data[n+4])<<8));
                  iecSignal.value = QVariant(val);
                  iecSignal.timestamp = CP56Time(data,n+6);
                  iecSignal.quality = data[n+5];
               }
              break;
        //INT
      case 35:{
                  qint16 val = ((uchar)data[n+3] +(((uchar)data[n+4])<<8));
                  iecSignal.value = QVariant(val);
                  iecSignal.timestamp = CP56Time(data,n+6);
                  iecSignal.quality = data[n+5];
               }
              break;
    }

    return iecSignal;
}
///Обработка целого фрейма, должен быть валидный пакет данных
QList<CIECSignal>* IEC104Tools::ParseFrame(QByteArray &data, quint16 *APCInum){
    QList<CIECSignal>* result=new QList<CIECSignal>();



    (*APCInum) = IEC104Tools::ParseAPCInum(data);
    uchar typeID = uchar(data[6]);


    int stride = 0;
    switch (typeID){
    case 30: stride = 11; break;    //
    case 33: stride = 15; break;    //dword
    case 36: stride = 15; break;    //float
    case 34: stride = 13; break;    //uint
    case 35: stride = 13; break;    //int
    }

    //uchar count = uchar(data[7]);
    uchar count = (data.length() - 12) / stride;

    for (int i=0; i< count; i++)
        result->append(ParseSignal(data,typeID,(i*stride)+12));
    return result;
}

///Обработка пакета байт
QList<CIECSignal>* IEC104Tools::ParseData(QByteArray &data, quint16 *APCInum){

    QList<CIECSignal>* result = new QList<CIECSignal>();
    QByteArray d;

    if (lostBytes.length()>0){
        data.insert(0,lostBytes);
        lostBytes.clear();
    }

    //длина APCI = APDU + 2
    uint APCILength = (uchar)data[1]+2;

    //размер принятого массива меньше APCI, сохраняем этот кусок до следующего вызова функции
    if (APCILength>data.length())
    {
        lostBytes.append(data);
        return result;
    }else
    {
        QList<CIECSignal>* temp = ParseFrame(data,APCInum);
        result->append(*temp);
    }
  //размер принятого массива больше APCI, рекурсивный вызов функции для оставшегося куска
  if (APCILength<data.length()){

        for (int i=(uchar)data[1]+2; i<data.length(); i++)
            d.append(data[i]);

        QList<CIECSignal>* temp = ParseData(d,APCInum);

        result->append(*temp);
    }
    return result;
}
