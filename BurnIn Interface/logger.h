#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QDebug>
#include <time.h>
#include "globaldatadefinitions.h"

#define	INTERVAL	(double)5.00


struct LogStatus{
	bool loggingLocal=false;
	bool loggingNet=false;
	bool initFailed=false;
	bool firstWrite=true;
	time_t lastWrite;

	LogStatus(){
		this->loggingNet=false;
		this->loggingLocal=false;
		this->initFailed=false;
		this->firstWrite=true;
	}

	void LoggingNet(){
		this->firstWrite=true;
		this->initFailed=false;
		this->loggingNet=true;
	}

	void LoggingLocal(){
		this->firstWrite=true;
		this->initFailed=false;
		this->loggingLocal=true;
	}

	void InitFailed(){
		this->initFailed=true;
		this->loggingLocal=false;
		this->loggingNet=false;
	}

	bool canLog(){
		if(this->firstWrite){
			this->firstWrite=false;
			time(&this->lastWrite);
			qDebug()<<"Time start: "<<this->lastWrite<<endl;
			return true;
		}else{
			double diff=difftime(time(nullptr),this->lastWrite);
			qDebug()<<"Time Delta: "<<diff<<endl;
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
	public slots:
		void InitLogger(const QString& filename);
		void writeOutData(const QString &data);
	private:
		bool logNet(const QString& data);
		bool logLocal(const QString& data);
		QString fullNetFilePath;
		QString fullLocalFilePath;
		const QString localDir="/home/pi/Documents/Logs/";
		const QString netDirectory="/home/pi/Documents/ARCHE/";
		const QString headers="Date,System Time,RunTime,V11,V12,V21,V22,V31,V32,Temp1,Temp2,Temp3,CurrentSetpoint(mA)";
		//QString lastUpdate;
		LogStatus logStatus;
		const double logTime=1000.00;
		std::atomic<bool> loggerRunning;
		std::atomic<bool> paused;
};

#endif // LOGGER_H
