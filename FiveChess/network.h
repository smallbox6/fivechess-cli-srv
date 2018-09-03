#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>

class NetWork : public QObject
{
    Q_OBJECT
public:
    ~NetWork();
    static NetWork* GetNetWork();
    void WriteData(const char *data, int size);
    bool IsConnected(){return m_isConn;}

private:
    explicit NetWork(QObject *parent = nullptr);

public slots:
    bool slot_Connect();
    void slot_ReadData();
    void slot_ReadError(QAbstractSocket::SocketError);

signals:

private:
    QTcpSocket *m_tcpSock;
    bool m_isConn;
    int m_port;
    QString m_ip;
};

#endif // NETWORK_H
