#include "logger.h"

Logger::Logger(QObject *parent):QObject(parent){
	//this->logTimer=new QTimer(this);
	//this->logTimer->setInterval(this->logTime);
	//connect(this->logTimer,&QTimer::timeout,this,&Logger::log);
	//this->loggerRunning=false;
}

Logger::~Logger(){
	/*if(this->file!=nullptr){
		if(this->file->isOpen()){
			this->file->close();
		}
		delete this->file;
	}
	if(this->logTimer!=nullptr){
		delete this->logTimer;
	}*/
}

void Logger::InitLogger(const QString& filename){
	this->fullNetFilePath="/home/pi/Documents/ARCHE/"+filename+".csv";
	this->fullLocalFilePath="/home/pi/Documents/Logs/"+filename+".csv";
	qDebug()<<"NetFilePath: "<<this->fullNetFilePath<<"\n";
	qDebug()<<"LocalFilePath: "<<this->fullLocalFilePath<<"\n";
	bool netOkay=this->logNet(this->headers);
	bool localOkay=this->logLocal(this->headers);

	if(!netOkay && !localOkay){
		this->logStatus.InitFailed();
		emit this->publishFileError("Error: Failed to log to file server and locally.\n"
									"Please contact Andrew Elmendorf\n"
									"Office: x159\n"
									"Cell: 843-687-6812\n");
		return;
	}

	if(netOkay){
		this->logStatus.LoggingNet();
	}

	if(localOkay){
		this->logStatus.LoggingLocal();
	}
}

void Logger::writeOutData(const QString &data){
	if(this->logStatus.canLog()){
		if(this->logStatus.loggingNet){
			this->logNet(data);
		}

		if(this->logStatus.loggingLocal){
			this->logLocal(data);
		}
	}
}

bool Logger::logLocal(const QString &data){
	QFile file(this->fullLocalFilePath);
	if(file.open(QFile::Append | QFile::Text | QFile::WriteOnly)){
		QTextStream stream(&file);
		stream<<data<<endl;
		//stream.flush();
		file.close();
		return true;
	}else{
		return false;
	}
}

bool Logger::logNet(const QString &data){
	QFile file(this->fullNetFilePath);
	if(file.open(QFile::Append | QFile::Text | QFile::WriteOnly)){
		QTextStream stream(&file);
		stream<<data<<endl;
		//stream.flush();
		file.close();
		return true;
	}else{
		return false;
	}
}

/*

bool Logger::isRunning(){
	return this->loggerRunning;
}

void Logger::stopLogging(){
	this->logTimer->stop();
	this->loggerRunning=false;
	this->paused=false;
	if(this->file->isOpen()){
		this->file->close();
	}
}writeOutData

void Logger::pauseLogging(){
	if(this->loggerRunning && !this->paused){
		this->logTimer->stop();
		this->paused=true;
	}
}

void Logger::continueLogging(){
	if(this->loggerRunning && this->paused){
		this->paused=false;
		this->logTimer->start(this->logTime);
	}
}

void Logger::recieveData(const QString &line){
	this->lastUpdate=line;
}

void Logger::log(){
	this->writeOutData(this->lastUpdate);
}

void Logger::startLogging(const QString &filename){
	if(!this->isRunning()){
		QDir dir(this->netDirectory);
		if(dir.exists()){

			qDebug()<<"Local File Path: "<<fullPath<<endl;
			this->file=new QFile(fullPath);
			if(!this->file->exists()){
				if(this->file->open(QFile::Append | QFile::Text)){
					QTextStream stream(this->file);writeOutData
					stream<<this->headers<<endl;
					stream<<this->lastUpdate<<endl;
					stream.flush();
				}
				this->file->close();
			}else{
				if(this->file->open(QFile::Append | QFile::Text)){
					QTextStream stream(this->file);
					stream<<this->lastUpdate<<endl;
					stream.flush();
				}
				this->file->close();
			}
		}else{
			dir.setPath(this->localDir);
			auto fullPath=dir.path()+"/"+filename+".csv";
			qDebug()<<"Local File Path: "<<fullPath<<endl;			if(
			this->file=new QFile(fullPath);
			if(!this->file->exists()){
				if(this->file->open(QFile::Append | QFile::Text)){
					QTextStream stream(this->file);
					stream<<this->headers<<endl;
					stream<<this->lastUpdate<<endl;
					stream.flush();
				}
				this->file->close();
			}else{
				if(this->file->open(QFile::Append | QFile::Text)){
					QTextStream stream(this->file);
					stream<<this->lastUpdate<<endl;
					stream.flush();
				}
				this->file->close();
			}
		}
		this->logTimer->start();
		this->loggerRunning=true;
		this->paused=false;
	}else{
		emit this->publishFileError("Warning: Logger already running");
	}
}
*/


