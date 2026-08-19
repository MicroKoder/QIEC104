#include "ctools.h"
#include "QDebug"
#include <cstring>
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
    QList<CIECSignal> result;

    // Minimum I-format: APCI(6) + type + vsq + cot(2) + asdu(2) = 12
    if (data.size() < 12)
        return result;

    if (R_Count !=0)
    {
        // Store next expected N(R) = received N(S) + 1
        const quint16 ns = (((unsigned char)data[2]>>1) + (((unsigned char)data[3])<<7)) & 0x7FFF;
        (*R_Count) = (ns + 1) & 0x7FFF;
    }

    uchar typeID = uchar(data[6]);

    uchar count = ((unsigned char)data[7])&0x7F;

    if (count == 0)
        return result;

    bool isSequence = (data[7]&0x80)>0;


    QVariant value;
    quint32 addr;
    int offset=0;
    int stride=0;
    //------------------------------------------------------- SQ = 1  ----------------------------
    if (isSequence)
    {
        if (data.size() < 15)
            return result;

        uint32_t LB = (uchar)data[12];
        uint32_t MB = (uchar)data[13];
        uint32_t HB = (uchar)data[14];
        addr = LB + (MB<<8) + (HB<<16);
        qDebug() << "got IOA: " << QString::number(addr);
        uint val_u32;
        for (int i=0; i<count; i++)
        {            
            CIECSignal signal;
            signal.ASDU = ( (uchar)data[11]<<8) + (uchar)data[10];
            signal.SetAddress(addr);
            signal.timestamp = CP56Time();
            signal.bNeverUpdated =false;
            addr++;

            signal.SetType(typeID);
            offset=15;
            stride=0;

            // Pre-check element fits in buffer
            switch (typeID)
            {
                case 1: case 3: stride = 1; break;
                case 5: stride = 2; break;
                case 7: case 15: case 20: stride = 5; break;
                case 9: case 11: stride = 3; break;
                case 13: stride = 5; break;
                case 21: stride = 2; break;
                case 30: case 31: stride = 8; break;
                case 32: stride = 9; break;
                case 33: case 36: stride = 12; break;
                case 34: case 35: stride = 10; break;
                case 37: stride = 12; break;
                case 38: case 39: case 40: stride = 11; break;
                default: stride = 0; break;
            }
            if (stride > 0 && (offset + (i + 1) * stride) > data.size())
                break;

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
                        quint16 raw = (uchar)data[offset+i*stride] + ((uchar)(data[offset+i*stride+1])<<8);
                        qint16 iValue = static_cast<qint16>(raw);
                        signal.value = QVariant(int(iValue));
                        signal.quality = (uchar)data[offset +i*stride + 2];

                        result.append(signal);
                    }
                break;
                case 13:
                    {
                        stride = 5;
                        quint32 raw = ((uchar)data[offset+i*stride]) |
                                   ((uchar)data[offset+i*stride+1]<<8) |
                                   ((uchar)data[offset+i*stride+2]<<16) |
                                   ((uchar)data[offset+i*stride+3]<<24);
                        float fValue;
                        memcpy(&fValue, &raw, sizeof(fValue));
                        signal.value = QVariant(fValue);

                        signal.quality = (uchar)data[offset +i*stride + 4];
                        qDebug() << "got IOA: " << QString::number(signal.GetAddress()) << " value: " <<  signal.value.toString();
                        result.append(signal);
                    }
                break;
                // M_IT_NA_1 integrated totals (BCR: 4-byte counter + seq/quality)
                case 15:
                    {
                        stride = 5;
                        val_u32 = (uchar)data[offset+i*stride] +
                                ((uchar)data[offset+i*stride+1]<<8) +
                                ((uchar)data[offset+i*stride+2]<<16) +
                                ((uchar)data[offset+i*stride+3]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+4];
                        result.append(signal);
                    }
                break;
                // M_PS_NA_1 packed single-point with status change detection (SCD 4 + QDS 1)
                case 20:
                    {
                        stride = 5;
                        val_u32 = (uchar)data[offset+i*stride] +
                                ((uchar)data[offset+i*stride+1]<<8) +
                                ((uchar)data[offset+i*stride+2]<<16) +
                                ((uchar)data[offset+i*stride+3]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+4];
                        result.append(signal);
                    }
                break;
                // M_ME_ND_1 normalized value without quality descriptor
                case 21:
                    {
                        stride = 2;
                        uint uiValue = (uchar)data[offset+i*stride] + (((uchar)data[offset+i*stride+1])<<8);
                        signal.value = QVariant(uiValue);
                        signal.quality = 0;
                        result.append(signal);
                    }
                break;

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
                    quint32 uivalue =(uchar)data[offset+i*stride] +
                        ((uchar)data[offset+i*stride+1]<<8) +
                        ((uchar)data[offset+i*stride+2]<<16) +
                        ((uchar)data[offset+i*stride+3]<<24);

                    signal.value = QVariant(uivalue);
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
                    quint16 raw = (uchar)data[offset+i*stride] + ((uchar)(data[offset+i*stride+1])<<8);
                    qint16 iValue = static_cast<qint16>(raw);
                    signal.value = QVariant(int(iValue));
                    signal.quality = (uchar)data[offset +i*stride + 2];
                    signal.timestamp = CP56Time(data,offset + i*stride+3);

                    result.append(signal);
                }; break;
                case 36:
                {
                    stride = 12;
                    quint32 raw = ((uchar)data[offset+i*stride]) |
                           ((uchar)data[offset+i*stride+1]<<8) |
                           ((uchar)data[offset+i*stride+2]<<16) |
                           ((uchar)data[offset+i*stride+3]<<24);
                    float fValue;
                    memcpy(&fValue, &raw, sizeof(fValue));
                    signal.value = QVariant(fValue);

                    signal.quality = (uchar)data[offset +i*stride + 4];
                    signal.timestamp = CP56Time(data,offset + i*stride+5);

                    result.append(signal);
                }; break;
                // M_IT_TB_1 integrated totals + CP56Time2a
                case 37:
                {
                    stride = 12;
                    val_u32 = (uchar)data[offset+i*stride] +
                            ((uchar)data[offset+i*stride+1]<<8) +
                            ((uchar)data[offset+i*stride+2]<<16) +
                            ((uchar)data[offset+i*stride+3]<<24);
                    signal.value = QVariant(val_u32);
                    signal.quality = (uchar)data[offset+i*stride+4];
                    signal.timestamp = CP56Time(data, offset + i*stride + 5);
                    result.append(signal);
                }; break;
                // M_EP_TD_1 / M_EP_TE_1 / M_EP_TF_1: SEP|SPE|OCI + QDP + CP16 + CP56
                case 38:
                case 39:
                case 40:
                {
                    stride = 11;
                    signal.value = QVariant((uchar)data[offset+i*stride]);
                    signal.quality = (uchar)data[offset+i*stride+1];
                    // bytes +2,+3 = CP16Time2a elapsed (ms), skipped in value
                    signal.timestamp = CP56Time(data, offset + i*stride + 4);
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

            int expectedStride = 0;
            switch (typeID)
            {
                case 1: case 3: expectedStride = 4; break;
                case 5: expectedStride = 5; break;
                case 7: case 15: case 20: expectedStride = 8; break;
                case 9: case 11: expectedStride = 6; break;
                case 13: expectedStride = 8; break;
                case 21: expectedStride = 5; break;
                case 30: case 31: expectedStride = 11; break;
                case 32: expectedStride = 12; break;
                case 33: case 36: expectedStride = 15; break;
                case 34: case 35: expectedStride = 13; break;
                case 37: expectedStride = 15; break;
                case 38: case 39: case 40: expectedStride = 14; break;
                default: expectedStride = 4; break;
            }
            if ((offset + (i + 1) * expectedStride) > data.size())
                break;

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
                        stride = 4;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(uint((uchar)data[offset + i*stride + 3] & 0x03));
                        signal.quality = (uchar)data[offset + i*stride +3] & 0xFC;

                        result.append(signal);
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
                case 7:
                    {
                        stride = 8;
                        if ((offset + (i + 1) * stride) > data.size())
                            break;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        val_u32 = (uchar)data[offset+i*stride+3] +
                                ((uchar)data[offset+i*stride+4]<<8) +
                                ((uchar)data[offset+i*stride+5]<<16) +
                                ((uchar)data[offset+i*stride+6]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+7];
                        result.append(signal);
                    } break;
                //uint
                case 9:
                    {
                        stride = 6;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(uint((uchar)data[offset + i*stride + 3] + ((uchar)data[offset + i*stride + 4]<<8)));
                        signal.quality = (uchar)data[offset + i*stride +5];

                        result.append(signal);
                    }break;
                //int
                case 11:
                    {
                        stride = 6;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        quint16 raw = (uchar)data[offset + i*stride + 3] + ((uchar)data[offset + i*stride + 4]<<8);
                        signal.value = QVariant(int(static_cast<qint16>(raw)));
                        signal.quality = data[offset + i*stride +5];

                        result.append(signal);
                    }break;
                //float
                case 13:
                    {
                        stride = 8;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);

                        quint32 raw = ((uchar)data[offset+i*stride+3]) |
                               ((uchar)data[offset+i*stride+4]<<8) |
                               ((uchar)data[offset+i*stride+5]<<16) |
                               ((uchar)data[offset+i*stride+6]<<24);
                        float fValue;
                        memcpy(&fValue, &raw, sizeof(fValue));

                        signal.value = QVariant(fValue);
                        signal.quality = data[offset + i*stride + 7];
                        qDebug() << "got IOA: " << QString::number(addr) << " value: " << signal.value.toString();

                        result.append(signal);
                    }break;
                // M_IT_NA_1 integrated totals
                case 15:
                    {
                        stride = 8;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        val_u32 = (uchar)data[offset+i*stride+3] +
                                ((uchar)data[offset+i*stride+4]<<8) +
                                ((uchar)data[offset+i*stride+5]<<16) +
                                ((uchar)data[offset+i*stride+6]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+7];
                        result.append(signal);
                    }break;
                // M_PS_NA_1 packed SP + SCD
                case 20:
                    {
                        stride = 8;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        val_u32 = (uchar)data[offset+i*stride+3] +
                                ((uchar)data[offset+i*stride+4]<<8) +
                                ((uchar)data[offset+i*stride+5]<<16) +
                                ((uchar)data[offset+i*stride+6]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+7];
                        result.append(signal);
                    }break;
                // M_ME_ND_1 normalized without quality
                case 21:
                    {
                        stride = 5;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(uint((uchar)data[offset + i*stride + 3] + ((uchar)data[offset + i*stride + 4]<<8)));
                        signal.quality = 0;
                        result.append(signal);
                    }break;
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
                case 31:
                    {
                        stride = 11;
                        addr = GetIOA(data[offset + i*stride],data[offset + i*stride +1],data[offset + i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant(uint((uchar)data[offset + i*stride + 3] & 0x03));
                        signal.quality = (uchar)data[offset + i*stride +3] & 0xFC;
                        signal.timestamp = CP56Time(data,offset + i*stride+4);

                        result.append(signal);
                    }break;
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
                         addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);

                        quint32 raw = ((uchar)data[offset+i*stride+3]) |
                               ((uchar)data[offset+i*stride+4]<<8) |
                               ((uchar)data[offset+i*stride+5]<<16) |
                               ((uchar)data[offset+i*stride+6]<<24);
                        float fValue;
                        memcpy(&fValue, &raw, sizeof(fValue));

                        signal.value = QVariant(fValue);
                        signal.quality = (uchar)(data[offset + i*stride+7]);
                        signal.timestamp = CP56Time(data,offset + i*stride+8);
                        qDebug() << "got IOA: " << QString::number(addr) << " value: " << signal.value.toString();

                        result.append(signal);
                    };
                break;
                // M_IT_TB_1 integrated totals + CP56
                case 37:
                    {
                        stride = 15;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        val_u32 = (uchar)data[offset+i*stride+3] +
                                ((uchar)data[offset+i*stride+4]<<8) +
                                ((uchar)data[offset+i*stride+5]<<16) +
                                ((uchar)data[offset+i*stride+6]<<24);
                        signal.value = QVariant(val_u32);
                        signal.quality = (uchar)data[offset+i*stride+7];
                        signal.timestamp = CP56Time(data, offset + i*stride + 8);
                        result.append(signal);
                    };
                break;
                // M_EP_TD_1 / M_EP_TE_1 / M_EP_TF_1
                case 38:
                case 39:
                case 40:
                    {
                        stride = 14;
                        addr = GetIOA(data[offset + i*stride], data[offset + i*stride +1],data[offset+ i*stride +2]);
                        signal.SetAddress(addr);
                        signal.value = QVariant((uchar)data[offset+i*stride+3]);
                        signal.quality = (uchar)data[offset+i*stride+4];
                        // +5,+6 = CP16Time2a elapsed
                        signal.timestamp = CP56Time(data, offset + i*stride + 7);
                        result.append(signal);
                    };
                break;
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

    // Prepend leftover bytes from a previous incomplete APDU
    if (!lostBytes.isEmpty()){
        data.prepend(lostBytes);
        lostBytes.clear();
    }

    if (data.isEmpty() || (uchar)data[0]!=0x68)
        return result;

    //длина APCI = APDU + 2
    uint APCILength = (uchar)data[1]+2;

    //размер принятого массива меньше APCI, сохраняем этот кусок до следующего вызова функции
    if (APCILength>(uint)data.length())
    {
        lostBytes.append(data);
        return result;
    }

    // Only I-format APDUs carry ASDU data (LSB of control field byte is 0)
    const bool isIFormat = (APCILength > 6) && (((uchar)data[2] & 0x01) == 0);
    if (isIFormat)
    {
        QList<CIECSignal> temp = ParseFrame(data,R_Count);
        if (!temp.isEmpty())
        {
            result->append(temp);
        }
    }

  //размер принятого массива больше APCI, рекурсивный вызов функции для оставшегося куска
  if (APCILength<(uint)data.length()){
        QByteArray d = data.mid((int)APCILength);

        QList<CIECSignal>* temp = ParseData(d,R_Count);

        if (temp != NULL)
        {
            result->append(*temp);
            delete temp;
        }
    }
  if (R_Count)
      qDebug() <<"APCI: "<< (*R_Count);
    return result;
}

void IEC104Tools::ClearLostBytes()
{
    lostBytes.clear();
}


