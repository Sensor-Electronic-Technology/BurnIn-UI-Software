#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScrollBar>
#include <QByteArray>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>
#include "arduino.h"
#include "logger.h"
#include "globaldatadefinitions.h"
#include "configsettingdialog.h"
#include "ledindicator.h"

#define realArrSize 100
#define boolArrSize 100

namespace Ui {
	class MainWindow;
}

typedef LedIndicator* Indicator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();

	private slots:
		void on_connectB_clicked();
		void on_startB_clicked();
		void on_currentToggle_clicked();
		void on_tempToggle_clicked();
		void on_testContactB_clicked();
		void on_resetDeviceB_clicked();

		void updateUI(const ControlValues &data);
		void recieveCriticalError(const QString &errorMessage);
		void recieveComMessage(const QString &comMessage);
		void recievePortName(const QString &portName);
		void updateDeviceSettings(const ApplicationSettings &settings);
		void transitionState(AppState newState);


	private:
		void startLogging(const QString &data);
		bool currentChanged=false;
		bool tempChanged=false;
		QString stationId="not set";
		AppState appState=AppState::IDLE;
		Arduino *arduino=nullptr;
		Logger *logger=nullptr;
		Ui::MainWindow *ui;
		ConfigSettingDialog *settingDialog;
		Indicator *indicators;
		const QString localDirectory="~/Documents/Logs/";
		const QString defaultFilename="temp";

};

#endif // MAINWINDOW_H
