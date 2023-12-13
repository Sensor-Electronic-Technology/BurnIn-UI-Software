#ifndef GLOBALDATADEFINITIONS_H
#define GLOBALDATADEFINITIONS_H

#include <QObject>
#include <QDateTime>
#include <QSettings>
#include <QApplication>
#include <QStandardPaths>
#include <QTextStream>
#include <chrono>


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

enum Pocket{
    p1=0,
    p2=1,
    p3=2,
    p4=3,
    p5=4,
    p6=5
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

    void SetTimeString(int hours,int minutes,int seconds){
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
        this->elapsedTime=tempTime;
    }
};

struct PadStatus{
    time_t lastUpdate;
    bool testRunning;
    double timeOffPercent;
    double  currentPercent;
    long runTime;
    double setCurrent;
    double minCurrent=0;
    double minTime;
    long elapsed;
    long runtimes[6]={0,0,0,0,0,0};
    bool statues[6]={false,false,false,false,false,false};

    PadStatus(){
        for(int i=0;i<6;i++){
            this->runtimes[i]=0;
            this->statues[i]=false;
        }
        this->timeOffPercent=1-0.015;
        this->currentPercent=0.8;

        this->testRunning=false;
    }

    PadStatus(double percentTime,double percentCurrent){
        for(int i=0;i<6;i++){
            this->runtimes[i]=0;
            this->statues[i]=false;
        }
        this->timeOffPercent=(1-(percentTime/100));
        this->currentPercent=percentCurrent/100;
    }

    PadStatus(const PadStatus &rhs){
        for(int i=0;i<6;i++){
            this->runtimes[i]=rhs.runtimes[i];
            this->statues[i]=rhs.statues[i];
        }
    }

    PadStatus& operator =(const  PadStatus& rhs){
        for(int i=0;i<6;i++){
            this->runtimes[i]=rhs.runtimes[i];
            this->statues[i]=rhs.statues[i];
        }
        return *this;
    }

    void StartTest(const ControlValues& data,double percentTime,double percentCurrent){
        this->setCurrent=data.currentSP;
        this->runTime=data.runTime;
        this->timeOffPercent=(1-(percentTime/100));
        this->currentPercent=percentCurrent/100;
        this->minCurrent=this->setCurrent*this->currentPercent;
        this->minTime=this->runTime*this->timeOffPercent;
        this->testRunning=true;
        time(&this->lastUpdate);
    }

    void StartTest(const ControlValues& data){
        this->setCurrent=data.currentSP;
        this->runTime=data.runTime;
        this->minCurrent=this->setCurrent*this->currentPercent;
        this->minTime=this->runTime*this->timeOffPercent;
        this->testRunning=true;
        time(&this->lastUpdate);
    }
    void StartTest(long rt,double currentSP,double percentTime,double percentCurrent){
        this->setCurrent=currentSP;
        this->runTime=rt;
        this->timeOffPercent=(1-(percentTime/100));
        this->currentPercent=percentCurrent/100;
        this->minCurrent=this->setCurrent*this->currentPercent;
        this->minTime=this->runTime*this->timeOffPercent;
        this->testRunning=true;
        time(&this->lastUpdate);
    }

    void ChangeLimits(double percentTime,double percentCurrent){
        this->timeOffPercent=(1-(percentTime/100));
        this->currentPercent=percentCurrent/100;
    }

    void StopTest(){
        this->runTime=0;
        this->testRunning=false;
    }


    void ParseControlData(const ControlValues& data){
        if(this->testRunning){
            auto diff=difftime(time(nullptr),this->lastUpdate);
            time(&this->lastUpdate);

            if(data.i11>=this->minCurrent){
                this->runtimes[Pocket::p1]+=diff;
            }

            if(data.i12>=this->minCurrent){
                this->runtimes[Pocket::p2]+=diff;
            }

            if(data.i21>=this->minCurrent){
                this->runtimes[Pocket::p3]+=diff;
            }


            if(data.i22>=this->minCurrent){
                this->runtimes[Pocket::p4]+=diff;
            }


            if(data.i31>=this->minCurrent){
                this->runtimes[Pocket::p5]+=diff;
            }

            if(data.i32>=this->minCurrent){
                this->runtimes[Pocket::p6]+=diff;
            }

            for(int i=0;i<6;i++){
                this->statues[i]=(data.elapsed-this->runtimes[i])>=this->minTime;
            }
        }
    }

    QString GetTimeString(Pocket pocket){
        int hours=this->runtimes[pocket]/3600;
        int minutes=(this->runtimes[pocket]/60) % 60;
        int seconds=this->runtimes[pocket] % 60;

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
        return tempTime;
    }
};

struct ApplicationSettings{
    int id;
	bool switchingEnabled;
    int defaultCurrent;
	int setTemperature;
    double timeOffPercent;
    double currentPercent;

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


