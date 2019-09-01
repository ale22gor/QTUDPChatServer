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
    explicit Server(QObject *parent = nullptr);
    void sendMessage(QByteArray);


signals:

public slots:
    void readMessage();

private:
    QUdpSocket *udpSocket;
    std::list<Client> clients;
};

#endif // SERVER_H
