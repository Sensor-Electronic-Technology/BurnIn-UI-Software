#include "logger.h"

Logger::Logger(QObject *parent):QObject(parent){

}

Logger::~Logger(){

}

void Logger::InitLogger(const QString& filename){
	this->fullNetFilePath=NetDirectory+filename+".csv";
	this->fullLocalFilePath=LocalDirectory+filename+".csv";
	bool netOkay,localOkay;
	netOkay=false;
	localOkay=false;
	if(this->logStatus.networkConnected()){
		netOkay=this->logNet(this->headers);
	}else{
		netOkay=false;
		this->logStatus.loggingNet=false;
	}

	localOkay=this->logLocal(this->headers);

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
		if(this->logStatus.loggingLocal){
			this->logLocal(data);
		}
		if(this->logStatus.loggingNet){
			if(this->logStatus.networkConnected()){
				this->logNet(data);
			}else{
				this->logStatus.loggingNet=false;
			}
		}else if(this->logStatus.networkConnected()){
			QFile localFile(this->fullLocalFilePath);
			QFile netFile(this->fullNetFilePath);
			if(netFile.exists()){
				QDir dir;
				if(dir.remove(this->fullNetFilePath)){
					if(localFile.copy(this->fullNetFilePath)){
						this->logStatus.loggingNet=true;
					}else{
						this->logStatus.loggingNet=false;
					}
				}else{
					this->logStatus.loggingNet=false;
				}
			}else{
				if(localFile.copy(this->fullNetFilePath)){
					this->logStatus.loggingNet=true;
				}else{
					this->logStatus.loggingNet=false;
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

