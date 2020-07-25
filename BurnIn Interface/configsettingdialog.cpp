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

ConfigSettingDialog::~ConfigSettingDialog()
{
	delete ui;
}

void ConfigSettingDialog::loadSettings(){
	QSettings settings(this->settingsFile,QSettings::NativeFormat);
	settings.sync();
	this->appSettings.switchingEnabled=settings.value("switching_enabled",false).toBool();
	this->appSettings.id=settings.value("id","0").toString();
	this->appSettings.ipAddress=settings.value("ipAddress","0.0.0.0").toString();
	this->appSettings.current2=settings.value("current2","120").toString();
	this->appSettings.setTemperature=settings.value("setTemperature",85).toInt();
	this->appSettings.analogVersion=settings.value("analogVersion",1).toInt();
	this->appSettings.voltageVersion=settings.value("voltageVersion",1).toInt();
	qDebug()<<"Current: "<<this->appSettings.current2<<" Version: "<<this->appSettings.analogVersion;
}


void ConfigSettingDialog::initGui(){
	QStringList list;
	list.append("120");
	list.append("060");
	list.append("none");
	this->ui->currentSelect2->addItems(list);
	emit this->ui->currentSelect2->setCurrentIndex(list.indexOf(this->appSettings.current2));
	emit this->ui->switchEnabledInput->setChecked(this->appSettings.switchingEnabled);
	emit this->ui->stationIdInput->setText(this->appSettings.id);
	emit this->ui->ipAddressInput->setText(this->appSettings.ipAddress);
	emit this->ui->analogVersionInput->setValue(this->appSettings.analogVersion);
	emit this->ui->setTemperatureInput->setValue(this->appSettings.setTemperature);
	emit this->ui->voltageVersionInput->setValue(this->appSettings.voltageVersion);
}

void ConfigSettingDialog::on_settingsButtons_accepted(){
	QSettings settings(this->settingsFile,QSettings::NativeFormat);
	this->appSettings.switchingEnabled=this->ui->switchEnabledInput->isChecked();
	this->appSettings.id=this->ui->stationIdInput->text();
	this->appSettings.ipAddress=this->ui->ipAddressInput->text();
	this->appSettings.current2=this->ui->currentSelect2->currentText();
	this->appSettings.analogVersion=this->ui->analogVersionInput->value();
	this->appSettings.setTemperature=this->ui->setTemperatureInput->value();
	this->appSettings.voltageVersion=this->ui->voltageVersionInput->value();
	settings.setValue("switching_enabled",this->appSettings.switchingEnabled);
	settings.setValue("id",this->appSettings.id);
	settings.setValue("ipAddress",this->appSettings.ipAddress);
	settings.setValue("current2",this->appSettings.current2);
	settings.setValue("setTemperature",this->appSettings.setTemperature);
	settings.setValue("analogVersion",this->appSettings.analogVersion);
	settings.setValue("voltageVersion",this->appSettings.voltageVersion);
	settings.sync();

	emit this->settingsUpdate(this->appSettings);
	emit this->hide();
}

void ConfigSettingDialog::on_settingsButtons_rejected(){
	emit this->hide();
}
