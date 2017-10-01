#include "ciecsignal.h"
#include "ctools.h"
#include <Qdebug>
CIECSignal::CIECSignal()
{

}

CIECSignal ParseSignal(QByteArray &data, uchar typeID, int n){
    CIECSignal iecSignal;

    iecSignal.address = data[n]+((data[n+1]<<8)&0xff00);
    iecSignal.typeID = typeID;
    switch (typeID){

      case 30:{  iecSignal.value = QVariant(((uchar)data[n+3])&0x01);
                  iecSignal.timestamp = CP56Time(data,n+4);
                   iecSignal.quality = data[n+3];
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
    }

    return iecSignal;
}

/*uint ParseAPCInum(QByteArray &data){
   return ((unsigned char)data[2] + (((unsigned char)data[3])<<8));
}
*/

QList<CIECSignal> ParseFrame(QByteArray &data, uint *APCInum){
    QList<CIECSignal> result;



    (*APCInum) = CTools::ParseAPCInum(data);
    uchar typeID = uchar(data[6]);
    uchar count = uchar(data[7]);

    int stride = 0;
    switch (typeID){
    case 30: stride = 11; break;    //РѕРґРЅРѕСЌР»РµРјРµРЅС‚РЅР°СЏ РёРЅС„
    case 33: stride = 15; break;    //dword
    case 36: stride = 15; break;    //float
    case 34: stride = 13; break;    //int
    }

    for (int i=0; i< count; i++)
        result.append(ParseSignal(data,typeID,(i*stride)+12));
    return result;
}
QByteArray CIECSignal::lostBytes;

QList<CIECSignal> CIECSignal::ParseData(QByteArray &data, uint *APCInum){

    QList<CIECSignal> result;
    QByteArray d;

    if (lostBytes.length()>0){
        data.insert(0,lostBytes);
        lostBytes.clear();
    }


    uint packsize = (uchar)data[1]+2;

    if (packsize>data.length()){
        lostBytes.append(data);
        return result;
    }else
        result.append(ParseFrame(data,APCInum));

  if (packsize<data.length()){

        for (int i=(uchar)data[1]+2; i<data.length(); i++)
            d.append(data[i]);
        result.append(ParseData(d,APCInum));
    }
    return result;
}

