#include "arduino.h"
#include <QDebug>



Arduino::Arduino(QObject *parent) : QObject(parent){
	this->connected=false;
	this->readEnabled=false;
	this->arduino=new QSerialPort();
	this->arduino->setBaudRate(QSerialPort::Baud38400);
	this->arduino->setDataBits(QSerialPort::Data8);
	this->arduino->setParity(QSerialPort::NoParity);
	this->arduino->setStopBits(QSerialPort::OneStop);
	this->arduino->setFlowControl(QSerialPort::NoFlowControl);
	connect(this->arduino,&QSerialPort::readyRead,this,&Arduino::readSerial);
	qRegisterMetaType<ControlValues>();
}

Arduino::~Arduino(){
	if(this->arduino->isOpen()){
		this->arduino->close();
	}
	delete this->arduino;
}

bool Arduino::isConnected(){
	return this->connected;
}

bool Arduino::FindArduinoSerialPort(){
	bool arduinoFound=true;
	//this->arduino->setPortName("arduino_1");
	foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
		qDebug()<<info.vendorIdentifier();
		if(info.vendorIdentifier()==this->arduinoVendorId1 || info.vendorIdentifier()==this->arduinoVendorId2){
				this->arduino->setPortName(info.portName());
				qDebug()<<info.manufacturer();
				arduinoFound=true;
				break;
			}
	}

	return arduinoFound;
}

void Arduino::sendCommand(SerialCommand command){
	if(this->connected){
		QString cmd;
		switch(command){
			case SerialCommand::START:{
				cmd="S";
				break;
			}
			case SerialCommand::PAUSE:{
				cmd="P";
				break;
			}
			case SerialCommand::TESTPROBE:{
				cmd="T";
				break;
			}
			case SerialCommand::RESET:{
				cmd="RR";
				break;
			}
			case SerialCommand::SWITCHCURRENT:{
				cmd="C";
				break;
			}
			case SerialCommand::SETTEMP:{
				cmd="H";
				break;
			}
		}
		this->arduino->write(cmd.toUtf8());
		if(!this->arduino->waitForBytesWritten(3000)){
			emit this->criticalError("Error: Timeout while sending "+cmd+" command");
		}
	}
}

void Arduino::processLine(const QByteArray &buffer){
	auto byteArrayList=buffer.split('[');
	for(const QByteArray &data:byteArrayList){
		if(data.length()>0){
			auto c=data.at(0);
			QChar command(c);
			switch(command.unicode()){
				case 'R':{
					auto string=QString::fromUtf8(data);
					auto closeC=string.indexOf(']');
					auto openB=string .indexOf('{');
					auto closeB=string.indexOf('}');
					bool success;
					auto rindex=string.mid(1,closeC-1).toInt(&success);
					auto vIndex=string.mid(openB+1,closeB-(openB+1)).toDouble(&success);
					if(success){
						this->realArray[rindex]=vIndex;
					}
					break;
				}
				case 'B':{
					bool success;
					auto string=QString::fromUtf8(data);
					auto closeC=string.indexOf(']');
					auto rindex=string.mid(1,closeC-1).toInt(&success);
					if(success){
						if(string.contains("{1}")){
							this->boolArray[rindex]=true;
						}else if(string.contains("{0}")){
							this->boolArray[rindex]=false;
						}
					}
					break;
				}
				case 'T':{
					auto string=QString::fromUtf8(data);
					auto openB=string .indexOf('{');
					auto closeB=string.indexOf('}');
					auto com=string.mid(openB+1,closeB-(openB+1));
					emit this->comUpdate(com);
					break;
				}
				default:{
					break;
				}
			}
		}
	}
	ControlValues controlData;
	controlData.v11=realArray[0];
	controlData.v12=realArray[1];

	controlData.v21=realArray[2];
	controlData.v22=realArray[3];

	controlData.v31=realArray[4];
	controlData.v32=realArray[5];

	controlData.t1=realArray[6];
	controlData.t2=realArray[7];
	controlData.t3=realArray[8];

	controlData.i11=realArray[12];
	controlData.i12=realArray[13];
	controlData.i21=realArray[14];
	controlData.i22=realArray[15];
	controlData.i31=realArray[16];
	controlData.i32=realArray[17];

	controlData.currentSP=realArray[11];
	controlData.temperatureSP=realArray[10];
	int totalSeconds=(int)this->realArray[9];
	int hours=totalSeconds/3600;
	int minutes=(totalSeconds/60) %60;
	int seconds=totalSeconds % 60;
	QString tempTime;
	QTextStream timeStream(&tempTime);

	if((hours / 10)<1 || hours==0){
		timeStream<<"0"+QString::number(hours)<<":";
	}else {
		timeStream<<QString::number(hours)<<":";
	}


	if((minutes / 10)<1 || minutes==0){
		timeStream<<"0"+QString::number(minutes)<<":";
	}else {
		timeStream<<QString::number(minutes)<<":";
	}

	if((seconds / 10)<1 || seconds==0){
		timeStream<<"0"+QString::number(seconds);
	}else {
		timeStream<<QString::number(seconds);
	}

	controlData.elapsedTime=tempTime;

	controlData.heating1=boolArray[1];
	controlData.heating2=boolArray[2];
	controlData.heating3=boolArray[3];
	controlData.running=boolArray[0];
	controlData.paused=boolArray[4];
	QString data;
	QTextStream stream(&data);
	auto dateTime=QDateTime::currentDateTime();
	stream<<dateTime.date().toString()<<",";
	stream<<dateTime.time().toString()<<",";
	stream<<controlData.elapsedTime<<","<<controlData.v11<<","<<controlData.v12<<",";
	stream<<controlData.v21<<","<<controlData.v22<<",";
	stream<<controlData.v31<<","<<controlData.v32<<",";

	stream<<controlData.i11<<","<<controlData.i12<<",";
	stream<<controlData.i21<<","<<controlData.i22<<",";
	stream<<controlData.i31<<","<<controlData.i32<<",";

	stream<<controlData.t1<<","<<controlData.t2<<","<<controlData.t3<<",";
	stream<<controlData.currentSP;
	controlData.logtext=data;
	emit this->dataUpdate(controlData);
}

void Arduino::serialConnect(){
	if(this->arduino->isOpen()){
		this->arduino->close();
	}
	bool arduinoFound=this->FindArduinoSerialPort();
	if(arduinoFound){

		if(!this->arduino->open(QSerialPort::ReadWrite)){
			emit this->criticalError("Error: Failed to connect to burn-in station");
			this->connected=false;
			this->readEnabled=false;
		}else{
			emit this->comUpdate("Burn-In station connected");
			emit this->portNameUpdate(this->arduino->portName());
			this->connected=true;
			this->readEnabled=true;
		}
	}else{
		emit this->criticalError("Error: Burn-In station USB not found");
		this->connected=false;
		this->readEnabled=false;
	}
}

void Arduino::readSerial(){
	if(this->connected && this->readEnabled){
		while(this->arduino->canReadLine()){
			this->processLine(this->arduino->readLine());
		}
	}
}

void Arduino::sendFirmwareUpdate(const QByteArray &settings){
	this->arduino->write(settings);
	if(!this->arduino->waitForBytesWritten(3000)){
		emit this->criticalError("Error: Timeout while sending "+settings+" command");
	}
}
