#include "Client.h"

Client::Client(QHostAddress clientAddress, quint16 clientPort, QString name):
    m_clientAddress{clientAddress},m_clientPort{clientPort},m_name{name},online{true}
{}
