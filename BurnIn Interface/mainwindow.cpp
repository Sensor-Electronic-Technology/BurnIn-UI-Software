#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
	this->arduino=new Arduino(this);
	this->logger=new Logger(this);
    this->database=new DatabaseLogger(this);
    this->database->InitConnection();
	this->settingDialog=new ConfigSettingDialog();
    this->setupLedIndicators();
    this->setupTestInfo();
    this->initAppSettings();
    this->firstLog=false;
    this->lastLog=true;
    this->resetLatch=false;
    this->stopLatch=false;
    this->startPressed=false;

	connect(this->arduino,&Arduino::criticalError,this,&MainWindow::recieveCriticalError);
	connect(this->arduino,&Arduino::dataUpdate,this,&MainWindow::updateUI);
	connect(this->arduino,&Arduino::comUpdate,this,&MainWindow::recieveComMessage);
	connect(this->arduino,&Arduino::portNameUpdate,this,&MainWindow::recievePortName);
	connect(this->logger,&Logger::publishFileError,this,&MainWindow::recieveFileError);
	connect(this->ui->actionOpen_Settings,&QAction::triggered,this->settingDialog,&ConfigSettingDialog::show);
	connect(this->settingDialog,&ConfigSettingDialog::settingsUpdate,this,&MainWindow::updateDeviceSettings);
    connect(this->database,&DatabaseLogger::databaseError,this,&MainWindow::recieveDatabaseError);
    emit this->ui->statusDisplay->setText("Idle");
}

MainWindow::~MainWindow(){
	if(this->arduino!=nullptr){
		delete this->arduino;
	}
	if(this->logger!=nullptr){
		delete this->arduino;
	}
    if(this->database!=nullptr){
        delete this->database;
    }
    delete ui;
}

void MainWindow::setupTestInfo(){
    this->ui->pocket1P1Area->addItems(AreaList);
    this->ui->pocket1P2Area->addItems(AreaList);
    this->ui->pocket2P1Area->addItems(AreaList);
    this->ui->pocket2P2Area->addItems(AreaList);
    this->ui->pocket3P1Area->addItems(AreaList);
    this->ui->pocket3P2Area->addItems(AreaList);
    this->resetAllPockets();
}

void MainWindow::initAppSettings(){
    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    qDebug()<<dataLocation;
    QString filename=dataLocation+"/burninsettings.ini";
    this->testLogFileName=dataLocation+"/testLogTracking.txt";
    QSettings settings(filename,QSettings::NativeFormat);
    settings.sync();
    this->appSettings.switchingEnabled=settings.value("switching_enabled",false).toBool();
    this->appSettings.id=settings.value("id",0).toInt();
    this->appSettings.defaultCurrent=settings.value("defaultCurrent",150).toInt();
    this->appSettings.setTemperature=settings.value("setTemperature",85).toInt();
    this->stationId="Station"+QString::number(this->appSettings.id);
}

void MainWindow::logRunningTest(){
    TestInfo testInfo=this->database->getTestInfo();
    QSettings testLog(this->testLogFileName,QSettings::NativeFormat);
    testLog.setValue("logName",testInfo.logName);
    testLog.setValue("wafer1",testInfo.pocket1.waferId);
    testLog.setValue("wafer2",testInfo.pocket2.waferId);
    testLog.setValue("wafer3",testInfo.pocket3.waferId);
    testLog.setValue("w1a1",QString(testInfo.pocket1.p1_area));
    testLog.setValue("w1a2",QString(testInfo.pocket1.p2_area));
    testLog.setValue("w2a1",QString(testInfo.pocket2.p1_area));
    testLog.setValue("w2a2",QString(testInfo.pocket2.p2_area));
    testLog.setValue("w3a1",QString(testInfo.pocket3.p1_area));
    testLog.setValue("w3a2",QString(testInfo.pocket3.p2_area));
    testLog.setValue("setCurrent",testInfo.set_current);
    testLog.setValue("setTemp",testInfo.set_temp);
    testLog.sync();
    time(&this->lastTestLogWrite);
}

TestInfo MainWindow::checkRunningTest(){
    qDebug()<<"Reading TestLog, should I be here"<<endl;
    TestInfo testInfo;
    QSettings testLog(this->testLogFileName,QSettings::NativeFormat);
    testLog.sync();
    testInfo.logName=testLog.value("logName","").toString();
    testInfo.pocket1.waferId=testLog.value("wafer1","").toString();
    testInfo.pocket2.waferId=testLog.value("wafer2","").toString();
    testInfo.pocket3.waferId=testLog.value("wafer3","").toString();     
    testInfo.pocket1.p1_area=testLog.value("w1a1","").toString()[0];
    testInfo.pocket1.p2_area=testLog.value("w1a2","").toString()[0];
    testInfo.pocket2.p1_area=testLog.value("w2a1","").toString()[0];
    testInfo.pocket2.p2_area=testLog.value("w2a2","").toString()[0];
    testInfo.pocket3.p1_area=testLog.value("w3a1","").toString()[0];
    testInfo.pocket3.p2_area=testLog.value("w3a2","").toString()[0];
    testInfo.set_current=testLog.value("setCurrent",0).toInt();
    testInfo.set_temp=testLog.value("setTemp",0).toInt();
    return testInfo;
}

void MainWindow::continueTest(){
    TestInfo testInfo=this->checkRunningTest();
    if(this->database->logFileExists(testInfo.logName)){
        if(!this->database->logCompleted(testInfo.logName)){
            this->database->setTest(testInfo);
            this->logger->InitLogger(testInfo.logName);
            this->logRunningTest();
            this->setTestInfo(testInfo);
            this->firstLog=true;
        }
    }else{
        qDebug()<<"Failed to find logName: "<<testInfo.logName<<endl;
        QString filename=this->ui->filenameBox->text();
        if(filename.isEmpty()){
            filename=QDateTime::currentDateTime().toString();
        }
        emit this->ui->filenameBox->setText(filename);
        emit this->logger->InitLogger(filename);
    }
}

void MainWindow::setTestInfo(const TestInfo& testInfo){
    emit this->ui->filenameBox->setText(testInfo.logName);
    emit this->ui->pocket1Wafer->setText(testInfo.pocket1.waferId);
    emit this->ui->pocket2Wafer->setText(testInfo.pocket2.waferId);
    emit this->ui->pocket3Wafer->setText(testInfo.pocket3.waferId);

    if(testInfo.pocket1.p1_area=='n'){
        emit this->ui->pocket1P1Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket1P1Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket1.p1_area)));
    }

    if(testInfo.pocket1.p2_area=='n'){
        emit this->ui->pocket1P2Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket1P2Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket1.p2_area)));
    }


    if(testInfo.pocket2.p1_area=='n'){
        emit this->ui->pocket2P1Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket2P1Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket2.p1_area)));
    }
    if(testInfo.pocket2.p2_area=='n'){
        emit this->ui->pocket2P2Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket2P2Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket2.p2_area)));
    }


    if(testInfo.pocket3.p1_area=='n'){
        emit this->ui->pocket3P1Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket3P1Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket3.p1_area)));
    }
    if(testInfo.pocket3.p2_area=='n'){
        emit this->ui->pocket3P2Area->setCurrentIndex(AreaList.indexOf("none"));
    }else{
        emit this->ui->pocket3P2Area->setCurrentIndex(AreaList.indexOf(QString(testInfo.pocket3.p2_area)));
    }
}

void MainWindow::clearRunningTest(){
    TestInfo testInfo=this->database->getTestInfo();
    QSettings testLog(this->testLogFileName,QSettings::NativeFormat);
    testLog.setValue("logName","");
    testLog.setValue("wafer1","");
    testLog.setValue("wafer2","");
    testLog.setValue("wafer3","");
    testLog.setValue("w1a1","");
    testLog.setValue("w1a2","");
    testLog.setValue("w2a1","");
    testLog.setValue("w2a2","");
    testLog.setValue("w3a1","");
    testLog.setValue("w3a2","");
    testLog.setValue("setCurrent","");
    testLog.setValue("setTemp","");
    testLog.sync();
}

void MainWindow::setupLedIndicators(){
    this->indicators=new Indicator[3];
    this->indicators[0]=new LedIndicator();
    this->indicators[1]=new LedIndicator();
    this->indicators[2]=new LedIndicator();
    this->waferIndicators=new Indicator[3];
    this->waferIndicators[0]=new LedIndicator();
    this->waferIndicators[1]=new LedIndicator();
    this->waferIndicators[2]=new LedIndicator();
    this->ui->gridLayout->addWidget(this->indicators[0],1,0);
    this->ui->gridLayout->addWidget(this->indicators[1],1,1);
    this->ui->gridLayout->addWidget(this->indicators[2],1,2);
    this->ui->waferSuccessP1->addWidget(this->waferIndicators[0],1,0);
    this->ui->waferSuccessP2->addWidget(this->waferIndicators[1],1,0);
    this->ui->waferSuccessP3->addWidget(this->waferIndicators[2],1,0);
    this->indicators[0]->setLedSize(20);
    this->indicators[1]->setLedSize(20);
    this->indicators[2]->setLedSize(20);
    this->waferIndicators[0]->setLedSize(25);
    this->waferIndicators[1]->setLedSize(25);
    this->waferIndicators[2]->setLedSize(25);
    this->ui->gridLayout->setAlignment(this->indicators[0],Qt::AlignHCenter);
    this->ui->gridLayout->setAlignment(this->indicators[1],Qt::AlignHCenter);
    this->ui->gridLayout->setAlignment(this->indicators[2],Qt::AlignHCenter);
    this->ui->waferSuccessP1->setAlignment(this->waferIndicators[0],Qt::AlignHCenter);
    this->ui->waferSuccessP2->setAlignment(this->waferIndicators[1],Qt::AlignHCenter);
    this->ui->waferSuccessP3->setAlignment(this->waferIndicators[2],Qt::AlignHCenter);
    this->indicators[0]->setState(false);
    this->indicators[1]->setState(false);
    this->indicators[2]->setState(false);
    this->waferIndicators[0]->setState(false);
    this->waferIndicators[1]->setState(false);
    this->waferIndicators[2]->setState(false);
    this->waferIndicators[0]->setOffColor(QColor::fromRgb(255,0,0));
    this->waferIndicators[1]->setOffColor(QColor::fromRgb(255,0,0));
    this->waferIndicators[2]->setOffColor(QColor::fromRgb(255,0,0));
}

void MainWindow::updateUI(const ControlValues &data){

	emit this->ui->v11_display->display(QString::number(data.v11,'f',1));

	emit this->ui->v12_display->display(QString::number(data.v12,'f',1));
	emit this->ui->v21_display->display(QString::number(data.v21,'f',1));
	emit this->ui->v22_display->display(QString::number(data.v22,'f',1));
	emit this->ui->v31_display->display(QString::number(data.v31,'f',1));
	emit this->ui->v32_display->display(QString::number(data.v32,'f',1));


	emit this->ui->i11_display->display(QString::number(data.i11,'f',1));
	emit this->ui->i12_display->display(QString::number(data.i12,'f',1));
	emit this->ui->i21_display->display(QString::number(data.i21,'f',1));
	emit this->ui->i22_display->display(QString::number(data.i22,'f',1));
	emit this->ui->i31_display->display(QString::number(data.i31,'f',1));
	emit this->ui->i32_display->display(QString::number(data.i32,'f',1));

    emit this->ui->currentToggle->setText(QString::number(data.currentSP));
	emit this->ui->tempToggle->setText(QString::number(data.temperatureSP));
	emit this->ui->temp1Displa->display(QString::number(data.t1,'f',1));
	emit this->ui->temp2_displa->display(QString::number(data.t2,'f',1));
	emit this->ui->temp3_displa->display(QString::number(data.t3,'f',1));

	this->indicators[0]->setState(data.heating1);
	this->indicators[1]->setState(data.heating2);
	this->indicators[2]->setState(data.heating3);
	emit this->ui->elapsedTimeBox->setText(data.elapsedTime);
	AppState newState=AppState::IDLE;
	if(data.running && !data.paused){
		newState=AppState::RUNNING;
	}else if(data.running && data.paused){
		newState=AppState::PAUSED;
	}else if(!data.running && !data.paused){
		newState=AppState::IDLE;
	}
	emit this->transitionState(newState,data);
}

void MainWindow::transitionState(AppState newState,const ControlValues &data){
    if(this->appState!=newState){
        switch(this->appState){
            case AppState::IDLE:{
                if(newState==AppState::RUNNING){
                    emit this->ui->statusDisplay->setText("Running");
                    emit this->ui->startB->setText("Pause");
                    if(this->startPressed){
                        this->startPressed=false;
                        this->startNewTest(data.currentSP,data.temperatureSP);
                    }else{
                        this->continueTest();
                    }
                    this->appState=newState;
                    qDebug()<<"IDLE to RUNNING"<<endl;
                }else if(newState==AppState::PAUSED){
                    this->appState=newState;
                    emit this->ui->statusDisplay->setText("Paused");
                    emit this->ui->startB->setText("Continue");
                    this->continueTest();
                    this->appState=newState;
                    qDebug()<<"IDLE to PAUSED"<<endl;
                }
                break;
            }
            case AppState::RUNNING:{
                if(newState==AppState::PAUSED){
                    emit this->ui->startB->setText("Continue");
                    emit this->ui->statusDisplay->setText("Paused");
                    this->appState=newState;
                    qDebug()<<"RUNNING to PAUSED"<<endl;
                }else if(newState==AppState::IDLE){                    
                    emit this->ui->statusDisplay->setText("Idle");
                    emit this->ui->startB->setText("Start Burn-In");
                    this->appState=newState;
                    qDebug()<<"RUNNING to IDLE"<<endl;
                    if(this->resetLatch){
                        this->stopLatch=false;
                        this->resetLatch=false;
                        this->firstLog=false;
                        this->lastLog=false;
                        this->database->deleteLog();
                        this->database->clearTest();
                        this->clearRunningTest();
                        qDebug()<<"RUNNING to RESET"<<endl;
                    }else{
                        this->resetLatch=false;
                        this->stopLatch=false;
                        this->firstLog=false;
                        this->lastLog=false;
                        this->database->logStop();
                        this->database->clearTest();
                        this->clearRunningTest();
                        qDebug()<<"RUNNING to STOPPED"<<endl;
                    }

                    if(this->stopLatch){

                    }
                }
                break;
            }
            case AppState::PAUSED:{
                if(newState==AppState::RUNNING){             
                    emit this->ui->statusDisplay->setText("Running");
                    emit this->ui->startB->setText("Pause");
                    this->appState=newState;
                    qDebug()<<"PAUSED to RUNNING"<<endl;
                }else if(newState==AppState::IDLE){
                    emit this->ui->statusDisplay->setText("Idle");
                    emit this->ui->startB->setText("Start Burn-In");
                    this->appState=newState;
                    qDebug()<<"PAUSED to IDLE"<<endl;
                    if(this->resetLatch){
                        this->resetLatch=false;
                        this->stopLatch=false;
                        this->firstLog=false;
                        this->lastLog=false;
                        emit this->database->deleteLog();
                        this->database->clearTest();
                        this->clearRunningTest();
                        qDebug()<<"RESET"<<endl;
                    }
                }
                break;
            }
        }
    }else{
        if(appState==AppState::RUNNING){
            if(!this->firstLog){
                if(data.elapsed>=FirstLogTime){
                    qDebug()<<"Logging Initial"<<endl;
                    emit this->logger->writeOutData(data.logtext);
                    TestInfo testInfo=this->database->getTestInfo();
                    qDebug()<<"Log Name: "<<testInfo.logName<<endl;
                    qDebug()<<"Wafer1: "<<testInfo.pocket1.waferId;
                    qDebug()<<"A1: "<<QString(testInfo.pocket1.p1_area)<<endl;
                    qDebug()<<"A1 V: "<<data.v11<<endl;
                    emit this->database->logInitial(data);
                    this->firstLog=true;
                }
            }else{
                emit this->logger->writeOutData(data.logtext);
                if(!this->lastLog){
                    if((data.runTime-data.elapsed)<=LastLogDelta){
                        qDebug()<<"Logged Last??? why"<<endl;
                        qDebug()<<"runtime: "<<data.runTime<<" elapsed: "<<data.elapsed<<" runTime-elapsed: "<<data.runTime-data.elapsed<<endl;
                        emit this->database->logFinal(data);
                        this->lastLog=true;
                    }
                }
            }
            double diff=difftime(time(nullptr),this->lastTestLogWrite);
            if(diff>=TestLogInterval){
                time(&this->lastTestLogWrite);
                this->logRunningTest();
            }
        }
    }
}

void MainWindow::startNewTest(double setCurrent,double setTemp){
    qDebug()<<"Starting test"<<endl;
    emit this->ui->statusDisplay->setText("Running");
    emit this->ui->startB->setText("Pause");
    emit this->logger->InitLogger(this->database->getTestInfo().logName);
    this->database->updateTestSetting(int(setCurrent),int(setTemp));
    emit this->database->logStart();
    this->logRunningTest();
}

void MainWindow::createTest(){
    if(this->appState==AppState::IDLE){
        QString w1=this->ui->pocket1Wafer->text();
        QString w2=this->ui->pocket2Wafer->text();
        QString w3=this->ui->pocket3Wafer->text();
        QString p1a1,p1a2,p2a1,p2a2,p3a1,p3a2;

        p1a1=this->ui->pocket1P1Area->currentText();
        p1a2=this->ui->pocket1P2Area->currentText();

        p2a1=this->ui->pocket2P1Area->currentText();
        p2a2=this->ui->pocket2P2Area->currentText();

        p3a1=this->ui->pocket3P1Area->currentText();
        p3a2=this->ui->pocket3P2Area->currentText();

        bool w1Okay,w2Okay,w3Okay=false;

        if(w1=="none"){
            w1Okay=true;
            this->waferIndicators[0]->setState(w1Okay);
        }else{
            w1Okay=this->database->waferExists(w1);
            this->waferIndicators[0]->setState(w1Okay);
        }

        if(w2=="none"){
            w2Okay=true;
            this->waferIndicators[1]->setState(w2Okay);
        }else{
            w2Okay=this->database->waferExists(w2);
            this->waferIndicators[1]->setState(w2Okay);
        }

        if(w3=="none"){
            w3Okay=true;
            this->waferIndicators[2]->setState(w3Okay);
        }else{
            w3Okay=this->database->waferExists(w3);
            this->waferIndicators[2]->setState(w3Okay);
        }

        if(w1Okay && w2Okay && w3Okay){
            QString append=this->ui->appendFileName->text();
            QString filename=w1+p1a1[0]+p1a2[0]+"_"+w2+p2a1[0]+p2a2[0]+"_"+w3+p3a1[0]+p3a2[0];

            if(!append.isEmpty()){
                filename+="_"+append;
            }

            filename+="_Station"+QString::number(this->appSettings.id);

            if(this->database->logFileExists(filename)){
                QMessageBox msgBox;
                emit msgBox.setText("Filename already exists, please check name then try again");
            }else{
                TestInfo testInfo;
                testInfo.logName=filename;
                testInfo.pocket1.waferId=w1;
                testInfo.pocket1.p1_area=p1a1[0];
                testInfo.pocket1.p2_area=p1a2[0];

                testInfo.pocket2.waferId=w2;
                testInfo.pocket2.p1_area=p2a1[0];
                testInfo.pocket2.p2_area=p2a2[0];

                testInfo.pocket3.waferId=w3;
                testInfo.pocket3.p1_area=p3a1[0];
                testInfo.pocket3.p2_area=p3a2[0];
                testInfo.stationId=this->appSettings.id;
                this->database->setTest(testInfo);
                emit this->ui->filenameBox->setText(testInfo.logName);
            }
        }else{
            QMessageBox msgBox;
            msgBox.setText("Error in wafer input.  Check check input then try again");
            emit msgBox.exec();
        }
    }else{
        QMessageBox msgBox;
        msgBox.setText("A test is currently running or paused.  Cannot create a new test "
                       "until current test is finished");
        emit msgBox.exec();
    }
}

void MainWindow::updateDeviceSettings(const ApplicationSettings &settings){
    if(this->arduino->isConnected()){
        QString buffer;
        QTextStream stream(&buffer);
        this->appSettings=settings;
        this->stationId="Station"+QString::number(settings.id);
        int enabled=(int)settings.switchingEnabled;
        stream<<"U"<<QString::number(enabled);
        if(settings.defaultCurrent==60){
            stream<<"0"<<QString::number(settings.defaultCurrent);
        }else{
            stream<<QString::number(settings.defaultCurrent);
        }
        stream<<QString::number(settings.setTemperature);
        emit this->arduino->sendFirmwareUpdate(buffer.toUtf8());
        qDebug()<<buffer;
    }else{
        emit this->recieveCriticalError("Error: device must be connected");
    }
}

void MainWindow::resetPocket1(){
    emit this->ui->pocket1Wafer->setText("none");
    emit this->ui->pocket1P1Area->setCurrentIndex(AreaList.indexOf("none"));
    emit this->ui->pocket1P2Area->setCurrentIndex(AreaList.indexOf("none"));
    this->waferIndicators[0]->setState(false);
}

void MainWindow::resetPocket2(){
    emit this->ui->pocket2Wafer->setText("none");
    emit this->ui->pocket2P1Area->setCurrentIndex(AreaList.indexOf("none"));
    emit this->ui->pocket2P2Area->setCurrentIndex(AreaList.indexOf("none"));
    this->waferIndicators[1]->setState(false);
}

void MainWindow::resetPocket3(){
    emit this->ui->pocket3Wafer->setText("none");
    emit this->ui->pocket3P1Area->setCurrentIndex(AreaList.indexOf("none"));
    emit this->ui->pocket3P2Area->setCurrentIndex(AreaList.indexOf("none"));
    this->waferIndicators[2]->setState(false);
}

void MainWindow::resetAllPockets(){
    resetPocket1();
    resetPocket2();
    resetPocket3();
}

void MainWindow::recieveCriticalError(const QString &errorMessage){
	QMessageBox msgBox;
	msgBox.setText(errorMessage);
	msgBox.setWindowTitle("Critical Error");
	msgBox.exec();
}

void MainWindow::recieveFileError(const QString &errorMessage){
	this->ui->comMessages->append(QDateTime::currentDateTime().toString()+":"+errorMessage);
	this->ui->comMessages->verticalScrollBar()->setValue(this->ui->comMessages->verticalScrollBar()->maximum());
}

void MainWindow::recieveComMessage(const QString &comMessage){
	this->ui->comMessages->append(QDateTime::currentDateTime().toString()+":"+comMessage);
	this->ui->comMessages->verticalScrollBar()->setValue(this->ui->comMessages->verticalScrollBar()->maximum());
}

void MainWindow::recievePortName(const QString &portName){
	this->ui->portName->setText(portName);
}

void MainWindow::recieveDatabaseError(const QString& message){
    this->ui->comMessages->append(QDateTime::currentDateTime().toString()+":"+message);
    this->ui->comMessages->verticalScrollBar()->setValue(this->ui->comMessages->verticalScrollBar()->maximum());
}

void MainWindow::recieveDatabaseMessage(const QString& message){
    this->ui->comMessages->append(QDateTime::currentDateTime().toString()+":"+message);
    this->ui->comMessages->verticalScrollBar()->setValue(this->ui->comMessages->verticalScrollBar()->maximum());
}

void MainWindow::on_connectB_clicked(){
	emit this->arduino->serialConnect();
}

void MainWindow::on_startB_clicked(){
	switch(this->appState){
		case AppState::IDLE:{
            if(!this->ui->filenameBox->text().isEmpty()){
                this->startPressed=true;
				emit this->arduino->sendCommand(SerialCommand::START);
			}else{
				QMessageBox msgBox;
				msgBox.setText("Error: Filename is empty, please enter a filename and try again");
				msgBox.setWindowTitle("No Filename");
				emit msgBox.exec();
			}
			break;
		}
		case AppState::RUNNING:{
			emit this->arduino->sendCommand(SerialCommand::PAUSE);
			break;
		}
		case AppState::PAUSED:{
			emit this->arduino->sendCommand(SerialCommand::PAUSE);
			break;
		}
	}
}

void MainWindow::on_currentToggle_clicked(){
	emit this->arduino->sendCommand(SerialCommand::SWITCHCURRENT);
}

void MainWindow::on_tempToggle_clicked(){
	emit this->arduino->sendCommand(SerialCommand::SETTEMP);
}

void MainWindow::on_testContactB_clicked(){
	emit this->arduino->sendCommand(SerialCommand::TESTPROBE);
}

void MainWindow::on_resetDeviceB_clicked(){
	if(this->appState!=AppState::IDLE){
		QMessageBox msgBox;
        msgBox.setText("Warning: Test currently running. If you continue test will stop delete log. \n Continue?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
        msgBox.setWindowTitle("!!Waring!!");
		auto ret=msgBox.exec();
		if(ret==QMessageBox::Yes){
			this->appState=AppState::IDLE;
			emit this->ui->startB->setText("Start Burn-in");
			emit this->ui->statusDisplay->setText("Idle");
            emit this->arduino->sendCommand(SerialCommand::RESET);
            this->resetLatch=true;
		}
	}else{
		this->appState=AppState::IDLE;
		emit this->ui->startB->setText("Start Burn-in");
		emit this->ui->statusDisplay->setText("Idle");
		emit this->arduino->sendCommand(SerialCommand::RESET);
	}
}

void MainWindow::on_checkWaferP1_clicked(){
    bool exist=this->database->waferExists(this->ui->pocket1Wafer->text());
    this->waferIndicators[0]->setState(exist);
}

void MainWindow::on_checkWaferP2_clicked(){
    bool exist=this->database->waferExists(this->ui->pocket2Wafer->text());
    this->waferIndicators[1]->setState(exist);
}

void MainWindow::on_checkWaferp3_clicked(){
    bool exist=this->database->waferExists(this->ui->pocket3Wafer->text());
    this->waferIndicators[2]->setState(exist);
}

void MainWindow::on_createTest_clicked(){
    this->createTest();
}

void MainWindow::on_resetP1_clicked(){
    this->resetPocket1();
}

void MainWindow::on_resetP2_clicked(){
    this->resetPocket2();
}

void MainWindow::on_resetP3_clicked(){
    this->resetPocket3();
}

void MainWindow::on_resetP2_2_clicked(){
    this->resetAllPockets();
}
