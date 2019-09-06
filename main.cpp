#include <QCoreApplication>
#include "Server.h"
#include <QTextStream>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint16 port;

    //QTextStream qtin(stdin);
    //qDebug() << "write port";
    //port = qtin.realNumberNotation();

    Server myServer{3228};

    return a.exec();
}
