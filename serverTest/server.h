#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>

class Server :public QTcpServer
{
    Q_OBJECT

public:
    Server();
    QTcpSocket *socket;
private:
    QVector <QTcpSocket*> Sockets;
    QByteArray Data;
//    void SendToClient;
public slots:

};

#endif // SERVER_H
