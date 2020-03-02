#include "tcpclient.h"


tcpClientControl::tcpClientControl(QObject *parent, int socketDescriptor):QObject(parent)
{
    macAddress = "";//initial mac address is empty
    function = "";//initial function is empty

    QThread *clientThread = new QThread;

    client = new tcpClient(NULL,socketDescriptor);
    client->moveToThread(clientThread);
    connect(clientThread,SIGNAL(started()),client,SLOT(run()));
    connect(client,SIGNAL(disconnected()),this,SIGNAL(disconnected()));//forward disconnect signal from client to client control.
    connect(this,SIGNAL(shouldSendData(QString)),client,SLOT(writeData(QString)));//used to trigger data transfer outside of the tcp thread
    connect(client,SIGNAL(solvedMacAddress(QString)),this,SLOT(setMacAddress(QString)));//save the mac address in control when made available
    connect(client,SIGNAL(functionChosen(QString)),this,SLOT(setFunction(QString)));//save the client function in control when made available
    connect(client,SIGNAL(sendDataToFunction(QString,QString)),this,SIGNAL(sendDataToFunction(QString,QString)));
    connect(client,SIGNAL(sendDataToMacs(QStringList,QString)),this,SIGNAL(sendDataToMacs(QStringList,QString)));



    //cleanup connections
    connect(client,SIGNAL(disconnected()),clientThread,SLOT(quit()));//stop thread when client is deleted
    connect(clientThread,SIGNAL(finished()),client,SLOT(deleteLater()));//delete client when client thread is stopped
    connect(client,SIGNAL(destroyed(QObject*)),this,SLOT(deleteLater()));//delete control when client is deleted
    connect(this,SIGNAL(destroyed()),clientThread,SLOT(deleteLater()));//delete thread when control is deleted




    clientThread->start();//go


}




tcpClient::tcpClient(QObject *parent, int socketDescriptor):QObject(parent),socketDescriptor(socketDescriptor)
{

}




void tcpClient::run()
{
    clientFunction = NULL;

    tcpSocket = new QTcpSocket(NULL);
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket->error());
        return;
    }

    connect(tcpSocket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));//let the server know the client disconnected
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readData()));

    bool test = false;
    QString macAddress = resolveMacAddress(&test);

    if(test)
    {
        emit solvedMacAddress(macAddress);
        qDebug()<<"mac address: "<<macAddress;
    }
    else
        qDebug()<<"mac address error";


}

void tcpClient::readData()
{

    QDataStream in;
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_5_9);
    in.startTransaction();

    QString data;
    in >> data;

    if (!in.commitTransaction())
    {
        qDebug()<<"non commit transaction";
        //return;
    }

    handleData(data);

    if(tcpSocket->bytesAvailable())//if we have data left to read, go again.
        readData();

}


void tcpClient::handleData(QString data)
{
    if(data=="controller")
    {
        makeClientController();
        return;
    }
    else if(data=="device")
    {
        makeClientDevice();
        return;
    }


    emit dataReceived(data);
}



void tcpClient::writeData(QString data)
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out<<data;
    tcpSocket->write(block);
    qDebug()<<tcpSocket->localAddress().toString()<<" >> "<<data;
}

void tcpClient::makeClientController()
{
    if(clientFunction)
    {
        qDebug()<<"client already has a function, error.";
        return;
    }

    emit functionChosen("controller");
    clientFunction = new controllerClient(this);
}


void tcpClient::makeClientDevice()
{
    if(clientFunction)
    {
        qDebug()<<"client already has a function, error.";
        return;
    }

    emit functionChosen("device");
    clientFunction = new deviceClient(this);
}


QString tcpClient::resolveMacAddress(bool *success)
{

    QProcess process;
    QString cmd = (QString)"arp -n "+tcpSocket->peerAddress().toString();
    process.start(cmd.toStdString().c_str());
    process.waitForFinished(-1); // will wait forever until finished

    QString stdout = process.readAllStandardOutput();
    QStringList data = stdout.split("\n");

    if(data.size()<2)
    {
        *success = false;
        return "";
    }
    data = ((QString)data[1]).split(" ",QString::SkipEmptyParts);

    if(data.size()<3)
    {
        *success = false;
        return "";
    }
    QString mac = data[2];


    if(mac.size()==17)
    {
        *success = true;
        return mac;
    }
    else
    {
        *success = false;
        qDebug()<<cmd;
        qDebug()<<stdout;
        return "";
    }


}
