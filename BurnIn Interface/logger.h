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
#include "globaldatadefinitions.h"



class Logger:public QObject
{
	Q_OBJECT
	public:
		explicit Logger(QObject *parent = nullptr);
		~Logger();
		bool isRunning();
	signals:
		void publishFileError(const QString &error);

	public slots:
		void log();
		void recieveData(const QString &line);
		void startLogging(const QString &filename);
		void stopLogging();
		void continueLogging();
		void pauseLogging();
	private:
		void writeOutData(const QString &data);
		QTimer *logTimer=nullptr;
		QFile *file=nullptr;
		int logTime=1000;
		const QString localDir="/home/pi/Documents/Logs/";
		const QString netDirectory="/home/pi/Documents/ARCHE/";
		const QString headers="Date,System Time,RunTime,V11,V12,V21,V22,V31,V32,Temp1,Temp2,Temp3,CurrentSetpoint(mA)";
		QString lastUpdate;
		std::atomic<bool> loggerRunning;
		std::atomic<bool> paused;
};

#endif // LOGGER_H
