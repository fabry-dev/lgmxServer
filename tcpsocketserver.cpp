#include "tcpsocketserver.h"


tcpSocketServer::tcpSocketServer(QObject *parent,QString ip,qint64 port) : QTcpServer(parent)
{























    if(listen(QHostAddress(ip),port))
    {
        qDebug()<<"tcp server running on "<<serverAddress().toString()<<":"<<serverPort();

    }
    else
    {
        qDebug()<<"tcp server not connected";
    }
}



void tcpSocketServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"connection";


    QThread *clientThread = new QThread;

    tcpClient * client = new tcpClient(NULL,socketDescriptor);
    client->moveToThread(clientThread);

    connect(clientThread,SIGNAL(started()),client,SLOT(run()));
    connect(client,SIGNAL(disconnected()),this,SLOT(incomingDisconnection()));
    //cleanup connections
    connect(client,SIGNAL(disconnected()),clientThread,SLOT(quit()));//stop thread when client is deleted
    connect(clientThread,SIGNAL(finished()),client,SLOT(deleteLater()));//delete client when client thread is stopped
    connect(client,SIGNAL(destroyed()),clientThread,SLOT(deleteLater()));//delete thread when client is deleted

    clients.push_back(client);//save client in the clients pool
    clientThread->start();

    qDebug()<<clients.size()<<" devices connected";



}


void tcpSocketServer::threadDestroyed(void)
{

    qDebug()<<"thread destroyed";
}

void tcpSocketServer::incomingDisconnection()
{
    tcpClient * client = (tcpClient*)QObject::sender();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    qDebug()<<clients.size()<<" devices connected";
}





void tcpSocketServer::testRead(void)
{
    QTcpSocket *clientConnection = (QTcpSocket *)QObject::sender();
    QDataStream in;
    in.setDevice(clientConnection);
    in.setVersion(QDataStream::Qt_5_9);
    in.startTransaction();

    QString data;
    in >> data;

    if (!in.commitTransaction())
    {
        qDebug()<<"non commit transaction";
        //return;
    }

    qDebug()<<clientConnection->localAddress().toString()<<" >> "<<data;

}





void tcpSocketServer::broadcastData(QString data)
{


}

void tcpSocketServer::ping(void)
{

    broadcastData("ping");

}





