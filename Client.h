#ifndef CLIENT_H
#define CLIENT_H
#include <QHostAddress>
#include <QString>


struct Client
{
    Client(QHostAddress, quint16, QString);
    Client(Client&& );
    Client(const Client&);

    QHostAddress m_clientAddress;
    quint16 m_clientPort;
    QString m_name;

    void setOnline(){online = !online;}
    bool isOnline() const {return online;}

private:
    bool online;


};

#endif // CLIENT_H
