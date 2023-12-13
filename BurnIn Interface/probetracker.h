#ifndef PROBETRACKER_H
#define PROBETRACKER_H

#include <QObject>
#include <chrono>
#include <QString>
#include "globaldatadefinitions.h"

using hr_clock = std::chrono::high_resolution_clock;
using hr_time_point = hr_clock::time_point;
using hr_duration = hr_clock::duration;
using milliseconds = std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

class ProbeTracker : public QObject
{
    Q_OBJECT
public:
    explicit ProbeTracker(QObject *parent = nullptr);
    ProbeTracker(QObject *parent = nullptr,
                 double percentCurrent=80,
                 double percentTime=1.5);

    void StartTracking(const ControlValues& data);
    void PauseTracking();
    void ContinueTracking();
    void StopTracking();
    void Update(const ControlValues& data);
    QString GetTimeString(Pocket pocket);
    QString GetElapsedString();
    void ContinueTracking(const ControlValues& data);
    void SetLimits(double percentTime,double percentCurrent);

    bool GetPadStatus(int i){
        return this->probeStatus[i];
    }

    long long GetPocketRuntime(Pocket pocket){
        return this->runTimes[pocket].count();
    }

    void SetPocketRuntime(Pocket pocket,long long rt){
        this->runTimes[pocket]=milliseconds(rt);
    }

    long long GetElapsedMilli(){
        return this->elapsed.count();
    }

    long long GetStartTime(){
        auto t= std::chrono::time_point_cast<milliseconds>(this->startTime);
        return t.time_since_epoch().count();
    }

    void SetStartTime(long long timeSinceEpoch){
        milliseconds oldStart=milliseconds(timeSinceEpoch);
        hr_time_point old(oldStart);
        auto now=hr_clock::now();
        this->startTime=now+(now-old);
    }

signals:

public slots:

private:
    hr_time_point lastUpdate,startTime,pauseTime;
    milliseconds  elapsed,pausedDuration;
    milliseconds  runTimes[6]={milliseconds(0),milliseconds(0),
                               milliseconds(0),milliseconds(0),
                               milliseconds(0),milliseconds(0)};
    bool probeStatus[6]={true,true,true,true,true,true};
    bool testRunning=false;
    double currentPercent,timePercent;
    double minCurrent;
    double minTime;
    double currentSetPoint;
    long runTime;

};

#endif // PROBETRACKER_H
