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



void Server::sendMessage(qint8 type,Client sender)
{
    QByteArray data;
    for (auto client:clients) {
        QByteArray data;

        QHostAddress sendToAddress;
        quint16 sendToPort;

        QHostAddress dataAddress;
        quint16 dataPort;
        QString dataName;


        if(client.isOnline() && client.m_name != sender.m_name){
            sendToAddress = client.m_clientAddress;
            sendToPort = client.m_clientPort;

            dataAddress = sender.m_clientAddress;
            dataPort = sender.m_clientPort;
            dataName = sender.m_name;

            data.append(type);

        }else if(type != 8 && client.m_name != sender.m_name) {
            sendToAddress = sender.m_clientAddress;
            sendToPort = sender.m_clientPort;

            dataAddress = client.m_clientAddress;
            dataPort = client.m_clientPort;
            dataName = client.m_name;


            data.append('1');

        }else {
            continue;
        }
        data.append(dataAddress.toString());
        data.append('|');
        data.append(QString::number(dataPort));
        data.append('|');
        data.append(dataName);

        qDebug()<<"send:"<< data;

        udpSocket->writeDatagram(data,sendToAddress,sendToPort);


    }

}

void Server::readMessage()
{
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray message {datagram.data()};
        qint8 type;

        type = message.mid(0,1).toInt();

        message.remove(0,1);

        QString clientName {message};

        QHostAddress clientAddress {datagram.senderAddress()};
        quint16 clientPort = datagram.senderPort();

        Client client{clientAddress,clientPort,clientName};

        qDebug()<<"ip:" << datagram.senderAddress();
        qDebug()<<"port:" << datagram.senderPort();
        qDebug()<<"type:" << type;
        qDebug()<<"message:" << message;
        qDebug()<<"clientName:" << clientName;

        //refactor !!!!!!!!! this
        auto oldInfo = std::find_if(clients.begin(),clients.end(),
                                    [&clientName] (Client const& c)
        {return (c.m_name == clientName) ; });

        if(type == 8){
            if(oldInfo->isOnline()&&
                    oldInfo->m_clientAddress == clientAddress &&
                    oldInfo->m_clientPort == clientPort){
                //check online status
                oldInfo->setOnline();
                //notify other clients (went online)
                sendMessage('8',client);
            }
            continue;
        }

        if(oldInfo == clients.end()){
            sendMessage('0',client);
            clients.push_back(client);
            continue;
        }
        if(!oldInfo->isOnline()){
            //check online status
            if(oldInfo->m_clientPort != clientPort || oldInfo->m_clientAddress != clientAddress){
                oldInfo->m_clientPort = clientPort;
                oldInfo->m_clientAddress = clientAddress;

                //notify other clients (info changed)
                sendMessage('7',client);

            }else {
                //notify other clients (went online)
                sendMessage('8',client);
            }
            oldInfo->setOnline();

        }

    }
}

