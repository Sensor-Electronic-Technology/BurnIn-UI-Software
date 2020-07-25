#include "logger.h"

Logger::Logger(QObject *parent):QObject(parent){
	this->logTimer=new QTimer(this);
	this->logTimer->setInterval(this->logTime);
	connect(this->logTimer,&QTimer::timeout,this,&Logger::log);
	this->loggerRunning=false;
}

Logger::~Logger(){
	if(this->file!=nullptr){
		if(this->file->isOpen()){
			this->file->close();
		}
		delete this->file;
	}
	if(this->logTimer!=nullptr){
		delete this->logTimer;
	}
}

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
}

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
			auto fullPath=dir.path()+"/"+filename+".csv";
			qDebug()<<"Local File Path: "<<fullPath<<endl;
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
		}else{
			dir.setPath(this->localDir);
			auto fullPath=dir.path()+"/"+filename+".csv";
			qDebug()<<"Local File Path: "<<fullPath<<endl;
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

void Logger::writeOutData(const QString &data){
	if(this->file->open(QFile::Append | QFile::Text)){
		QTextStream stream(this->file);
		stream<<data<<endl;
		stream.flush();
	}
	this->file->close();
}
