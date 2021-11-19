#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QSaveFile>
#include <QFileInfo>
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>
#include <time.h>
#include <QTcpSocket>
#include "globaldatadefinitions.h"

#define	INTERVAL		(double)5.00
#define FileServer		"172.20.4.6"
#define Port			445
#define TimeOut			100
#define NetDirectory	"/media/ARCHE/"
#define LocalDirectory	"/home/pi/Documents/Logs/"


struct LogStatus{
	bool loggingLocal=false;
	bool loggingNet=false;
	bool localInitFailed=true ;
	bool localFirstWrite=false;
	bool netInitFailed=true;
	bool netFirstWrite=false;
	bool firstTime=true;
	time_t lastWrite;

	LogStatus(){
		this->loggingNet=false;
		this->loggingLocal=false;
		this->localInitFailed=false;
		this->localFirstWrite=true;
		this->netFirstWrite=false;
		this->netInitFailed=true;
		this->firstTime=true;
	}

	void LoggingNet(){
		this->netFirstWrite=true;
		this->netInitFailed=false;
		this->loggingNet=true;
	}

	void LoggingLocal(){
		this->localFirstWrite=true;
		this->localInitFailed=false;
		this->loggingLocal=true;
	}

	void InitFailed(){
		this->localInitFailed=true;
		this->netInitFailed=true;
		this->netFirstWrite=false;
		this->localFirstWrite=false;
		this->loggingLocal=false;
		this->loggingNet=false;
	}

	bool networkConnected(){
		QTcpSocket* socket=new QTcpSocket();
		socket->connectToHost(FileServer,Port);
		bool connected=socket->waitForConnected();
		socket->close();
		return connected;
	}

	bool canLog(){
		if(this->firstTime){
			this->firstTime=false;
			time(&this->lastWrite);
			return true;
		}else{			
			double diff=difftime(time(nullptr),this->lastWrite);
			if(diff>=INTERVAL){
				time(&this->lastWrite);
				return true;
			}else{
				return false;
			}
		}
	}
};

class Logger:public QObject
{
	Q_OBJECT
	public:
		explicit Logger(QObject *parent = nullptr);
		~Logger();
		//bool isRunning();
	signals:
		void publishFileError(const QString &error);
        void publishMessage(const QString& message);
	public slots:
		void InitLogger(const QString& filename);
		void writeOutData(const QString &data);
	private:
		bool logNet(const QString& data);
		bool logLocal(const QString& data);
		QString fullNetFilePath;
		QString filename;
		QString fullLocalFilePath;
		const QString headers="Date,System Time,RunTime,V11,V12,V21,V22,V31,V32,i11,i12,i21,i22,i31,i32,Temp1,Temp2,Temp3,CurrentSetpoint(mA)";
		LogStatus logStatus;
        bool networkErrorLatch;
        bool fCopyErrorLatch;
		const double logTime=1000.00;
};

#endif // LOGGER_H
