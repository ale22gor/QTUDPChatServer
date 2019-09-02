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

void Server::sendMessage(qint8 type,QByteArray message)
{
    QByteArray data;
    data.append(type);
    data.append(message);

    qDebug()<<"send:"<< data;


    for (auto client:clients) {
        udpSocket->writeDatagram(data,client.m_clientAddress,client.m_clientPort);
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

        qDebug()<<"ip:" << datagram.senderAddress();
        qDebug()<<"port:" << datagram.senderPort();
        qDebug()<<"type:" << type;
        qDebug()<<"message:" << message;

        if(type == 0 || type == 8){

            QHostAddress clientAddress {datagram.senderAddress()};
            quint16 clientPort = datagram.senderPort();
            QString clientName {message};

            Client client{clientAddress,clientPort,clientName};
            qDebug()<<"clientName:" << clientName;


            //refactor !!!!!!!!! this
            auto oldInfo = std::find_if(clients.begin(),clients.end(),
                                        [&clientName] (Client const& c)
            {return (c.m_name == clientName) ; });

            QByteArray data;

            data.append(clientAddress.toString());
            data.append('|');
            data.append(QString::number(clientPort));
            data.append('|');
            data.append(message);


            if(oldInfo != clients.end()){
                if(type != 8 && oldInfo->m_clientPort != clientPort || oldInfo->m_clientAddress != clientAddress){
                    oldInfo->m_clientPort = clientPort;
                    oldInfo->m_clientAddress = clientAddress;

                    //notify other clients (info changed)
                    sendMessage('7',data);

                }else {
                    //check online status
                    oldInfo->setOnline();
                    //notify other clients (went online)
                    sendMessage('8',data);
                }

            }else{

                sendMessage('0',data);
                clients.push_back(client);
            }
        }
    }
}

