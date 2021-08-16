#ifndef MODBUSSERVER_H
#define MODBUSSERVER_H

#include <QModbusTcpServer>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QStatusBar>
#include <QUrl>
#include <QLineEdit>
#include "globaldatadefinitions.h"

class ModbusServer : public QObject
{
		Q_OBJECT
	public:
		explicit ModbusServer(QObject *parent = nullptr);

	signals:

	public slots:
		//void updateData(const ControlValues &data,const AppState &appState);
	private:
		void setupDeviceData(const ControlValues &data,const AppState &appState);
		QModbusServer *modbusDevice;
		vector<quint16> holdingRegisters;
		vector<quint16> inputRegisters;
		vector<bool>    coils;

};

#endif // MODBUSSERVER_H
