#include "ctools.h"
#include "QDebug.h"
IEC104Tools::IEC104Tools()
{

}


QByteArray IEC104Tools::lostBytes=QByteArray();

QString IEC104Tools::GetQualityString(uchar quality)
{
    QString quality_str;
    if (quality&1)
        quality_str+="OV ";

    if (quality&8)
        quality_str+="EI ";

    if (quality&16)
        quality_str+="BL ";

    if (quality&32)
        quality_str+="SB ";

    if (quality&64)
        quality_str+="NT ";

    if (quality&128)
        quality_str+="IV ";

    if (quality==0)
        quality_str = "GOOD";
    return quality_str;
}
quint32 GetIOA(char LB, char MB, char HB)
{
    quint32 lb = (unsigned char)(LB);
    quint32 mb = (unsigned char)(MB);
    quint32 hb = (unsigned char)(HB);

    return lb+ (mb<<8) + (hb<<16);
}

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

//uint IEC104Tools::ParseAPCInum(QByteArray &data){
//   return (((unsigned char)data[2]>>1) + (((unsigned char)data[3])<<7));
//}


///Обработка целого фрейма, должен быть валидный пакет данных
QList<CIECSignal> IEC104Tools::ParseFrame(QByteArray &data, quint16 *R_Count){
    QList<CIECSignal> result;//=new QList<CIECSignal>();

    if (R_Count !=0)
    {
        (*R_Count) = (((unsigned char)data[2]>>1) + (((unsigned char)data[3])<<7));
    }

//    if (T_Count !=0)
//    {
//        (*T_Count) = ((unsigned char)data[4]>>1) + ((unsigned char)data[5]<<7);
//    }

    uchar typeID = uchar(data[6]);

    uchar count = ((unsigned char)data[7])&0x7F; //достаем количество элементов

    if (count == 0)
        return result;    //нет элементов, можно не продолжать

    bool isSequence = (data[7]&0x80)>0; //последовательность или одиночный элемент.


    QVariant value;
    quint32 addr;
    int offset=0;
    int stride=0;
    //------------------------------------------------------- SQ = 1  ----------------------------
    if (isSequence)
    {

        //addr = data[12] | ((((uint)data[13]&0xFF)<<8)&0xFF00) | (((((uint)data[14])&0xFF)<<16)&0xFF0000);
        uint32_t LB = (uchar)data[12];
        uint32_t MB = (uchar)data[13];
        uint32_t HB = (uchar)data[14];
        addr = LB + (MB<<8) + (HB<<16);
        qDebug() << "got IOA: " << QString::number(addr);
        uint val_u32;   //буфер для вычисления значения
        for (int i=0; i<count; i++)
        {            
            CIECSignal signal;
            signal.ASDU = ( (uchar)data[11]<<8) + (uchar)data[10];
            signal.SetAddress(addr);
            signal.timestamp = CP56Time();
            signal.bNeverUpdated =false;
            addr++;

            signal.SetType(typeID);
            offset=15;//номер байта с которого начинаются значения
            stride=0;

            switch (typeID)
            {
                //single-point
                case 1:
                    {
                        stride = 1;
                        signal.value = QVariant(data[offset+i*stride]&0x01);
                        signal.quality = uchar(data[offset+i*stride])&0xFE;

                        result.append(signal);
                    }
                break;
                //тип 2 не определен для SQ=1
                //

                //double-point
                case 3:
                    {
                        stride = 1;
                        signal.value = QVariant((uchar)data[offset+i*stride]&0x03);
                        signal.quality = uchar(data[offset+i*stride])&0xFC;

                        result.append(signal);
                    }
                break;
                //положение отпаек M_ST_NA
                case 5:
                    {
                        stride = 2;
                        signal.value = QVariant((uchar)data[offset+i*stride]);
                        signal.quality = uchar(data[offset+i*stride+1]);

                        result.append(signal);
                    }
                break;
                // 32-bit string (dword)
                case 7:
                {
                    stride = 5; //4 байта на значение и байт качества
                    val_u32 = (uchar)data[offset+i*stride] +
                            ((uchar)data[offset+i*stride+1]<<8) +
                            ((uchar)data[offset+i*stride+2]<<16) +
                            ((uchar)data[offset+i*stride+3]<<24);
                    signal.value = QVariant(val_u32);
                    signal.quality = (uchar)data[offset+i*stride+4];

                    result.append(signal);
                }
                break;
                //uint
                case 9:
                    {
                        stride =3;
                        uint uiValue = (uchar)data[offset+i*stride] + (((uchar)data[offset+i*stride+1])<<8);
                        signal.value = QVariant(uiValue);
                        signal.quality = (uchar)data[offset +i*stride + 2];

                        result.append(signal);
                    }
                break;
                //int
                case 11:
                    {
                        stride =3;
                        uint *bytes = new uint();
                        (*bytes) = (uchar)data[offset+i*stride] + ((uchar)(data[offset+i*stride+1])<<8);
                        int (*iValue) = reinterpret_cast<int*>(bytes);
                        signal.value = QVariant(*iValue);
                        signal.quality = (uchar)data[offset +i*stride + 2];

                        result.append(signal);
                    }
                break;
                case 13:
                    {
                        stride = 5;
                        uint *bytes = new uint();
                        (*bytes) = ((uchar)data[offset+i*stride]) |
                                   ((uchar)data[offset+i*stride+1]<<8) |
                                   ((uchar)data[offset+i*stride+2]<<16) |
                                   ((uchar)data[offset+i*stride+3]<<24);
                        //float *fValue = reinterpret_cast<float*>(bytes);
                        float *fValue = (float*)(bytes);
                        signal.value = QVariant(*fValue);

                        signal.quality = (uchar)data[offset +i*stride + 4];
                        qDebug() << "got IOA: " << QString::number(signal.GetAddress()) << " value: " <<  signal.value.toString();
                        result.append(signal);
                    }
                break;
                //integral sum
                case 15:break;
                //sp packed
                case 20:break;
                //uint without quality
                case 21:break;

                //single point + timestamp
                case 30:
                {
                    stride = 8;
                    signal.value = QVariant((uchar)(data[offset + i*stride]&0x01));
                    signal.quality = (uchar)(data[offset + i*stride])&0xFE;
                    signal.timestamp = CP56Time(data,offset + i*stride+1);

                    result.append(signal);
                }; break;
                //M_DP_TB_1 double point
                case 31:
                {
                    stride = 8;
                    signal.value = QVariant((uchar)data[offset+i*stride]&0x03);
                    signal.quality = (uchar)(data[offset + i*stride])&0xFC;
                    signal.timestamp = CP56Time(data,offset + i*stride+1);

                    result.append(signal);
                };break;

            case 32:
            {
                stride = 9; //байт значения, байт качества, 7 байт метки времени
                signal.value = QVariant((uchar)data[offset+i*stride]);
                signal.quality = (uchar)(data[offset + i*stride+1]);
                signal.timestamp = CP56Time(data,offset + i*stride+2);

                result.append(signal);
            }; break;

                //bitstring
                case 33:
                {
                    stride = 12; //4 байта на значение и байт качества
                    uint *bytes = new (uint);
                    (*bytes)=(uchar)data[offset+i*stride] +
                        ((uchar)data[offset+i*stride+1]<<8) +
                        ((uchar)data[offset+i*stride+2]<<16) +
                        ((uchar)data[offset+i*stride+3]<<24);
                    uint *uivalue = reinterpret_cast<uint*>(bytes);

                    signal.value = QVariant(*uivalue);
                    signal.quality = (uchar)data[offset+i*stride+4];
                    signal.timestamp = CP56Time(data,offset + i*stride+5);

                    result.append(signal);
                }; break;
                //uint
                case 34:
                {
                    stride =10;
                    uint uiValue = (uchar)data[offset+i*stride] + (((uchar)data[offset+i*stride+1])<<8);
                    signal.value = QVariant(uiValue);
                    signal.quality = (uchar)data[offset +i*stride + 2];
                    signal.timestamp = CP56Time(data,offset + i*stride+3);

                    result.append(signal);
                }; break;
                //int
                case 35:
                {
                    stride =10;
                    uint *bytes = new uint();
                    (*bytes) = (uchar)data[offset+i*stride] + ((uchar)(data[offset+i*stride+1])<<8);
                    int (*iValue) = reinterpret_cast<int*>(bytes);
                    signal.value = QVariant(*iValue);
                    signal.quality = (uchar)data[offset +i*stride + 2];
                    signal.timestamp = CP56Time(data,offset + i*stride+3);

                    result.append(signal);
                }; break;
                case 36:
                {
                    stride = 12;
                    uint *bytes = new uint();
                    (*bytes) = ((uchar)data[offset+i*stride]) |
                           ((uchar)data[offset+i*stride+1]<<8) |
                           ((uchar)data[offset+i*stride+2]<<16) |
                           ((uchar)data[offset+i*stride+3]<<24);
                    //float *fValue = reinterpret_cast<float*>(bytes);
                    float *fValue = (float*)(bytes);
                    signal.value = QVariant(*fValue);

                    signal.quality = (uchar)data[offset +i*stride + 4];
                    signal.timestamp = CP56Time(data,offset + i*stride+5);

                    result.append(signal);
                }; break;
            }

            //signal.value = value;
            //signal.quality = quality;


        }//end for

    }else
        //------------------------- SQ=0  -----------------------------
    {
        uint val_u32;   //буфер для вычисления значения
        offset = 12;
        for (int i=0; i<count; i++)
        {
            CIECSignal signal;
            quint32 addr;
            signal.ASDU = ((uchar)data[11]<<8) + (uchar)data[10];
            signal.bNeverUpdated = false;
            signal.SetType(typeID);

            switch (typeID)
            {
                case 1:
                    {
                        stride = 4;
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(bool((data[offset + i*stride + 3]&0x01)>0));
                        signal.quality = data[offset + i*stride +3]&0xFE;

                        result.append(signal);
                    }break;
                case 3:
                    {
                        stride = 1;
                    }break;
                case 5:
                    {
                        stride = 5;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        signal.SetAddress(addr);
                        signal.value = QVariant(char(data[offset + i*stride + 3]));
                        signal.quality = data[offset + i*stride + 4];

                        result.append(signal);
                    }
                break;
                //32-bit string (dword)
                case 7: break;
                //uint
                case 9:
                    {
                        stride = 6;
             //           (*p_addr) = data[offset + i*stride] + (data[offset + i*stride +1]<<8) + (data[offset+ i*stride +2]<<16);
             //           addr = reinterpret_cast<quint32*>(p_addr);
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(uint(data[offset + i*stride + 3] + (data[offset + i*stride + 4]<<8)));
                        signal.quality = data[offset + i*stride +5];

                        result.append(signal);
                    }break;
                //int
                case 11:
                    {
                        stride = 6;
           //            (*p_addr) = data[offset + i*stride] + (data[offset + i*stride +1]<<8) + (data[offset+ i*stride +2]<<16);
           //             addr = reinterpret_cast<quint32*>(p_addr);
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(int(data[offset + i*stride + 3] + (data[offset + i*stride + 4]<<8)));
                        signal.quality = data[offset + i*stride +5];

                        result.append(signal);
                    }break;
                //float
                case 13:
                    {
                        stride = 8;
               //         (*p_addr) = data[offset + i*stride] + (data[offset + i*stride +1]<<8) + (data[offset+ i*stride +2]<<16);
               //         addr = reinterpret_cast<quint32*>(p_addr);

                        //addr = (data[offset+ i*stride +2]<<16) + ((data[offset + i*stride +1])<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);


                      /*  val_u32 = (uchar)data[offset+i*stride] +
                                ((uchar)data[offset+i*stride+1]<<8) +
                                ((uchar)data[offset+i*stride+2]<<16) +
                                ((uchar)data[offset+i*stride+3]<<24);
                        signal.value = QVariant(val_u32);
                        */
                        uint *bytes = new uint();
                        (*bytes) = ((uchar)data[offset+i*stride+3]) |
                               ((uchar)data[offset+i*stride+4]<<8) |
                               ((uchar)data[offset+i*stride+5]<<16) |
                               ((uchar)data[offset+i*stride+6]<<24);
                        //float *fValue = reinterpret_cast<float*>(bytes);
                        float *fValue = (float*)(bytes);

                        signal.value = QVariant(*fValue);


                        //signal.value = QVariant(int(data[offset + i*stride + 3] + (data[offset + i*stride + 4]<<8)+ (data[offset + i*stride + 5]<<16)+ (data[offset + i*stride + 6]<<24)));
                        signal.quality = data[offset + i*stride + 7];
                        qDebug() << "got IOA: " << QString::number(addr) << " value: " << signal.value.toString();

                        result.append(signal);
                    }break;
                case 15: break;
                case 20: break;
                case 21: break;
                case 30:
                    {
                        stride = 11;
                        addr = GetIOA(data[offset + i*stride],data[offset + i*stride +1],data[offset + i*stride +2]);
                       // addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        signal.SetAddress(addr);
                        signal.value = QVariant(bool((data[offset + i*stride + 3]&0x01)>0));
                        signal.quality = data[offset + i*stride +3]&0xFE;
                        signal.timestamp = CP56Time(data,offset + i*stride+4);

                        result.append(signal);
                    }break;
                case 31:break;
                case 32:
                   {
                        stride = 12; //3 байта - IOA, 1 байт значения, байт качества, 7 байт метки времени
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);

                        signal.value = QVariant((uchar)data[offset+i*stride+3]);
                        signal.quality = (uchar)(data[offset + i*stride+4]);
                        signal.timestamp = CP56Time(data,offset + i*stride+5);

                        result.append(signal);
                    }; break;
                case 33:
                    {
                        stride = 15;
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);

                        val_u32 = (uchar)data[offset+i*stride+3] +
                                ((uchar)data[offset+i*stride+4]<<8) +
                                ((uchar)data[offset+i*stride+5]<<16) +
                                ((uchar)data[offset+i*stride+6]<<24);
                        signal.value = QVariant(val_u32);

                        //signal.value = QVariant((data[offset+ i*stride +6]<<24) + (data[offset+ i*stride +5]<<16) + (data[offset + i*stride +4]<<8) + data[offset + i*stride+3]);
                        signal.quality = (uchar)(data[offset + i*stride+7]);
                        signal.timestamp = CP56Time(data,offset + i*stride+8);

                        result.append(signal);
                    };
                break;
                case 34:
                    {
                        stride = 13;
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                         addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant( (data[offset + i*stride +4]<<8) + data[offset + i*stride+3]);
                        signal.quality = (uchar)(data[offset + i*stride+5]);
                        signal.timestamp = CP56Time(data,offset + i*stride+6);

                        result.append(signal);
                    };
                break;
                case 35:
                    {
                        stride = 13;
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                         addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant( (data[offset + i*stride +4]<<8) + data[offset + i*stride+3]);
                        signal.quality = (uchar)(data[offset + i*stride+5]);
                        signal.timestamp = CP56Time(data,offset + i*stride+6);

                        result.append(signal);
                    };
                break;
                case 36:
                    {
                        stride = 15;
                        //addr = (data[offset+ i*stride +2]<<16) + (data[offset + i*stride +1]<<8) + data[offset + i*stride];
                         addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);

                        uint *bytes = new uint();
                        (*bytes) = ((uchar)data[offset+i*stride+3]) |
                               ((uchar)data[offset+i*stride+4]<<8) |
                               ((uchar)data[offset+i*stride+5]<<16) |
                               ((uchar)data[offset+i*stride+6]<<24);
                        //float *fValue = reinterpret_cast<float*>(bytes);
                        float *fValue = (float*)(bytes);

                        signal.value = QVariant(*fValue);
                        signal.quality = (uchar)(data[offset + i*stride+7]);
                        signal.timestamp = CP56Time(data,offset + i*stride+8);
                        qDebug() << "got IOA: " << QString::number(addr) << " value: " << signal.value.toString();

                        result.append(signal);
                    };
                break;
                case 37:break;
                case 38:break;
                case 39:break;
                case 40:break;
            }


        }//end for
        /*
        //c метками в формате cp56
        */
    }
    return result;
}

///Обработка пакета байт
QList<CIECSignal>* IEC104Tools::ParseData(QByteArray &data, quint16 *R_Count){

    QList<CIECSignal>* result = new QList<CIECSignal>();


   /* if (lostBytes.length()>0){
        data.insert(0,lostBytes);
        lostBytes.clear();
    }*/
    if ((uchar)data[0]!=0x68)
        return result;

    //длина APCI = APDU + 2
    uint APCILength = (uchar)data[1]+2;

    //размер принятого массива меньше APCI, сохраняем этот кусок до следующего вызова функции
    if (APCILength>(uint)data.length())
    {
        lostBytes.append(data);
        return result;
    }else
    {
        QList<CIECSignal> temp = ParseFrame(data,R_Count);
        if (!temp.isEmpty())
        {
            result->append(temp);
        }
    }

    QByteArray d;
  //размер принятого массива больше APCI, рекурсивный вызов функции для оставшегося куска
  if (APCILength<(uint)data.length()){

        for (int i=(uchar)data[1]+2; i<data.length(); i++)
            d.append(data[i]);

        QList<CIECSignal>* temp = ParseData(d,R_Count);
      //    QList<CIECSignal>* temp = ParseData(data[],APCInum);

        if (temp != NULL)
            result->append(*temp);
    }
  qDebug() <<"APCI: "<< (*R_Count);
    return result;
}


