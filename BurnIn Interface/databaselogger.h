#ifndef DATABASELOGGER_H
#define DATABASELOGGER_H
#include <QObject>
#include <QtSql>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRelationalTableModel>
#include "globaldatadefinitions.h"

#define DATABASE    "epi"
#define HOST        "172.20.4.20"
#define USERNAME    "aelmendorf"
#define PASSWORD    "Drizzle123!"

#define BURNIN_START        "Call burnin_start(:logName,:w1,:w2,:w3,:w1_a1,:w1_a2,:w2_a1,:w2_a2,:w3_a1,:w3_a2,:station,:runTime,:setTemp,:setCurrent)"
#define BURNIN_STOP         "Call burnin_stop(:logName)"
#define UPDATE_SETTINGS     "call burnin_update_settings(:wafer,:area1,:area2,:setTemp,:setCurrent,:pocket)"

#define WAFEREXIST          "Select exists(Select 1 from burnin_data where WaferId=:wafer Limit 1)"
#define LOGEXIST            "Select exists(Select 1 from burnin_log where logFileName=:logFile Limit 1)"

#define DELETELOG           "call burnin_delete_log(:logName)"
#define UPDATE_INITIAL      "Call burnin_update_initial(:wafer,:area1,:area2,:p1_v,:p1_i,:p2_v,:p2_i)"
#define UPDATE_FINAL        "Call burnin_update_final(:wafer,:area1,:area2,:p1_v,:p1_i,:p2_v,:p2_i)"
#define LOGCOMPLETE         "Select completed from burnin_log where logFileName=:logName limit 1"

class DatabaseLogger : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseLogger(QObject *parent = nullptr);
    bool InitConnection();
    void setTest(const TestInfo& test);
    void updateTestSetting(int setCurrent,int setTemp);
    bool waferExists(const QString& wafer);
    bool logFileExists(const QString& logFileName);
    bool logCompleted(const QString& logFileName);
    void clearTest();

    const TestInfo& getTestInfo();
signals:
    void databaseError(const QString& message);
    void databaseMessage(const QString& message);
public slots:
    void logInitial(const ControlValues& data);
    void logFinal(const ControlValues& data);
    void logStart();
    void logStop();
    void deleteLog();
private:
    bool CheckConnection();
    void updateInitial(const WaferLog& waferLog);
    void updateFinal(const WaferLog& waferLog);
    void parseControlValues(const ControlValues& data,bool initial);
    bool connected;
    TestInfo testLog;
    QString lastError;
};

#endif // DATABASELOGGER_H
