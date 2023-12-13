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
#include "databaselogger.h"
#include "probetracker.h"


#define realArrSize         100
#define boolArrSize         100
#define TestLogInterval     60

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
        void timer_update();
		void on_connectB_clicked();
		void on_startB_clicked();
		void on_currentToggle_clicked();
		void on_tempToggle_clicked();
		void on_testContactB_clicked();
		void on_resetDeviceB_clicked();
		void updateUI(const ControlValues &data);
		void recieveCriticalError(const QString &errorMessage);
		void recieveFileError(const QString &errorMessage);
		void recieveComMessage(const QString &comMessage);
		void recievePortName(const QString &portName);
        void recieveDatabaseError(const QString& message);
        void recieveDatabaseMessage(const QString& message);
		void updateDeviceSettings(const ApplicationSettings &settings);
		void transitionState(AppState newState,const ControlValues &data);
        void on_checkWaferP1_clicked();
        void on_checkWaferP2_clicked();
        void on_checkWaferp3_clicked();
        void on_createTest_clicked();
        void on_resetP1_clicked();
        void on_resetP2_clicked();
        void on_resetP3_clicked();
        void on_resetP2_2_clicked();
private:
		void startLogging(const QString &data);
        void createTest();
        void initAppSettings();
        void setupTestInfo();
        void startNewTest(double setCurrent,double setTemp);
        bool canStart();
        void setupLedIndicators();
        void resetPocket1();
        void resetPocket2();
        void resetPocket3();
        void resetAllPockets();
        void continueTest();
        void logRunningTest();
        void clearRunningTest();
        void setTestInfo(const TestInfo& testInfo);
        TestInfo checkRunningTest();
private:
        bool currentChanged=false;
        bool tempChanged=false;
        QString stationId="not set";
		AppState appState=AppState::IDLE;
		Arduino *arduino=nullptr;
		Logger *logger=nullptr;
        DatabaseLogger* database;
        ApplicationSettings appSettings;
		Ui::MainWindow *ui;
		ConfigSettingDialog *settingDialog;
		Indicator *indicators;
        Indicator *padIndicators;
        Indicator *waferIndicators;
        TestInfo currentTest;
        QString testLogFileName;
        QTimer* timer;
        time_t lastTestLogWrite;
        time_t lastPocketTime;
        PadStatus padStatus;
        ProbeTracker* probeTracker=nullptr;
        ControlValues dummyData;
        bool first=true;
        bool resetLatch;
        bool stopLatch;
        bool firstLog;
        bool lastLog;
        bool startPressed;
};

#endif // MAINWINDOW_H
