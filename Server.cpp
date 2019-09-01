#include "Server.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <algorithm>

Server::Server(QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 3228);
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readMessage()));
}

void Server::sendMessage(QByteArray message)
{
    QNetworkDatagram datagram;
    datagram.setData(message);

    for (auto client:clients) {
        datagram.setDestination(client.m_clientAddress,client.m_clientPort);
        udpSocket->writeDatagram(datagram);
    }

}

void Server::readMessage()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray message  = datagram.data();
        qint8 type;

        type = message.mid(0,1).toInt();
        message.remove(0,1);

        qDebug()<<"ip:" << datagram.senderAddress();
        qDebug()<<"port:" << datagram.senderPort();
        qDebug()<<"type:" << type;
        qDebug()<<"message:" << message;

        if(type == 0){

            QHostAddress clientAddress = datagram.senderAddress();
            quint16 clientPort = datagram.senderPort();
            QString clientName = message.mid(1);

            Client client{clientAddress,clientPort,clientName};

            auto oldInfo = std::find_if(clients.begin(),clients.end(),
                                  [&] (Client const& c)
            {return (c.m_name == clientName) &&
                    (c.m_clientPort != clientPort || c.m_clientAddress != clientAddress); });


            if(oldInfo != clients.end()){
                oldInfo->m_clientPort = clientPort;
                oldInfo->m_clientAddress = clientAddress;

                //send new to other
                //check info
                //update info if changed
                //send new to other

            }else{
                QByteArray data;

                data.append('0');
                data.append(clientAddress.toString());
                data.append('|');
                data.append(QString::number(datagram.senderPort()));
                data.append('|');
                data.append(message);

                qDebug() << data;
                //qDebug() << data.mid(1,4);

                datagram.setData(data);
                sendMessage(data);
                clients.push_back(client);
                //add client
                //send new to other

            }

        }

    }
}

