#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <list>
#include "Client.h"





class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(quint16 port = 3228,QObject *parent = nullptr);
    void sendMessage(qint8, Client);



signals:

public slots:
    void readMessage();

private:
    QUdpSocket *udpSocket;
    std::list<Client> clients;
};

#endif // SERVER_H
