#include <QCoreApplication>
#include "tcpsocketserver.h"
#include "QFile"

#define PATH_DEFAULT (QString)"/home/fred/Dropbox/Taf/lgmx/raspiServer/"

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);


    QString PATH,IP,PORT;
    IP="";
    PORT="";
    QStringList params = a.arguments();
    if(params.size()>1)
        PATH = params[1];
    else
        PATH=PATH_DEFAULT;



    QFile file(PATH+"config.cfg");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug()<<"no config file";

    }
    else
    {

        QTextStream in(&file);

        QString  line;
        QString paramName,paramValue;
        QStringList params;


        while(!in.atEnd()) {
            line = in.readLine();
            line = (line.split("#"))[0];
            params = line.split("=");
            if(params.size()>=2)
            {
                paramName = params[0];
                paramValue = params[1];

                if (paramName=="IP")
                    IP = paramValue;
                else if (paramName=="PORT")
                    PORT = paramValue;
                else
                    qDebug()<<paramName<<" - "<<paramValue;
            }
        }
        file.close();

    }

  new tcpSocketServer(NULL,IP,PORT.toInt());

    return a.exec();
}
