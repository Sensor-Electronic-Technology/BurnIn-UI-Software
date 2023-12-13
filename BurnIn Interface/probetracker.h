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

signals:

public slots:

private:
    hr_time_point lastUpdate,startTime,pauseTime;
    milliseconds  elapsed;
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
