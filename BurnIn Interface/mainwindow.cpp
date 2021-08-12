#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
	this->arduino=new Arduino(this);
	this->logger=new Logger(this);
	this->settingDialog=new ConfigSettingDialog();
	this->indicators=new Indicator[3];
	this->indicators[0]=new LedIndicator();
	this->indicators[1]=new LedIndicator();
	this->indicators[2]=new LedIndicator();
	this->ui->gridLayout->addWidget(this->indicators[0],1,0);
	this->ui->gridLayout->addWidget(this->indicators[1],1,1);
	this->ui->gridLayout->addWidget(this->indicators[2],1,2);
	this->indicators[0]->setLedSize(20);
	this->indicators[1]->setLedSize(20);
	this->indicators[2]->setLedSize(20);
	this->ui->gridLayout->setAlignment(this->indicators[0],Qt::AlignHCenter);
	this->ui->gridLayout->setAlignment(this->indicators[1],Qt::AlignHCenter);
	this->ui->gridLayout->setAlignment(this->indicators[2],Qt::AlignHCenter);
	this->indicators[0]->setState(false);
	this->indicators[1]->setState(false);
	this->indicators[2]->setState(false);
	connect(this->arduino,&Arduino::criticalError,this,&MainWindow::recieveCriticalError);
	connect(this->arduino,&Arduino::dataUpdate,this,&MainWindow::updateUI);
	connect(this->arduino,&Arduino::comUpdate,this,&MainWindow::recieveComMessage);
	connect(this->arduino,&Arduino::portNameUpdate,this,&MainWindow::recievePortName);
	connect(this->logger,&Logger::publishFileError,this,&MainWindow::recieveFileError);
	connect(this->ui->actionOpen_Settings,&QAction::triggered,this->settingDialog,&ConfigSettingDialog::show);
	connect(this->settingDialog,&ConfigSettingDialog::settingsUpdate,this,&MainWindow::updateDeviceSettings);
	emit this->ui->statusDisplay->setText("Idle");
	QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	qDebug()<<dataLocation;
	QString filename=dataLocation+"/burninsettings.ini";
	QSettings settings(filename,QSettings::NativeFormat);
	settings.sync();
	this->stationId="Station"+settings.value("id","notset").toString();
}

MainWindow::~MainWindow(){
	if(this->arduino!=nullptr){
		delete this->arduino;
	}
	if(this->logger!=nullptr){
		delete this->arduino;
	}
    delete ui;
}

void MainWindow::updateUI(const ControlValues &data){
	emit this->ui->v11_display->display(QString::number(data.v11,'f',1));
	emit this->ui->v12_display->display(QString::number(data.v12,'f',1));
	emit this->ui->v21_display->display(QString::number(data.v21,'f',1));
	emit this->ui->v22_display->display(QString::number(data.v22,'f',1));
	emit this->ui->v31_display->display(QString::number(data.v31,'f',1));
	emit this->ui->v32_display->display(QString::number(data.v32,'f',1));
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

void MainWindow::on_connectB_clicked(){
	emit this->arduino->serialConnect();
}

void MainWindow::on_startB_clicked(){
	switch(this->appState){
		case AppState::IDLE:{
			if(!this->ui->filenameBox->text().isEmpty()){
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
		msgBox.setText("Warning: Test currently running. If you continue test will stop. \n Continue?");
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::No);
		msgBox.setWindowTitle("Waring");
		auto ret=msgBox.exec();
		if(ret==QMessageBox::Yes){
			this->appState=AppState::IDLE;
			emit this->ui->startB->setText("Start Burn-in");
			emit this->ui->statusDisplay->setText("Idle");
			emit this->arduino->sendCommand(SerialCommand::RESET);
		}
	}else{
		this->appState=AppState::IDLE;
		emit this->ui->startB->setText("Start Burn-in");
		emit this->ui->statusDisplay->setText("Idle");
		emit this->arduino->sendCommand(SerialCommand::RESET);
	}
}

void MainWindow::transitionState(AppState newState,const ControlValues &data){
	if(this->appState!=newState){
		switch(this->appState){
			case AppState::IDLE:{
				if(newState==AppState::RUNNING){
					emit this->ui->statusDisplay->setText("Running");
					emit this->ui->startB->setText("Pause");
					QString filename=this->ui->filenameBox->text();
					if(filename.isEmpty()){
						filename=QDateTime::currentDateTime().toString();
					}
					emit this->logger->InitLogger(this->stationId+"_"+filename);
					this->appState=newState;
				}else if(newState==AppState::PAUSED){
					this->appState=newState;
					emit this->ui->startB->setText("Continue");
					emit this->ui->statusDisplay->setText("Paused");
				}
				break;
			}
			case AppState::RUNNING:{
				if(newState==AppState::PAUSED){
					this->appState=newState;
					emit this->ui->startB->setText("Continue");
					emit this->ui->statusDisplay->setText("Paused");
				}else if(newState==AppState::IDLE){
					this->appState=newState;
					emit this->ui->statusDisplay->setText("Idle");
					emit this->ui->startB->setText("Start Burn-In");
				}
				break;
			}
			case AppState::PAUSED:{
				if(newState==AppState::RUNNING){
					this->appState=newState;
					emit this->ui->statusDisplay->setText("Running");
					emit this->ui->startB->setText("Pause");
				}else if(newState==AppState::IDLE){
					this->appState=newState;
					emit this->ui->statusDisplay->setText("Idle");
					emit this->ui->startB->setText("Start Burn-In");
				}
				break;
			}
		}
	}else{
		if(appState==AppState::RUNNING){
			emit this->logger->writeOutData(data.logtext);
		}
	}
}

void MainWindow::updateDeviceSettings(const ApplicationSettings &settings){
	if(this->arduino->isConnected()){
		QString buffer;
		QTextStream stream(&buffer);
		this->stationId="Station"+settings.id;
		int enabled=(int)settings.switchingEnabled;
		stream<<"U"<<QString::number(enabled);
		stream<<QString::number(settings.analogVersion);
		stream<<QString::number(settings.voltageVersion);
		QString current;
		if(settings.current2=="none"){
			stream<<"000";
		}else{
			stream<<settings.current2;
		}
		stream<<QString::number(settings.setTemperature);

		emit this->arduino->sendFirmwareUpdate(buffer.toUtf8());
		qDebug()<<buffer;
	}else{
		emit this->recieveCriticalError("Error: device must be connected");
	}
}
