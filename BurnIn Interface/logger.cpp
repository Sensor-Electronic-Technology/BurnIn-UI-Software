#include "logger.h"

Logger::Logger(QObject *parent):QObject(parent){
}

Logger::~Logger(){

}

void Logger::InitLogger(const QString& filename){
	this->fullNetFilePath=NetDirectory+filename+".csv";
	this->fullLocalFilePath=LocalDirectory+filename+".csv";
    this->networkErrorLatch=false;
    this->fCopyErrorLatch=false ;

	if(this->logStatus.networkConnected()){
        this->logStatus.loggingNet=this->logNet(this->headers);
	}else{
		this->logStatus.loggingNet=false;
	}

    this->logStatus.loggingLocal=this->logLocal(this->headers);

    if(!this->logStatus.loggingNet && !this->logStatus.loggingLocal){
		this->logStatus.InitFailed();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Error: failed to log to file server and local"<<endl;
        buffer<<"Please contact Andrew Elmendorf"<<endl;
        buffer<<"Office: x159"<<endl;
        buffer<<"Cell: 843-687-6812"<<endl;
        emit this->publishFileError(message);
		return;
    }else if(!this->logStatus.loggingNet && this->logStatus.loggingLocal){
        this->logStatus.LoggingLocal();
        this->networkErrorLatch=true;
        QString message;
        QTextStream buffer(&message);
        buffer<<"Error: Failed to network log"<<endl;
        buffer<<"Log should start once network connection or server access is fixes"<<endl;
        emit this->publishFileError(message);
    }else if(this->logStatus.loggingNet && !this->logStatus.loggingLocal){
        this->logStatus.LoggingNet();
        this->networkErrorLatch=false;
        QString message;
        QTextStream buffer(&message);
        buffer<<"Error: Failed to local log"<<endl;
        emit this->publishFileError(message);
    }else{
        this->logStatus.LoggingNet();
        this->logStatus.LoggingLocal();
        this->networkErrorLatch=true;
        QString message;
        QTextStream buffer(&message);
        buffer<<"Logging started without issue"<<endl;
        emit this->publishMessage(message);
    }

/*	if(netOkay){
		this->logStatus.LoggingNet();
	}

	if(localOkay){
		this->logStatus.LoggingLocal();
    }*/
}

void Logger::writeOutData(const QString &data){
	if(this->logStatus.canLog()){
		if(this->logStatus.loggingLocal){
			this->logLocal(data);
		}
		if(this->logStatus.loggingNet){
			if(this->logStatus.networkConnected()){
                this->logStatus.loggingNet=this->logNet(data);
			}else{
                if(!this->networkErrorLatch){
                    this->networkErrorLatch=true;
                    QString message;
                    QTextStream buffer(&message);
                    buffer<<"Log Error: Network logging interupted"<<endl;
                    emit this->publishFileError(message);
                    this->logStatus.loggingNet=false;
                }
			}
		}else if(this->logStatus.networkConnected()){
			QFile localFile(this->fullLocalFilePath);
			QFile netFile(this->fullNetFilePath);
			if(netFile.exists()){
				QDir dir;
				if(dir.remove(this->fullNetFilePath)){
					if(localFile.copy(this->fullNetFilePath)){
						this->logStatus.loggingNet=true;
                        this->networkErrorLatch=false;
                        this->fCopyErrorLatch=false;
                        QString message;
                        QTextStream buffer(&message);
                        buffer<<"Log Message:Network logging continued"<<endl;
                        emit this->publishMessage(message);
					}else{
                        if(!this->fCopyErrorLatch){
                            this->fCopyErrorLatch=true;
                            QString message;
                            QTextStream buffer(&message);
                            buffer<<"Log Error: Failed to copy local file to network"<<endl;
                            emit this->publishFileError(message);
                            this->logStatus.loggingNet=false;
                        }
					}
				}else{
                    QString message;
                    QTextStream buffer(&message);
                    buffer<<"Log Error: Failed to remove old network file"<<endl;
                    emit this->publishFileError(message);
					this->logStatus.loggingNet=false;
				}
			}else{
				if(localFile.copy(this->fullNetFilePath)){
					this->logStatus.loggingNet=true;
                    QString message;
                    QTextStream buffer(&message);
                    buffer<<"Log Message:Network logging continued"<<endl;
                    emit this->publishMessage(message);
				}else{
                    if(!this->fCopyErrorLatch){
                        this->fCopyErrorLatch=true;
                        this->logStatus.loggingNet=false;
                        QString message;
                        QTextStream buffer(&message);
                        buffer<<"Log Error: Failed to copy local file to network"<<endl;
                        emit this->publishFileError(message);
                        this->logStatus.loggingNet=false;
                    }
				}
			}
		}
	}
}

bool Logger::logLocal(const QString &data){
	QFile file(this->fullLocalFilePath);
	if(file.open(QFile::Append | QFile::Text | QFile::WriteOnly)){
		QTextStream stream(&file);
		stream<<data<<endl;
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
		file.close();
		return true;
	}else{
		return false;
	}
}

