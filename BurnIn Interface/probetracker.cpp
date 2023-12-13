#include "probetracker.h"

ProbeTracker::ProbeTracker(QObject *parent) : QObject(parent){
    this->currentPercent=0.80;
    this->timePercent=0.985;
}

ProbeTracker::ProbeTracker(QObject *parent,
                           double percentCurrent,
                           double percentTime)
    :QObject(parent){
    this->currentPercent=(percentCurrent/1000);
    this->timePercent=1-(percentTime/1000);
}

void ProbeTracker::SetLimits(double percentTime,double percentCurrent){
    this->currentPercent=(percentCurrent/1000);
    this->timePercent=1-(percentTime/1000);
}

void ProbeTracker::StartTracking(const ControlValues& data){
    this->runTime=long(data.runTime)*1000;
    this->currentSetPoint=data.currentSP;
    this->minTime=this->runTime*this->timePercent;
    this->minCurrent=this->currentSetPoint*this->currentPercent;
    for(int i=0;i<6;i++){
        this->runTimes[i]=milliseconds(0);
        this->probeStatus[i]=true;
    }
    this->startTime=hr_clock::now();
    this->lastUpdate=this->startTime;
    this->testRunning=true;
}

void ProbeTracker::PauseTracking(){
    this->testRunning=false;
}

void ProbeTracker::ContinueTracking(){
    this->testRunning=true;
    this->lastUpdate=hr_clock::now();
}

void ProbeTracker::ContinueTracking(const ControlValues& data){
    this->runTime=long(data.runTime)*1000;
    this->currentSetPoint=data.currentSP;
    this->minTime=this->runTime*this->timePercent;
    this->minCurrent=this->currentSetPoint*this->currentPercent;
    this->testRunning=true;
    this->lastUpdate=hr_clock::now();

}

void ProbeTracker::Update(const ControlValues& data){
    if(this->testRunning){
        auto now=hr_clock::now();
        hr_duration t=now-this->startTime;
        this->elapsed=std::chrono::duration_cast<milliseconds>(t);
        auto padDuration=now-this->lastUpdate;
        milliseconds padDur=duration_cast<milliseconds>(padDuration);
        this->lastUpdate=now;
        if(data.i11>=this->minCurrent){
            this->runTimes[Pocket::p1]+=padDur;
        }

        if(data.i12>=this->minCurrent){
            this->runTimes[Pocket::p2]+=padDur;
        }

        if(data.i21>=this->minCurrent){
            this->runTimes[Pocket::p3]+=padDur;
        }


        if(data.i22>=this->minCurrent){
            this->runTimes[Pocket::p4]+=padDur;
        }

        if(data.i31>=this->minCurrent){
            this->runTimes[Pocket::p5]+=padDur;
        }

        if(data.i32>=this->minCurrent){
            this->runTimes[Pocket::p6]+=padDur;
        }

        for(int i=0;i<6;i++){
            auto dur=this->elapsed-this->runTimes[i];
            auto padDuration=duration_cast<milliseconds>(dur);
            this->probeStatus[i]=padDuration.count()<=this->minTime;
        }
    }
}

QString ProbeTracker::GetTimeString(Pocket pocket){
    long runTime=this->runTimes[pocket].count();
    int hours=(runTime/1000)/3600;
    int minutes=((runTime/1000)/60) % 60;
    int seconds=(runTime/1000) % 60;


    QString tempTime;
    QTextStream timeStream(&tempTime);


    if((hours / 10)<1 || hours==0){
        timeStream<<"0"<<QString::number(hours)<<":";
    }else {
        timeStream<<QString::number(hours)<<":";
    }

    if((minutes / 10)<1 || minutes==0){
        timeStream<<"0"<<QString::number(minutes)<<":";
    }else {
        timeStream<<QString::number(minutes)<<":";
    }

    if((seconds / 10)<1 || seconds==0){
        timeStream<<"0"<<QString::number(seconds);
    }else {
        timeStream<<QString::number(seconds);
    }
    return tempTime;
}

QString ProbeTracker::GetElapsedString(){
    long runTime=this->elapsed.count();
    int hours=(runTime/1000)/3600;
    int minutes=((runTime/1000)/60) % 60;
    int seconds=(runTime/1000) % 60;


    QString tempTime;
    QTextStream timeStream(&tempTime);


    if((hours / 10)<1 || hours==0){
        timeStream<<"0"<<QString::number(hours)<<":";
    }else {
        timeStream<<QString::number(hours)<<":";
    }

    if((minutes / 10)<1 || minutes==0){
        timeStream<<"0"<<QString::number(minutes)<<":";
    }else {
        timeStream<<QString::number(minutes)<<":";
    }

    if((seconds / 10)<1 || seconds==0){
        timeStream<<"0"<<QString::number(seconds);
    }else {
        timeStream<<QString::number(seconds);
    }
    return tempTime;
}

void ProbeTracker::StopTracking(){
    this->testRunning=false;
    this->elapsed=milliseconds(0);
    for(int i=0;i<6;i++){
        this->runTimes[i]=milliseconds(0);
        this->probeStatus[i]=true;
    }
}
