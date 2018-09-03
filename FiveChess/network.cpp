#include "network.h"
#include "packetprocessor.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

NetWork::NetWork(QObject *parent) : QObject(parent)
{
    m_isConn = false;

    QFile file(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/FiveChess/config.cfg");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString str(line);
        str = str.trimmed();
        QStringList strlist = str.split(":");
        if(strlist.at(0) == "ip")
        {
            m_ip = strlist.at(1);
        }
        else if(strlist.at(0) == "port")
        {
            str = strlist.at(1);
            m_port = str.toInt();
        }
    }
    m_tcpSock = new QTcpSocket();
    connect(m_tcpSock, SIGNAL(readyRead()), this, SLOT(slot_ReadData()));
    connect(m_tcpSock, SIGNAL(error(QAbstractSocket::SocketError)), \
            this, SLOT(slot_ReadError(QAbstractSocket::SocketError)));
}

NetWork::~NetWork()
{
    if(m_tcpSock)
    {
        m_tcpSock->abort();
        delete m_tcpSock;
    }
    m_isConn = false;
}

NetWork* NetWork::GetNetWork()
{
    static NetWork* network;
    if(network)
        return network;
    else
        return network = new NetWork();
}

void NetWork::WriteData(const char *data, int size)
{
    m_tcpSock->write(data, size);
}

bool NetWork::slot_Connect()
{
    m_tcpSock->connectToHost(m_ip, m_port);
    if (m_tcpSock->waitForConnected(2000))
    {
        m_isConn = true;
        qDebug() << "Connected to Server!";
    }
    return m_isConn;
}

void NetWork::slot_ReadData()
{
    PacketProcessor::GetPacketProcessor()->HandlePacket(m_tcpSock->readAll());
}

void NetWork::slot_ReadError(QAbstractSocket::SocketError)
{
    qDebug() << "slot_ReadError";
}
