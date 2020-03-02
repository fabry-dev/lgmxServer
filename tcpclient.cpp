#include "tcpclient.h"


tcpClient::tcpClient(QObject *parent, int socketDescriptor):QObject(parent),socketDescriptor(socketDescriptor)
{


}



tcpClient::~tcpClient()
{
    // qDebug()<<"client destroyed";
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
    macAddress = resolveMacAddress(&test);


    qDebug()<<"ip: "<<tcpSocket->peerAddress().toString();
    if(test)
        qDebug()<<"mac: "<<macAddress;
    else
        qDebug()<<"mac address error";




    //exec();//event loop
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

    clientFunction = new controllerClient(this);
}


void tcpClient::makeClientDevice()
{
    if(clientFunction)
    {
        qDebug()<<"client already has a function, error.";
        return;
    }

    clientFunction = new deviceClient(this);
}

QString tcpClient::getMacAddress(void)
{
    return macAddress;
}

QString tcpClient::resolveMacAddress(bool *success)
{

    QProcess process;
    QString cmd = (QString)"arp -n "+"192.168.1.39";
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
