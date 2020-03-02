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

    tcpClientControl *client = new tcpClientControl(this,socketDescriptor);
    connect(client,SIGNAL(disconnected()),this,SLOT(incomingDisconnection()));
    connect(client,SIGNAL(sendDataToFunction(QString,QString)),this,SLOT(sendDataToFunction(QString,QString)));
    connect(client,SIGNAL(sendDataToMacs(QStringList,QString)),this,SLOT(sendDataToMacs(QStringList,QString)));
    clients.push_back(client);//save client in the clients pool



    qDebug()<<clients.size()<<" devices connected";



}

void tcpSocketServer::incomingDisconnection()
{
    tcpClientControl * client = (tcpClientControl*)QObject::sender();
    clients.erase(std::remove(clients.begin(), clients.end(), client), clients.end());
    qDebug()<<clients.size()<<" devices connected";
}



//send data to max 1 client which mac address is macAddress
void tcpSocketServer::sendDataToMac(QString macAddress,QString data)
{
    for(auto client:clients)
    {
        if(client->getMacAddress() == macAddress)
        {
            client->sendData(data);
            return;
        }
    }
    //if we reached this, data did not go
    qDebug()<<"Error, no client with mac address="<<macAddress;
}


//send data to all clients which mac address is macAddresses
void tcpSocketServer::sendDataToMacs(QStringList macAddresses,QString data)
{
    for(auto client:clients)
    {
        if(macAddresses.contains(client->getMacAddress()))
            client->sendData(data);
    }

}

//send data to any client which function is "function"
void tcpSocketServer::sendDataToFunction(QString function,QString data)
{

    for(auto client:clients)
    {
        if(client->getFunction() == function)
        {
            client->sendData(data);
        }
    }
}


