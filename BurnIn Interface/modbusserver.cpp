#include "modbusserver.h"

ModbusServer::ModbusServer(QObject *parent) : QObject(parent)
{

}

void ModbusServer::setupDeviceData(const ControlValues &data,const AppState &appState){
	if(!this->modbusDevice)
		return;

	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,0,static_cast<quint16>(data.v11*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,1,static_cast<quint16>(data.v12*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,2,static_cast<quint16>(data.v21*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,3,static_cast<quint16>(data.v22*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,4,static_cast<quint16>(data.v31*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,5,static_cast<quint16>(data.v32*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,6,static_cast<quint16>(data.t1*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,7,static_cast<quint16>(data.t2*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,8,static_cast<quint16>(data.t3*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,9,static_cast<quint16>(data.currentSP*100));
	this->modbusDevice->setData(QModbusDataUnit::InputRegisters,10,static_cast<quint16>(data.temperatureSP*100));
	std::vector<uint16_t> elapsedWords(data.elapsedTime.begin(), data.elapsedTime.end());
	quint16 i=11;
	for(auto value:elapsedWords){
		this->modbusDevice->setData(QModbusDataUnit::InputRegisters,i,static_cast<quint16>(value));
		i++;
	}
	this->modbusDevice->setData(QModbusDataUnit::Coils,0,appState);
	this->modbusDevice->setData(QModbusDataUnit::Coils,1,data.heating1);
	this->modbusDevice->setData(QModbusDataUnit::Coils,2,data.heating2);
	this->modbusDevice->setData(QModbusDataUnit::Coils,3,data.heating3);


}

