#ifndef GLOBALDATADEFINITIONS_H
#define GLOBALDATADEFINITIONS_H

#include <QObject>
#include <QDateTime>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>

using namespace std;

#define FirstLogTime    5
#define LastLogDelta    10

const static QStringList AreaList={"a","b","c","t","r","l","g","none"};

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
	double i11,i12,i21,i22,i31,i32;
	bool heating1,heating2,heating3,running,paused;
	double currentSP,temperatureSP;
    long runTime,elapsed;
	QString elapsedTime;
	QString logtext;

	ControlValues(){
		this->v11=0;
		this->v12=0;
		this->v21=0;
		this->v22=0;
		this->v31=0;
		this->v32=0;
		this->i11=0;
		this->i12=0;
		this->i21=0;
		this->i22=0;
		this->i31=0;
		this->i32=0;
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
        this->runTime=0;
        this->elapsed=0;
	}

	ControlValues(const ControlValues &rhs){
		this->v11=rhs.v11;
		this->v12=rhs.v12;
		this->v21=rhs.v21;
		this->v22=rhs.v22;
		this->v31=rhs.v31;
		this->v32=rhs.v32;
		this->i11=rhs.i11;
		this->i12=rhs.i12;
		this->i21=rhs.i21;
		this->i22=rhs.i22;
		this->i31=rhs.i31;
		this->i32=rhs.i32;
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
        this->runTime=rhs.runTime;
        this->elapsed=rhs.elapsed;
	}
};

struct ApplicationSettings{
    int id;
	bool switchingEnabled;
    int defaultCurrent;
	int setTemperature;

	ApplicationSettings(){
        this->id=0;
		this->switchingEnabled=false;
        this->defaultCurrent=150;
		this->setTemperature=85;
	}

    ApplicationSettings(const ApplicationSettings& other){
        this->id=other.id;
        this->switchingEnabled=other.switchingEnabled;
        this->defaultCurrent=other.defaultCurrent;
        this->setTemperature=other.setTemperature;
    }

    ApplicationSettings& operator=(const ApplicationSettings& other){
        this->id=other.id;
        this->switchingEnabled=other.switchingEnabled;
        this->defaultCurrent=other.defaultCurrent;
        this->setTemperature=other.setTemperature;
        return *this;
    }
};

struct WaferLog{
    QString waferId;
    QChar p1_area;
    QChar p2_area;
    double p1_init_volt;
    double p1_init_current;
    double p2_init_volt;
    double p2_init_current;

    double p1_final_volt;
    double p1_final_current;
    double p2_final_volt;
    double p2_final_current;

    WaferLog(){
        this->waferId="none";
        this->p1_area='n';
        this->p2_area='n';
        this->p1_init_volt=0;
        this->p1_init_current=0;
        this->p1_final_volt=0;
        this->p1_final_current=0;
        this->p2_init_volt=0;
        this->p2_init_current=0;
        this->p2_final_volt=0;
        this->p2_final_current=0;
    }

    WaferLog(const WaferLog& other){
        this->waferId=other.waferId;
        this->p1_area=other.p1_area;
        this->p2_area=other.p2_area;
        this->p1_init_volt=other.p1_init_volt;
        this->p1_init_current=other.p1_init_current;
        this->p2_init_volt=other.p2_init_volt;
        this->p2_init_current=other.p2_init_current;
        this->p1_final_volt=other.p1_final_volt;
        this->p1_final_current=other.p1_final_current;
        this->p2_final_volt=other.p2_final_volt;
        this->p2_final_current=other.p2_final_current;
    }

    WaferLog& operator=(const WaferLog& other){
        this->waferId=other.waferId;
        this->p1_area=other.p1_area;
        this->p2_area=other.p2_area;
        this->p1_init_volt=other.p1_init_volt;
        this->p1_init_current=other.p1_init_current;
        this->p2_init_volt=other.p2_init_volt;
        this->p2_init_current=other.p2_init_current;
        this->p1_final_volt=other.p1_final_volt;
        this->p1_final_current=other.p1_final_current;
        this->p2_final_volt=other.p2_final_volt;
        this->p2_final_current=other.p2_final_current;
        return *this;
    }
};

struct TestInfo{
    WaferLog pocket1;
    WaferLog pocket2;
    WaferLog pocket3;
    QString logName;
    QDateTime startTime;
    QDateTime stopTime;
    int runTime;
    int elapsed;
    int stationId;
    int set_current;
    int set_temp;

    TestInfo(){
        this->set_current=0;
        this->set_temp=0;
        this->runTime=0;
        this->elapsed=0;
        this->stationId=0;
    }

    TestInfo(const TestInfo& other){
        this->pocket1=other.pocket1;
        this->pocket2=other.pocket2;
        this->pocket3=other.pocket3;
        this->startTime=other.startTime;
        this->stopTime=other.stopTime;
        this->elapsed=other.elapsed;
        this->runTime=other.runTime;
        this->logName=other.logName;
        this->stationId=other.stationId;
        this->set_temp=other.set_temp;
        this->set_current=other.set_current;
    }

    TestInfo& operator=(const TestInfo& other){
        this->pocket1=other.pocket1;
        this->pocket2=other.pocket2;
        this->pocket3=other.pocket3;
        this->startTime=other.startTime;
        this->stopTime=other.stopTime;
        this->elapsed=other.elapsed;
        this->runTime=other.runTime;
        this->logName=other.logName;
        this->stationId=other.stationId;
        this->set_temp=other.set_temp;
        this->set_current=other.set_current;
        return *this;
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
Q_DECLARE_METATYPE(ApplicationSettings)
Q_DECLARE_METATYPE(TestInfo)



#endif // GLOBALDATADEFINITIONS_H


