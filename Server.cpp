#include "Server.h"
#include <QNetworkDatagram>
#include <QDebug>
#include <algorithm>
#include <QDataStream>
#include <QNetworkInterface>

enum  MessageTypes{ HelloOnline, HelloOffline , Message , Ping ,
                    SendPingTo ,SomeFeature , AnotherFeature ,
                    UpdateInfo , OffOnLine , ExitChat };

Server::Server(quint16 port,QObject *parent) : QObject(parent)
{
    udpSocket = new QUdpSocket(this);

    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    QHostAddress localIpAddress;
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            localIpAddress = address;
    }

    udpSocket->bind(localIpAddress, port);
    connect(udpSocket, SIGNAL(readyRead()),this, SLOT(readMessage()));
}



void Server::sendMessage(const qint8 type, const Client sender)
{
    QByteArray data;
    for (auto client:clients) {
        QByteArray dataToClient;
        QByteArray dataBackToSender;


        QHostAddress sendToAddress;
        quint16 sendToPort;

        QHostAddress dataAddress;
        quint16 dataPort;
        QString dataName;

        if(client.m_name == sender.m_name)
            continue;

        if(client.isOnline()){
            sendToAddress = client.m_clientAddress;
            sendToPort = client.m_clientPort;

            dataAddress = sender.m_clientAddress;
            dataPort = sender.m_clientPort;
            dataName = sender.m_name;

            data.append(type);

            data.append(dataAddress.toString());
            data.append('|');
            data.append(QString::number(dataPort));
            data.append('|');

            data.append(dataName);

            udpSocket->writeDatagram(data,sendToAddress,sendToPort);


        }

        if(!sender.isOnline())
            continue;

        sendToAddress = sender.m_clientAddress;
        sendToPort = sender.m_clientPort;

        dataAddress = client.m_clientAddress;
        dataPort = client.m_clientPort;
        dataName = client.m_name;

        if(client.isOnline())
            dataBackToSender.append(HelloOnline);
        else
            dataBackToSender.append(HelloOffline);

        dataBackToSender.append(dataAddress.toString());
        dataBackToSender.append('|');
        dataBackToSender.append(QString::number(dataPort));
        dataBackToSender.append('|');

        dataBackToSender.append(dataName);

        udpSocket->writeDatagram(dataBackToSender,sendToAddress,sendToPort);



    }

}



void Server::readMessage()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray message {datagram.data()};
        qint8 type;

        QDataStream dataStream(message);
        dataStream >> type;
        message.remove(0,1);

        if ( type == Ping) {
            continue;
        }

        QString clientName {message};

        QHostAddress clientAddress {datagram.senderAddress()};
        quint16 clientPort = datagram.senderPort();

        Client client{clientAddress,clientPort,clientName};

        qDebug()<<"ip:" << datagram.senderAddress();
        qDebug()<<"port:" << datagram.senderPort();
        qDebug()<<"type:" << type;
        qDebug()<<"message:" << message;
        qDebug()<<"clientName:" << clientName;

        auto oldInfo = std::find_if(clients.begin(),clients.end(),
                                    [&clientName] (Client const& c)
        {return (c.m_name == clientName) ; });

        switch (type) {

        case HelloOnline:{

            if(oldInfo == clients.end()){
                sendMessage(HelloOnline,client);
                clients.push_back(client);

            }else if(!oldInfo->isOnline()){
                oldInfo->setOnline();
                if(oldInfo->m_clientPort != clientPort || oldInfo->m_clientAddress != clientAddress){
                    oldInfo->m_clientPort = clientPort;
                    oldInfo->m_clientAddress = clientAddress;

                    sendMessage(UpdateInfo,client);

                }else
                    sendMessage(OffOnLine,client);
            }
            break;
        }
        case OffOnLine:{
            if(oldInfo->isOnline()&&
                    oldInfo->m_clientAddress == clientAddress &&
                    oldInfo->m_clientPort == clientPort){
                oldInfo->setOnline();
                sendMessage(OffOnLine,client);
            }
            break;
        }
        }
    }
}

