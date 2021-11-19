#include "databaselogger.h"

DatabaseLogger::DatabaseLogger(QObject *parent) : QObject(parent){

}

bool DatabaseLogger::InitConnection(){
    QSqlDatabase database=QSqlDatabase::addDatabase("QMYSQL");
    database.setHostName(HOST);
    database.setDatabaseName(DATABASE);
    database.setUserName(USERNAME);
    database.setPassword(PASSWORD);
    if(database.open()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Database connected"<<endl;
        emit this->databaseMessage(message);
        return true;
    }else{
        this->lastError=database.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Failed to connect to database"<<endl;
        buffer<<this->lastError<<endl;
        emit this->databaseError(message);
       return false;
    }
}

const TestInfo& DatabaseLogger::getTestInfo(){
    return this->testLog;
}

bool DatabaseLogger::CheckConnection(){
    QSqlDatabase database=QSqlDatabase::database("MYSQL");
    if(database.isOpen()){
        return true;
    }else{
        if(database.open()){
            return true;
        }else{
            this->lastError=database.lastError().text();
            QString message;
            QTextStream buffer(&message);
            buffer<<" Database not connected "<<endl;
            buffer<<this->lastError<<endl;
            emit this->databaseError(message);
            return false;
        }
    }
}

bool DatabaseLogger::waferExists(const QString& wafer){
    QSqlQuery query;
    query.prepare(WAFEREXIST);
    query.bindValue(":wafer",wafer);
    if(query.exec()){
        if(query.next()){
            return query.value(0).toBool();
        }else{
            this->lastError=query.lastError().text();
            QString message;
            QTextStream buffer(&message);
            buffer<<"Failed to fetch value from query.  WaferId: "<<wafer<<endl;
            buffer<<this->lastError<<endl;
            emit this->databaseMessage(message);
            return false;
        }
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Failed to query waferId "<<wafer<<endl;
        buffer<<this->lastError<<endl;
        emit this->databaseError(message);
        return false;
    }
}

bool DatabaseLogger::logFileExists(const QString &logFileName){
    QSqlQuery query;
    query.prepare(LOGEXIST);
    query.bindValue(":logFile",logFileName);
    if(query.exec()){
        qDebug()<<"Query executed,fetching value"<<endl;
        if(query.next()){
            return query.value(0).toBool();
        }else{
            this->lastError=query.lastError().text();
            QString message;
            QTextStream buffer(&message);
            buffer<<"Failed to fetch database log query"<<endl;
            buffer<<this->lastError<<endl;
            emit this->databaseError(message);
            return false;
        }
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log query failed"<<endl;
        buffer<<this->lastError<<endl;
        emit this->databaseError(message);
        return false;
    }
}

bool DatabaseLogger::logCompleted(const QString &logFileName){
    QSqlQuery query;
    query.prepare(LOGCOMPLETE);
    query.bindValue(":logFile",logFileName);
    if(query.exec()){
        qDebug()<<"Query executed,fetching value"<<endl;
        if(query.next()){
            return query.value(0).toBool();
        }else{
            this->lastError=query.lastError().text();
            QString message;
            QTextStream buffer(&message);
            buffer<<"Failed to fetch log completed";
            buffer<<this->lastError<<endl;
            emit this->databaseError(message);
            return false;
        }
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log completed query failed. Error:";
        buffer<<this->lastError;
        emit this->databaseError(message);
        return false;
    }
}

void DatabaseLogger::logInitial(const ControlValues& data){
    this->parseControlValues(data,true);
    this->updateInitial(this->testLog.pocket1);
    this->updateInitial(this->testLog.pocket2);
    this->updateInitial(this->testLog.pocket3);
}

void DatabaseLogger::logFinal(const ControlValues& data){
    this->parseControlValues(data,false);
    this->updateFinal(this->testLog.pocket1);
    this->updateFinal(this->testLog.pocket2);
    this->updateFinal(this->testLog.pocket3);
}

void DatabaseLogger::clearTest(){
    TestInfo testInfo;
    this->testLog=testInfo;
}

void DatabaseLogger::updateTestSetting(int setCurrent, int setTemp){
    this->testLog.set_current=setCurrent;
    this->testLog.set_temp=setTemp;
}

void DatabaseLogger::setTest(const TestInfo& test){
    this->testLog=test;
    qDebug()<<"setting test log"<<endl;
}

void DatabaseLogger::logStart(){
    QSqlQuery query;
    query.prepare(BURNIN_START);
    query.bindValue(":logName",this->testLog.logName);
    query.bindValue(":w1",this->testLog.pocket1.waferId);
    query.bindValue(":w2",this->testLog.pocket2.waferId);
    query.bindValue(":w3",this->testLog.pocket3.waferId);
    query.bindValue(":w1_a1",this->testLog.pocket1.p1_area);
    query.bindValue(":w1_a2",this->testLog.pocket1.p2_area);
    query.bindValue(":w2_a1",this->testLog.pocket2.p1_area);
    query.bindValue(":w2_a2",this->testLog.pocket2.p2_area);
    query.bindValue(":w3_a1",this->testLog.pocket3.p1_area);
    query.bindValue(":w3_a2",this->testLog.pocket3.p2_area);
    query.bindValue(":station",this->testLog.stationId);
    query.bindValue(":runtime",this->testLog.runTime);
    query.bindValue(":setTemp",this->testLog.set_temp);
    query.bindValue(":setCurrent",this->testLog.set_current);
    if(query.exec()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log started: "<<this->testLog.logName<<endl;
        emit this->databaseMessage(message);
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Error Starting Log "<<this->testLog.logName<<" Error: ";
        buffer<<this->lastError;
        emit this->databaseError(message);
    }
}

void DatabaseLogger::logStop(){
    qDebug()<<"Stopping Log?"<<endl;
    QSqlQuery query;
    query.prepare(BURNIN_STOP);
    query.bindValue(":logName",this->testLog.logName);
    if(query.exec()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log stopped: LogName:"<<this->testLog.logName;
        emit this->databaseMessage(message);
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Error stopping log "<<this->testLog.logName<<" Error: ";
        buffer<<this->lastError;
        emit this->databaseError(message);
    }
}

void DatabaseLogger::deleteLog(){
    QSqlQuery query;
    query.prepare(DELETELOG);
    query.bindValue(":logName",this->testLog.logName);
    if(query.exec()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log: "<<this->testLog.logName<<" deleted.  Wafer data has been reset";
        emit this->databaseMessage(message);
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Log: "<<this->testLog.logName<<" failed to delete";
        buffer<<this->lastError;
        emit this->databaseError(message);
    }
}

void DatabaseLogger::updateInitial(const WaferLog& waferLog){
    QSqlQuery query;
    query.prepare(UPDATE_INITIAL);
    query.bindValue(":wafer",waferLog.waferId);
    query.bindValue(":area1",waferLog.p1_area);
    query.bindValue(":area2",waferLog.p2_area);
    query.bindValue(":p1_v",waferLog.p1_init_volt);
    query.bindValue(":p1_i",waferLog.p1_init_current);
    query.bindValue(":p2_v",waferLog.p2_init_volt);
    query.bindValue(":p2_i",waferLog.p2_init_current);
    if(query.exec()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Wafer: "<<waferLog.waferId<<" initial data updated";
        emit this->databaseMessage(message);
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Wafer: "<<waferLog.waferId<<" initial data update failed. Error: ";
        buffer<<this->lastError;
        emit this->databaseError(message);
    }
}

void DatabaseLogger::updateFinal(const WaferLog& waferLog){
    QSqlQuery query;
    query.prepare(UPDATE_FINAL);
    query.bindValue(":wafer",waferLog.waferId);
    query.bindValue(":area1",waferLog.p1_area);
    query.bindValue(":area2",waferLog.p2_area);
    query.bindValue(":p1_v",waferLog.p1_final_volt);
    query.bindValue(":p1_i",waferLog.p1_final_current);
    query.bindValue(":p2_v",waferLog.p2_final_volt);
    query.bindValue(":p2_i",waferLog.p2_final_current);
    if(query.exec()){
        QString message;
        QTextStream buffer(&message);
        buffer<<"Wafer: "<<waferLog.waferId<<" final data updated"<<endl;
        emit this->databaseMessage(message);
    }else{
        this->lastError=query.lastError().text();
        QString message;
        QTextStream buffer(&message);
        buffer<<"Wafer: "<<waferLog.waferId<<" final data update failed. Error: ";
        buffer<<this->lastError;
        emit this->databaseError(message);
    }
}

void DatabaseLogger::parseControlValues(const ControlValues &data,bool initial){
    if(initial){
        this->testLog.pocket1.p1_init_volt=data.v11;
        this->testLog.pocket1.p1_init_current=data.i11;

        this->testLog.pocket1.p2_init_volt=data.v12;
        this->testLog.pocket1.p2_init_current=data.i12;

        this->testLog.pocket2.p1_init_volt=data.v21;
        this->testLog.pocket2.p1_init_current=data.i21;

        this->testLog.pocket2.p2_init_volt=data.v22;
        this->testLog.pocket2.p2_init_current=data.i22;

        this->testLog.pocket3.p1_init_volt=data.v31;
        this->testLog.pocket3.p1_init_current=data.i31;

        this->testLog.pocket3.p2_init_volt=data.v32;
        this->testLog.pocket3.p2_init_current=data.i32;
    }else{
        this->testLog.pocket1.p1_final_volt=data.v11;
        this->testLog.pocket1.p1_final_current=data.i11;

        this->testLog.pocket1.p2_final_volt=data.v12;
        this->testLog.pocket1.p2_final_current=data.i12;

        this->testLog.pocket2.p1_final_volt=data.v21;
        this->testLog.pocket2.p1_final_current=data.i21;

        this->testLog.pocket2.p2_final_volt=data.v22;
        this->testLog.pocket2.p2_final_current=data.i22;

        this->testLog.pocket3.p1_final_volt=data.v31;
        this->testLog.pocket3.p1_final_current=data.i31;

        this->testLog.pocket3.p2_final_volt=data.v32;
        this->testLog.pocket3.p2_final_current=data.i32;
    }
}





