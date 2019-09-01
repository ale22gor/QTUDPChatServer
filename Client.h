#ifndef CLIENT_H
#define CLIENT_H
#include <QHostAddress>
#include <QString>


struct Client
{
    Client(QHostAddress, quint16, QString);

    QHostAddress m_clientAddress;
    quint16 m_clientPort;
    QString m_name;

};

#endif // CLIENT_H
