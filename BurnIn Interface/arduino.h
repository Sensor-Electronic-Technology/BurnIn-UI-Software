#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include "globaldatadefinitions.h"

class Arduino : public QObject
{
		Q_OBJECT
	public:
		explicit Arduino(QObject *parent = nullptr);
		bool FindArduinoSerialPort();
		bool isConnected();
		~Arduino();

	signals:
		void dataUpdate(const ControlValues &data);
		void comUpdate(const QString &comMessage);
		void criticalError(const QString &errorMessage);
		void logUpdate(const QString &data);
		void portNameUpdate(const QString &data);

	public slots:
		void readSerial();
		void sendCommand(SerialCommand command);
		void sendFirmwareUpdate(const QByteArray &settings);
		void serialConnect();
	private:
		void processLine(const QByteArray &buffer);
		static const qint16 arduinoVendorId1=9025;
		static const qint16 arduinoVendorId2=10755;
		QSerialPort *arduino=nullptr;
		bool connected=false;
		bool readEnabled=false;
		double realArray[100]={0};
		bool boolArray[100]={0};
};

#endif // ARDUINO_H
