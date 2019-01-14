#include "iec104driver.h"
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <qlist>

static char testAct[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};
static char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
static char startCon[] = {0x68, 0x04, 0x0B, 0x00, 0x00, 0x00};
static char StartDTAct[] ={0x68, 0x04, 0x07, 0x00, 0x00, 0x00};

IEC104Driver* IEC104Driver::instance = NULL;

IEC104Driver::IEC104Driver():
    sock(new QTcpSocket())
{
    N_R = 0;
    N_T = 0;
    testTimer = new QTimer();

    testTimer->setSingleShot(false);
    connect(testTimer, SIGNAL(timeout()),this,SLOT(OnTestTimer()));

    connect(sock,SIGNAL(connected()),this,SLOT(OnConnected()));
    connect(sock,SIGNAL(disconnected()),this, SLOT(OnDisconnected()));
    connect(sock,SIGNAL(readyRead()), this, SLOT(OnSockReadyRead()));
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
  //  in.setDevice(sock);
}

///
/// \brief IEC104Driver::SendFullRequest - general interrogation command
/// \param ASDU
/// \param requestDescription - описатель запроса (20 - общий, 21 - группа 1 и т.д.)
///
void IEC104Driver::SendFullRequest(quint8 requestDescription)
{
    quint16 ASDU = settings->asdu;

    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   0x64, 0x01,
                   0x06,0x00,    //причина передачи - активация
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   0x00,0x00,0x00,
                   char(requestDescription)
                  };

   QByteArray buf = QByteArray(temp, 16);
    if (sock->state() == QTcpSocket::SocketState::ConnectedState){
        sock->write(buf,16);
        emit Message("<-- Команда опроса");
        N_T ++;
    }

}

void IEC104Driver::SendRequestCounter()
{

}

void IEC104Driver::SendRequestSingle()
{

}

void IEC104Driver::SendSyncTime()
{

}

void IEC104Driver::OnTestTimer()
{
        SendTestAct();
}

void IEC104Driver::SendTestAct()
{
   // char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
   QByteArray buf = QByteArray(testAct, 6);
    if (sock->state() == QTcpSocket::SocketState::ConnectedState){
        sock->write(buf,6);
        emit Message("<-- TestAct");

   //     sock->waitForBytesWritten();
     //   pLog->append("testFRact");
    }
}

void IEC104Driver::SendTestCon()
{
    //char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
   QByteArray buf = QByteArray(testFrCon, 6);

    if (sock->state() == QTcpSocket::SocketState::ConnectedState){
        sock->write(buf,6);
        emit Message("<-- TestCon");
    }
}

void IEC104Driver::Send_ConfirmPacks(){
    if (sock->state() == QTcpSocket::SocketState::ConnectedState)
    {
        uint count = (uint)N_R+1;
        char pack[] ={0x68, 0x04, 0x01, 0x00, char(count<<1),char(count>>7)};
        //buf = QByteArray(pack,6);
        //sock->write(buf);
        sock->write(pack, 6);
        lastAPCICount = N_R;
        emit Message(IEC104Tools::BytesToString(pack,6));
        emit Message("<-- Confirm");
    }

}


void IEC104Driver::SendStart()
{
    QByteArray buf = QByteArray(StartDTAct,6);
    sock->write(buf, 6);
    emit Message("<-- StartAct");

}

bool IEC104Driver::isStartAct(QByteArray data)
{
    for (int i=0; i<6; i++)
        if (data[i] != StartDTAct[i])
            return false;
    return true;
}

bool IEC104Driver::isStartCon(QByteArray data)
{
    for (int i=0; i<6; i++)
        if (data[i] != startCon[i])
            return false;
    return true;
}

bool IEC104Driver::isTestAct(QByteArray data)
{
    //char testAct[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};

    for (int i=0; i<6; i++)
        if (data[i] != testAct[i])
            return false;
    return true;

}

bool IEC104Driver::isTestCon(QByteArray data)
{
   // char testAct[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};

    for (int i=0; i<6; i++)
        if (data[i] != testFrCon[i])
            return false;
    return true;

}

IEC104Driver* IEC104Driver::GetInstance()
{
    if (IEC104Driver::instance==NULL)
    {
        IEC104Driver::instance = new IEC104Driver();
    }
    return IEC104Driver::instance;
}

///
/// \brief IEC104Driver::SetSettings
/// give connection parameters straight
/// \param settings
/// can't be NULL
/*void IEC104Driver::SetSettings(CSetting *settings)
{
    if (settings != NULL)
    {
        this->settings = settings;
    }
}*/

///
/// \brief IEC104Driver::SetSettings
/// build connection parameters from QSettings object
/// \param settings
/// can't be NULL
void IEC104Driver::SetSettings(QSettings *settings)
{

    if (settings != NULL)
    {
        settings->beginGroup("driver");
        this->settings = new CSetting(
            settings->value("IP").toString(),
            settings->value("asdu").toInt(),
            settings->value("port").toInt()
        );
        this->settings->t0 = settings->value("t0").toInt();
        this->settings->t1 = settings->value("t1").toInt();
        this->settings->t2 = settings->value("t2").toInt();
        this->settings->t3 = settings->value("t3").toInt();
        this->settings->k = settings->value("k").toInt();
        this->settings->w = settings->value("w").toInt();
        settings->endGroup();
    }
}

CSetting* IEC104Driver::GetSettings()
{
    return settings;
}


void IEC104Driver::OpenConnection(CSetting *_settings)
{
    //still open? exit
    if (sock->isOpen())
        return;

    //use new settings if got it
    if (_settings != NULL)
    {
        this->settings = _settings;
    }

    if (settings != NULL)
    {
        sock->open(QIODevice::ReadWrite);
        //setup connection
        qDebug()<< settings->IP;
        sock->connectToHost(settings->IP,settings->Port);

        //setup timer
        testTimer->setInterval(settings->t3*1000);

    }

    return;
}

void IEC104Driver::CloseConnection()
{
    sock->disconnectFromHost();
}

///
/// \brief IEC104Driver::SendCommand
/// \param type
/// \param ioa
/// запись однопозиционной команды
void IEC104Driver::SendCommand(quint16 type, quint32 ioa, quint8 value)
{
    qDebug() << "sending command "<< type << " " << ioa << " "<< value ;
    quint16 ASDU = settings->asdu;
    //char requestDescription = 0;
    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   45, 0x01,
                   0x06,0x00,    //причина передачи - активация
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                   char(value)
                  };

   QByteArray buf = QByteArray(temp, 16);
    if (sock->state() == QTcpSocket::SocketState::ConnectedState){
        sock->write(buf,16);
        emit Message("<-- Однопозиционная команда");
        N_T ++;
        emit Message(IEC104Tools::BytesToString(&buf));
    }

}

///
/// \brief IEC104Driver::SetPoint
/// \param type
/// \param ioa
/// \param value
///Запись уставки
void IEC104Driver::SetPoint(quint16 type, quint32 ioa, QVariant value)
{
    quint16 ASDU = settings->asdu;
    //char requestDescription = 0;

    if (type == 48 || type == 61)
    {
        quint16 uvalue = value.toUInt();
        char temp[] = {0x68, 0xE,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,    //причина передачи - активация
                       char(ASDU&0xFF), char(ASDU>>8),
                       char(ioa&0xFF),char((ioa)>>8),char((ioa)>>16),
                       char(uvalue&0xff),char((uvalue>>8)&0xff),char(0x00)
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (sock->state() == QTcpSocket::SocketState::ConnectedState){
            sock->write(buf,buf.count());
            emit Message("<-- Команда уставки, нормализованное значение");
            N_T ++;
            emit Message(IEC104Tools::BytesToString(&buf));
        }
        return;
    }//48,61
    if (type == 49 || type == 62)
    {
        int ivalue = value.toUInt();
        char temp[] = {0x68, 0xE,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,    //причина передачи - активация
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       char(ivalue&0xff),char((ivalue>>8)&0xff),0x00
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (sock->state() == QTcpSocket::SocketState::ConnectedState){
            sock->write(buf,buf.count());
            emit Message("<-- Команда уставки, масштабированное значение");
            N_T ++;
            emit Message(IEC104Tools::BytesToString(&buf));
        }
        return;
    }//49,62
    if (type == 50 || type == 63)
    {
        float fvalue = value.toFloat();
        QByteArray bytes(reinterpret_cast<const char*>(&fvalue), sizeof(fvalue));
        char temp[] = {0x68, 0x12,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,    //причина передачи - активация
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       bytes[0],bytes[1],bytes[2],bytes[3],0x00
                      };


        QByteArray buf = QByteArray(temp, 20);

        qDebug() << temp;
        if (sock->state() == QTcpSocket::SocketState::ConnectedState){
            sock->write(buf,20);
            emit Message("<-- Команда уставки с плавающей точкой");
            N_T ++;
            emit Message(IEC104Tools::BytesToString(&buf));
        }
        return;
    }//50,63
    if (type == 51 || type == 64)
    {
        uint dvalue = value.toUInt();
        char temp[] = {0x68, 0x12,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,    //причина передачи - активация
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       char(dvalue&0xff),char((dvalue>>8)&0xff),char((dvalue>>16)&0xff),char((dvalue>>24)&0xff),char(0x00)
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (sock->state() == QTcpSocket::SocketState::ConnectedState){
            sock->write(buf,buf.count());
            emit Message("<-- Команда уставки, строка 32 бит");
            N_T ++;
            emit Message(IEC104Tools::BytesToString(&buf));
        }
        return;
    }//48,61


}
//==================================================== PRIVATE SLOTS =======================================================================
void IEC104Driver::OnConnected()
{
    SendStart();
    testTimer->start();
    emit Connected();
}

void IEC104Driver::OnDisconnected()
{
    //sock->disconnectFromHost();
    sock->close();
    testTimer->stop();
    emit Disconnected();
}

/*===================================================================================
Format U:
[0] 0x68
[1] 0x4
[2] ######11

Format I:
[0] 0x68
[1] any
[2] #######0
=====================================================================================*/

void IEC104Driver::OnSockReadyRead()
{
    QByteArray buf= sock->readAll();
    QString str =QTime::currentTime().toString()+" -->[";


    emit Message(QTime::currentTime().toString()+" -->" + IEC104Tools::BytesToString(&buf));

    // reset timer when any package received
    testTimer->stop();
    testTimer->start();

    //----------------------------------- processing format U packages ---------------
    if (buf.count()==6 && ((buf[2]&0x3) == 0x3)){
        if (isTestAct(buf))
        {
            emit Message("--> TestAct");

            SendTestCon();
            return;
        }
        if(isTestCon(buf))
        {
            emit Message("--> TestCon");
            return;
        }
        if(isStartCon(buf))
        {
            emit Message("--> StartCon");
            return;
        }
    }//if count 6

    //-------------------------------- processing packages I (with num) -----------
    //N_R = IEC104Tools::ParseAPCInum(buf);
    if ((buf.length()>6) && ((buf[2]&0x1) == 0))
    {
       QList<CIECSignal>* s = IEC104Tools::ParseData(buf,&N_R,&N_T);
       if (s!=NULL)
       {
           bool isSequence = (buf[7]&0x80)>0;
           //emit Message("count: " + QString::number(s->length()));
           emit Message("SQ= " + QString::number(isSequence ? 1 : 0 ) + QString("; count: ") + QString::number(s->length()));
           foreach (CIECSignal signal, (*s))
           {
               if (signal.ASDU == settings->asdu)
               {
                emit Message(signal.GetValueString());
                emit IECSignalReceived(signal);
               }
           }
           delete s; //
       }
    }

    //send confirmation
    //2.. хрен знает зачем, но подтверждение надо отправлять раньше чем через w пакетов
   if ((N_R - lastAPCICount + 2u)>=(settings->w)){
       Send_ConfirmPacks();
    }
}

void IEC104Driver::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::information(0, tr("IEC104 client"), sock->errorString(),QMessageBox::Ok,QMessageBox::NoButton);
    OnDisconnected();
}

void IEC104Driver::Interrogation()
{

}

void IEC104Driver::ClockSynch()
{

}
