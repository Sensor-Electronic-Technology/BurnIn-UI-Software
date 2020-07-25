#ifndef GLOBALDATADEFINITIONS_H
#define GLOBALDATADEFINITIONS_H

#include <QObject>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>

using namespace std;

enum FileError{
	CREATEFAILED,
	DIRNOTFOUND,
	FILEALREADYEXIST,
};

enum SerialCommand{
	TESTPROBE,
	START,
	PAUSE,
	SETTEMP,
	SWITCHCURRENT,
	RESET
};

struct ControlValues{
	double v11,v12,v21,v22,v31,v32,t1,t2,t3;
	bool heating1,heating2,heating3,running,paused;
	double currentSP,temperatureSP;
	QString elapsedTime;
	QString logtext;

	ControlValues(){
		this->v11=0;
		this->v12=0;
		this->v21=0;
		this->v22=0;
		this->v31=0;
		this->v32=0;
		this->t1=0;
		this->t2=0;
		this->t3=0;
		this->heating1=false;
		this->heating2=false;
		this->heating3=false;
		this->running=false;
		this->paused=false;
		this->currentSP=0;
		this->temperatureSP=0;
		this->elapsedTime="00:00:00";
		this->logtext="";
	}

	ControlValues(const ControlValues &rhs){
		this->v11=rhs.v11;
		this->v12=rhs.v12;
		this->v21=rhs.v21;
		this->v22=rhs.v22;
		this->v31=rhs.v31;
		this->v32=rhs.v32;
		this->t1=rhs.t1;
		this->t2=rhs.t2;
		this->t3=rhs.t3;
		this->heating1=rhs.heating1;
		this->heating2=rhs.heating2;
		this->heating3=rhs.heating3;
		this->running=rhs.running;
		this->paused=rhs.paused;
		this->currentSP=rhs.currentSP;
		this->temperatureSP=rhs.temperatureSP;
		this->elapsedTime=rhs.elapsedTime;
		this->logtext=rhs.logtext;
	}
};

struct ApplicationSettings{
	QString id,ipAddress;
	bool switchingEnabled;
	QString current2;
	int analogVersion;
	int setTemperature;
	int voltageVersion;

	ApplicationSettings(){
		this->id="1";
		this->switchingEnabled=false;
		this->current2="120";
		this->ipAddress="0.0.0.0";
		this->setTemperature=85;
		this->analogVersion=1;
		this->voltageVersion=1;
	}
};

struct HoldingRegisters{
	quint16 id;
	quint16 current2;
	quint16 setTemperature;

};

enum AppState{
	RUNNING,
	PAUSED,
	IDLE
};

Q_DECLARE_METATYPE(ControlValues)
Q_DECLARE_METATYPE(ApplicationSettings);



#endif // GLOBALDATADEFINITIONS_H


