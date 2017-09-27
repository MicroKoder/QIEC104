#include "iec104driver.h"
#include <QMessageBox>
#include <QDebug>
#include <QTime>
/*IEC104Driver* IEC104Driver::GetInstance()
{
    if (IEC104Driver::hinstance==NULL)
    {
        IEC104Driver::hinstance = new IEC104Driver();
    }
    return IEC104Driver::hinstance;

}
*/
static char testAct[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};
static char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
static char startCon[] = {0x68, 0x04, 0x0B, 0x00, 0x00, 0x00};
static char StartDTAct[] ={0x68, 0x04, 0x07, 0x00, 0x00, 0x00};

IEC104Driver::IEC104Driver():
    sock(new QTcpSocket())
{

    testTimer = new QTimer();

    testTimer->setSingleShot(false);
    connect(testTimer, SIGNAL(timeout()),this,SLOT(OnTestTimer()));

    connect(sock,SIGNAL(connected()),this,SLOT(OnConnected()));
    connect(sock,SIGNAL(disconnected()),this, SLOT(OnDisconnected()));
    connect(sock,SIGNAL(channelReadyRead(int)), this, SLOT(OnSockReadyRead(int)));
    connect(sock,SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
  //  in.setDevice(sock);
}

void IEC104Driver::OnTestTimer()
{
        SendTestAct();
}


void IEC104Driver::Send_ConfirmPacks(){
    uint count = (uint)N_R+1;
    char pack[] ={0x68, 0x04, 0x01, 0x00, char(count<<1),char(count>>7)};
    buf = QByteArray(pack,6);
    sock->write(buf);

    lastAPCICount = N_R;
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

void IEC104Driver::SendTestAct()
{
   // char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
    buf = QByteArray(testAct, 6);
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
    buf = QByteArray(testFrCon, 6);
    if (sock->state() == QTcpSocket::SocketState::ConnectedState){
        sock->write(buf,6);
        emit Message("<-- TestCon");
    }
}

void IEC104Driver::SendStart()
{
    buf = QByteArray(StartDTAct,6);
    sock->write(buf, 6);
    emit Message("<-- StartAct");

}

void IEC104Driver::SendFullRequest()
{

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

void IEC104Driver::OpenConnection(CSetting *settings)
{
    if (sock->isOpen())
        return;

    sock->open(QIODevice::ReadWrite);
    sock->connectToHost(settings->IP,settings->Port);
    testTimer->setInterval(settings->t3*1000);
    emit Connected();
    return;
}

void IEC104Driver::CloseConnection()
{
    sock->disconnectFromHost();
}

void IEC104Driver::OnConnected()
{
    SendStart();
    testTimer->start();
}

void IEC104Driver::OnDisconnected()
{
    sock->close();
    emit Disconnected();
}

void IEC104Driver::OnSockReadyRead(int)
{
    QByteArray buf= sock->readAll();
    QString str =QTime::currentTime().toString()+" [";
    foreach(char c, buf)
    {
        str += QString::number((unsigned char)c,16)+" ";
    }
    str += "]";

    emit Message(str);

    if (buf.count()==6){
        if (isTestAct(buf))
        {
            str = "--> TestAct";
            emit Message(str);

            SendTestCon();
            return;
        }
        if(isTestCon(buf))
        {
            str = "--> TestCon";
            emit Message(str);
            return;
        }
        if(isStartCon(buf))
        {
            emit Message("--> StartCon");
            return;
        }
    }//if count 6

}

void IEC104Driver::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::information(0, tr("IEC104 client"), sock->errorString(),QMessageBox::Ok,QMessageBox::NoButton);
}
