#include "mainwindow.h"
#include <QApplication>
#include <QProcess>



void execute();

int main(int argc, char *argv[])
{
	//execute();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

void execute(){
	QProcess *process=new QProcess();

	process->waitForFinished();
	process->start("stty",QStringList() << "-F" <<"/dev/ttyACM0"<<"-hupcl"<< "echo \"pre\";ps -aux; echo \"post\"");
	qDebug()<<"in usb process"<<endl;
	if(!process->waitForStarted()){
		qWarning()<<"Error starting usb process";
		qDebug()<<"Error starting usb process"<<endl;
	}
	int waitTime=30000;
	if(!process->waitForFinished(waitTime)){
		qWarning()<<"timeout will running usb process";
		qDebug()<<"timeout will running usb process"<<endl;
	}
}
