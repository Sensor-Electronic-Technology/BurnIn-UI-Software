#include "configsettingdialog.h"
#include "ui_configsettingdialog.h"

ConfigSettingDialog::ConfigSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ConfigSettingDialog)
{
	qRegisterMetaType<ApplicationSettings>();
	ui->setupUi(this);
	QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
	qDebug()<<dataLocation;
	this->settingsFile=dataLocation+"/burninsettings.ini";
	qDebug()<<"Settings File: "<<this->settingsFile<<endl;
	loadSettings();
	initGui();
}

ConfigSettingDialog::~ConfigSettingDialog(){
	delete ui;
}

void ConfigSettingDialog::loadSettings(){
	QSettings settings(this->settingsFile,QSettings::NativeFormat);
	settings.sync();
	this->appSettings.switchingEnabled=settings.value("switching_enabled",false).toBool();
    this->appSettings.id=settings.value("id",0).toInt();
    this->appSettings.defaultCurrent=settings.value("defaultCurrent",150).toInt();
	this->appSettings.setTemperature=settings.value("setTemperature",85).toInt();
}


void ConfigSettingDialog::initGui(){
    int index=this->ui->defaultCurrentSelect->findText(QString::number(this->appSettings.defaultCurrent));
    emit this->ui->defaultCurrentSelect->setCurrentIndex(index);
	emit this->ui->switchEnabledInput->setChecked(this->appSettings.switchingEnabled);
    emit this->ui->stationIdInput->setValue(this->appSettings.id);
	emit this->ui->setTemperatureInput->setValue(this->appSettings.setTemperature);
}

void ConfigSettingDialog::on_settingsButtons_accepted(){
	QSettings settings(this->settingsFile,QSettings::NativeFormat);
	this->appSettings.switchingEnabled=this->ui->switchEnabledInput->isChecked();
    this->appSettings.id=this->ui->stationIdInput->value();
    this->appSettings.defaultCurrent=this->ui->defaultCurrentSelect->currentText().toInt();
	this->appSettings.setTemperature=this->ui->setTemperatureInput->value();
	settings.setValue("switching_enabled",this->appSettings.switchingEnabled);
	settings.setValue("id",this->appSettings.id);
    settings.setValue("defaultCurrent",this->appSettings.defaultCurrent);
	settings.setValue("setTemperature",this->appSettings.setTemperature);
	settings.sync();
	emit this->settingsUpdate(this->appSettings);
	emit this->hide();
}

void ConfigSettingDialog::on_settingsButtons_rejected(){
	emit this->hide();
}
