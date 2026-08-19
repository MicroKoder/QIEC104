#include "iec104driver.h"
#include <QMessageBox>
#include <QDebug>
#include <QTime>
#include <QList>

static char testAct[] = {0x68, 0x04, 0x43, 0x00, 0x00, 0x00};
static char testFrCon[] = {0x68, 0x04,(char)0x83, 0x00, 0x00, 0x00};
static char startCon[] = {0x68, 0x04, 0x0B, 0x00, 0x00, 0x00};
static char StartDTAct[] ={0x68, 0x04, 0x07, 0x00, 0x00, 0x00};

IEC104Driver* IEC104Driver::instance = NULL;

quint16 IEC104Driver::SeqDiff(quint16 a, quint16 b)
{
    return (a - b) & 0x7FFF;
}

IEC104Driver::IEC104Driver():
    sock(new QTcpSocket(this))
{
    N_R = 0;
    N_T = 0;
    lastConfirmed = 0;
    peerAcked = 0;

    testTimer = new QTimer(this);
    testTimer->setSingleShot(false);
    connect(testTimer, SIGNAL(timeout()),this,SLOT(OnTestTimer()));

    conTimer = new QTimer(this);
    conTimer->setSingleShot(true);
    connect(conTimer, SIGNAL(timeout()),this,SLOT(OnConnectionTimer()));

    t2Timer = new QTimer(this);
    t2Timer->setSingleShot(true);
    connect(t2Timer, SIGNAL(timeout()), this, SLOT(OnT2Timer()));

    connect(sock,SIGNAL(connected()),this,SLOT(OnConnected()));
    connect(sock,SIGNAL(disconnected()),this, SLOT(OnDisconnected()));
    connect(sock,SIGNAL(readyRead()), this, SLOT(OnSockReadyRead()));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(sock, &QAbstractSocket::errorOccurred,
            this, &IEC104Driver::displayError);
#else
    connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
#endif
}

void IEC104Driver::OnConnectionTimer()
{
    emit Message(tr("No answer from server, t1 elapsed"));
    if (sock->state() == QAbstractSocket::ConnectingState)
        sock->abort();
    OnDisconnected();
}

void IEC104Driver::OnT2Timer()
{
    if (SeqDiff(N_R, lastConfirmed) > 0)
        Send_ConfirmPacks();
}

bool IEC104Driver::CanSendIFormat() const
{
    if (!settings)
        return false;
    return SeqDiff(N_T, peerAcked) < settings->k;
}

bool IEC104Driver::SendIFormat(const QByteArray &buf, const QString &logMessage)
{
    if (sock->state() != QTcpSocket::ConnectedState)
        return false;

    if (!CanSendIFormat())
    {
        emit Message(tr("Send blocked: k window full (%1 unacked)")
                     .arg(SeqDiff(N_T, peerAcked)));
        return false;
    }

    sock->write(buf);
    emit Message(logMessage);
    N_T = (N_T + 1) & 0x7FFF;

    // Waiting for peer ACK — keep t1 alive
    if (settings)
    {
        conTimer->stop();
        conTimer->start(settings->t1 * 1000);
    }
    return true;
}

void IEC104Driver::UpdatePeerAck(quint16 nr)
{
    peerAcked = nr & 0x7FFF;
}

void IEC104Driver::ScheduleConfirm()
{
    if (!settings)
        return;

    const quint16 pending = SeqDiff(N_R, lastConfirmed);
    if (pending == 0)
    {
        t2Timer->stop();
        return;
    }

    if (pending >= settings->w)
    {
        Send_ConfirmPacks();
        return;
    }

    // Delayed confirm via t2
    t2Timer->start(settings->t2 * 1000);
}

void IEC104Driver::SendFullRequest(quint8 requestDescription)
{
    if (!settings)
        return;

    quint16 ASDU = settings->asdu;

    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   0x64, 0x01,
                   0x06,0x00,
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   0x00,0x00,0x00,
                   char(requestDescription)
                  };

    QByteArray buf = QByteArray(temp, 16);
    SendIFormat(buf, QStringLiteral("<-- Команда опроса"));
}

void IEC104Driver::SendRequestCounter()
{
}

void IEC104Driver::SendRequestSingle()
{
}

void IEC104Driver::OnTestTimer()
{
    if (SeqDiff(N_R, lastConfirmed) > 0)
        Send_ConfirmPacks();
    else
        SendTestAct();
}

void IEC104Driver::SendTestAct()
{
    QByteArray buf = QByteArray(testAct, 6);
    if (sock->state() == QTcpSocket::ConnectedState){
        sock->write(buf,6);
        emit Message("<-- TestAct");
    }
}

void IEC104Driver::SendTestCon()
{
    QByteArray buf = QByteArray(testFrCon, 6);
    if (sock->state() == QTcpSocket::ConnectedState){
        sock->write(buf,6);
        emit Message("<-- TestCon");
    }
}

void IEC104Driver::Send_ConfirmPacks()
{
    if (sock->state() != QTcpSocket::ConnectedState)
        return;

    t2Timer->stop();

    char pack[] ={0x68, 0x04, 0x01, 0x00, char(N_R<<1),char(N_R>>7)};
    sock->write(pack, 6);
    lastConfirmed = N_R;
    emit Message(IEC104Tools::BytesToString(pack,6));
    emit Message("<-- Confirm");
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
    for (int i=0; i<6; i++)
        if (data[i] != testAct[i])
            return false;
    return true;
}

bool IEC104Driver::isTestCon(QByteArray data)
{
    for (int i=0; i<6; i++)
        if (data[i] != testFrCon[i])
            return false;
    return true;
}

IEC104Driver* IEC104Driver::GetInstance()
{
    if (IEC104Driver::instance==NULL)
        IEC104Driver::instance = new IEC104Driver();
    return IEC104Driver::instance;
}

void IEC104Driver::SetSettings(QSettings *settings)
{
    if (settings != NULL)
    {
        settings->beginGroup("driver");
        if (this->settings)
            delete this->settings;

        this->settings = new CSetting(
            settings->value("ip").toString(),
            settings->value("asdu").toInt(),
            settings->value("port").toInt()
        );
        this->settings->t1 = settings->value("t1", 15).toInt();
        this->settings->t2 = settings->value("t2", 10).toInt();
        this->settings->t3 = settings->value("t3", 5).toInt();
        this->settings->k = settings->value("k", 12).toInt();
        this->settings->w = settings->value("w", 8).toInt();
        this->settings->SendGIOnStart = settings->value("SendGIOnStart").toBool();
        this->settings->SendTCOnStart = settings->value("SendTCOnStart").toBool();
        this->settings->autoReconnect = settings->value("autoReconnect").toBool();

        // Enforce t2 < t1
        if (this->settings->t2 >= this->settings->t1 && this->settings->t1 > 1)
            this->settings->t2 = this->settings->t1 - 1;
        if (this->settings->k == 0)
            this->settings->k = 12;
        if (this->settings->w == 0)
            this->settings->w = 8;

        settings->endGroup();
    }
}

CSetting* IEC104Driver::GetSettings()
{
    return settings;
}

void IEC104Driver::OpenConnection(/*CSetting *_settings*/)
{
    if (sock->state() != QAbstractSocket::UnconnectedState)
        return;

    if (settings != nullptr)
    {
        emit Connecting();
        emit Message("Connecting...");
        qDebug()<< settings->IP;
        sock->connectToHost(settings->IP,settings->Port);

        testTimer->setInterval(settings->t3*1000);

        N_R = 0;
        N_T = 0;
        lastConfirmed = 0;
        peerAcked = 0;
        rxBuffer.clear();
        IEC104Tools::ClearLostBytes();

        conTimer->setInterval(settings->t1*1000);
        conTimer->start();

        needSendGI = settings->SendGIOnStart;
        needSendTC = settings->SendTCOnStart;
    }
}

void IEC104Driver::CloseConnection()
{
    if (settings)
        settings->autoReconnect = false;
    qDebug() <<"sock state: " << sock->state();
    switch(sock->state())
    {
    case QAbstractSocket::UnconnectedState: break;
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
        sock->abort();
        conTimer->stop();
        OnDisconnected();
        break;
    case QAbstractSocket::ConnectedState:
        sock->disconnectFromHost();
        break;
    case QAbstractSocket::BoundState: break;
    case QAbstractSocket::ListeningState: break;
    case QAbstractSocket::ClosingState: break;
    }
}

void IEC104Driver::SendCommand(quint16 type, quint32 ioa, quint8 value)
{
    if (!settings)
        return;

    qDebug() << "sending command "<< type << " " << ioa << " "<< value ;
    quint16 ASDU = settings->asdu;

    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   char(type), 0x01,
                   0x06,0x00,
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                   char(value)
                  };

    QByteArray buf = QByteArray(temp, 16);

    if (type>=58)
        buf.append(CP56Time::GetTimestamp());

    buf[1] = (char)buf.length()-2;

    if (SendIFormat(buf, tr("<-- command")))
        emit Message(IEC104Tools::BytesToString(&buf));
}

void IEC104Driver::SetPoint(quint16 type, quint32 ioa, QVariant value)
{
    if (!settings)
        return;

    quint16 ASDU = settings->asdu;

    if (type == 48 || type == 61)
    {
        quint16 uvalue = value.toUInt();
        char temp[] = {0x68, 0xE,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,
                       char(ASDU&0xFF), char(ASDU>>8),
                       char(ioa&0xFF),char((ioa)>>8),char((ioa)>>16),
                       char(uvalue&0xff),char((uvalue>>8)&0xff),char(0x00)
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (type==61)
            buf.append(CP56Time::GetTimestamp());
        buf[1] = (char)buf.length()-2;
        if (SendIFormat(buf, tr("<-- Set point command, normalized value")))
            emit Message(IEC104Tools::BytesToString(&buf));
        return;
    }
    if (type == 49 || type == 62)
    {
        int ivalue = value.toInt();
        char temp[] = {0x68, 0xE,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       char(ivalue&0xff),char((ivalue>>8)&0xff),0x00
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (type==62)
            buf.append(CP56Time::GetTimestamp());
        buf[1] = (char)buf.length()-2;
        if (SendIFormat(buf, tr("<-- Set point command, scaled value")))
            emit Message(IEC104Tools::BytesToString(&buf));
        return;
    }
    if (type == 50 || type == 63)
    {
        float fvalue = value.toFloat();
        QByteArray bytes(reinterpret_cast<const char*>(&fvalue), sizeof(fvalue));
        char temp[] = {0x68, 0x12,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       bytes[0],bytes[1],bytes[2],bytes[3],0x00
                      };

        QByteArray buf = QByteArray(temp, 20);
        if (type==63)
            buf.append(CP56Time::GetTimestamp());
        buf[1] = (char)buf.length()-2;
        if (SendIFormat(buf, tr("<-- Set point command with floating point value")))
            emit Message(IEC104Tools::BytesToString(&buf));
        return;
    }
    if (type == 51 || type == 64)
    {
        uint dvalue = value.toUInt();
        char temp[] = {0x68, 0x12,
                       char(N_T<<1), char(N_T>>7),
                       char(N_R<<1), char(N_R>>7),
                       char(type), 0x01,
                       0x06,0x00,
                       char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                       char(ioa&0xFF),char(((ioa)>>8)&0xff),char(((ioa)>>16)&0xff),
                       char(dvalue&0xff),char((dvalue>>8)&0xff),char((dvalue>>16)&0xff),char((dvalue>>24)&0xff),char(0x00)
                      };

        QByteArray buf = QByteArray(temp, sizeof(temp));
        if (type==64)
            buf.append(CP56Time::GetTimestamp());
        buf[1] = (char)buf.length()-2;
        if (SendIFormat(buf, tr("<-- Set point command, bit string 32 bit")))
            emit Message(IEC104Tools::BytesToString(&buf));
        return;
    }
}

void IEC104Driver::OnConnected()
{
    SendStart();
    testTimer->start();
    emit Connected();
}

void IEC104Driver::OnDisconnected()
{
    emit Message(tr("disconnected"));
    sock->close();
    testTimer->stop();
    conTimer->stop();
    t2Timer->stop();
    rxBuffer.clear();
    if (settings && settings->autoReconnect)
        OpenConnection();
    else
        emit Disconnected();
}

void IEC104Driver::ProcessAPDU(const QByteArray &apdu)
{
    if (apdu.size() < 6 || (uchar)apdu[0] != 0x68)
        return;

    const uchar ctrl = (uchar)apdu[2];

    // U-format: bits 0..1 == 11
    if (apdu.size() == 6 && (ctrl & 0x03) == 0x03)
    {
        if (isTestAct(apdu))
        {
            emit Message("--> TestAct");
            SendTestCon();
            return;
        }
        if (isTestCon(apdu))
        {
            emit Message("--> TestCon");
            return;
        }
        if (isStartCon(apdu))
        {
            emit Message("--> StartCon");
            if (needSendGI)
            {
                needSendGI = false;
                SendFullRequest(20);
            }
            if (needSendTC)
            {
                needSendTC = false;
                ClockSynch();
            }
            return;
        }
        return;
    }

    // S-format: bit0 == 1, bit1 == 0
    if (apdu.size() == 6 && (ctrl & 0x03) == 0x01)
    {
        const quint16 nr = (((uchar)apdu[4] >> 1) + (((uchar)apdu[5]) << 7)) & 0x7FFF;
        UpdatePeerAck(nr);
        emit Message(QStringLiteral("--> S-format ACK N(R)=%1").arg(nr));
        return;
    }

    // I-format: bit0 == 0
    if ((ctrl & 0x01) == 0 && apdu.size() > 6)
    {
        const quint16 ns = (((uchar)apdu[2] >> 1) + (((uchar)apdu[3]) << 7)) & 0x7FFF;
        const quint16 nr = (((uchar)apdu[4] >> 1) + (((uchar)apdu[5]) << 7)) & 0x7FFF;
        UpdatePeerAck(nr);

        // Next expected receive sequence
        N_R = (ns + 1) & 0x7FFF;

        QByteArray frame = apdu;
        IEC104Tools::ClearLostBytes();
        QList<CIECSignal>* s = IEC104Tools::ParseData(frame, nullptr);
        if (s != NULL)
        {
            if (apdu.size() > 7)
            {
                const bool isSequence = (apdu[7] & 0x80) > 0;
                emit Message("SQ= " + QString::number(isSequence ? 1 : 0)
                             + QString("; count: ") + QString::number(s->length()));
            }
            foreach (CIECSignal signal, (*s))
            {
                if (settings && signal.ASDU == settings->asdu)
                {
                    emit Message(signal.GetValueString());
                    emit IECSignalReceived(signal);
                }
            }
            delete s;
        }

        ScheduleConfirm();
    }
}

void IEC104Driver::OnSockReadyRead()
{
    QByteArray chunk = sock->readAll();
    emit Message(QTime::currentTime().toString() + " -->" + IEC104Tools::BytesToString(&chunk));

    testTimer->stop();
    testTimer->start();

    conTimer->stop();
    if (settings)
        conTimer->start(settings->t1 * 1000);

    rxBuffer.append(chunk);

    while (rxBuffer.size() >= 2)
    {
        // Resync to start-of-frame
        if ((uchar)rxBuffer[0] != 0x68)
        {
            int pos = rxBuffer.indexOf(char(0x68));
            if (pos < 0)
            {
                rxBuffer.clear();
                break;
            }
            rxBuffer.remove(0, pos);
            if (rxBuffer.size() < 2)
                break;
        }

        const int apduLen = (uchar)rxBuffer[1] + 2;
        if (apduLen < 6)
        {
            rxBuffer.remove(0, 1);
            continue;
        }
        if (rxBuffer.size() < apduLen)
            break; // wait for more TCP data

        const QByteArray apdu = rxBuffer.left(apduLen);
        rxBuffer.remove(0, apduLen);
        ProcessAPDU(apdu);
    }
}

void IEC104Driver::displayError(QAbstractSocket::SocketError)
{
    emit Message(tr("Socket error: %1").arg(sock->errorString()));
    OnDisconnected();
}

void IEC104Driver::ClockSynch()
{
    if (!settings)
        return;

    quint16 ASDU = settings->asdu;

    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   0x67, 0x01,
                   0x06,0x00,
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   0x00,0x00,0x00,
                  };

    QByteArray buf = QByteArray(temp, sizeof(temp));
    buf.append(CP56Time::GetTimestamp());
    buf[1] = buf.length()-2;
    SendIFormat(buf, tr("<-- Time sync command"));
}

void IEC104Driver::ReadIOA(quint32 ioa)
{
    if (!settings)
        return;

    quint16 ASDU = settings->asdu;

    char temp[] = {0x68, 0xE,
                   char(N_T<<1), char(N_T>>7),
                   char(N_R<<1), char(N_R>>7),
                   0x66, 0x01,
                   0x05,0x00,
                   char(ASDU&0xFF), char((ASDU>>8)&0xFF),
                   char(ioa&0xff),char((ioa>>8)&0xff),char((ioa>>16)&0xff),
                  };

    QByteArray buf = QByteArray(temp, sizeof(temp));
    buf[1] = buf.length()-2;
    SendIFormat(buf, tr("<-- Read command"));
}
