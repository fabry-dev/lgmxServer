#include <QCoreApplication>


#include "tcpsocketserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);



    tcpSocketServer *ts = new tcpSocketServer(NULL);


    return a.exec();
}
