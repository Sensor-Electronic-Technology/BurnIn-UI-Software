#include <EEPROM.h>
#include "BurnInHelper.h"

volatile boolean is150on = true; //false for 60mA operation, true for 120
int Current1 = 150;
int Current2 = 120;
bool SwitchingEnabled = false;
int AnalogVersion = 1;
volatile int SetCurrent = Current1;
volatile float tempSP = 0;
volatile float tempSetpoint = 85;
volatile boolean tempsOK = false;
volatile boolean firstRun = true;
volatile boolean paused = false;

volatile int heaterDuty1 = 100;
volatile float Duty1 = 100.0;
volatile float pTerm1 = 0.0;
volatile float err1 = 0.0;
volatile float iTerm1 = 0.0;
volatile float dTerm1 = 0.0;
volatile float lastErr1 = 0.0;
volatile int heaterDuty2 = 100;
volatile float Duty2 = 100.0;
volatile float pTerm2 = 0.0;
volatile float err2 = 0.0;
volatile float iTerm2 = 0.0;
volatile float dTerm2 = 0.0;
volatile float lastErr2 = 0.0;
volatile int heaterDuty3 = 100;
volatile float Duty3 = 100.0;
volatile float pTerm3 = 0.0;
volatile float err3 = 0.0;
volatile float iTerm3 = 0.0;
volatile float dTerm3 = 0.0;
volatile float lastErr3 = 0.0;
volatile float fWeight = 0.1;
volatile float Temp1 = tempSP;
volatile float Temp2 = tempSP;
volatile float Temp3 = tempSP;
volatile float V1 = 0;
volatile float V2 = 0;
volatile float V3 = 0;
volatile float V4 = 0;
volatile float V5 = 0;
volatile float V6 = 0;
volatile int aValue = 1024;
volatile unsigned long readTime = 0;
volatile boolean serialContact = false;
volatile boolean boolArray[100];
volatile float realArray[100];
String textArray[100];
volatile boolean limitArray[10];
volatile unsigned long burnInStartTime = 0;
volatile unsigned long burnInPauseTime = 0;
volatile unsigned long burnInTimeMillis = 0;
volatile unsigned long burnInTimeLength = BurnTime150;
volatile unsigned long lastDutyTime = 525;
volatile unsigned long lastRunTime = 50;
volatile unsigned long lastOutputTime = 50;
volatile unsigned long lastReadTime = 0;
volatile unsigned long comTime = 50;
volatile unsigned long lastComTime = 50;
volatile unsigned long eepromDelay = 500;
volatile unsigned long lastEEpromTime = 0;
volatile int i = 0;
volatile bool printVoltageError = false;
volatile bool firstTimeCheck = false;
volatile int settingsAddr=0;
volatile float systemVoltage = 105.00;
volatile uint8_t analog1 = PIN_A0;

volatile TestInfo runningTest;
volatile SystemSettings systemSettings;
volatile AnalogPins analogPins;

void(*resetFunc) (void) = 0; //declare reset function @ address 0

//Start Program Setup
void setup() {
	// Open serial communications
	loadFromMemory(true);
	Serial.begin(38400);
	#if DEBUG
	Serial.println("[T]{DEBUG MODE ENABLED}");
	#endif
	analogReference(EXTERNAL);
	pinMode(heatPin1, OUTPUT);  digitalWrite(heatPin1, LOW);
	pinMode(heatPin2, OUTPUT);  digitalWrite(heatPin2, LOW);
	pinMode(heatPin3, OUTPUT);  digitalWrite(heatPin3, LOW);
	pinMode(ledPin, OUTPUT);  digitalWrite(ledPin, LOW);
	pinMode(led150Pin, OUTPUT);
	if (is150on) {
		digitalWrite(led150Pin, HIGH);
		burnInTimeLength = BurnTime150;
		SetCurrent = Current1;
	} else {
		digitalWrite(led150Pin, LOW);
		burnInTimeLength = BurnTime120;
		SetCurrent = Current2;
	}

	pinMode(TempPin1, INPUT);
	pinMode(TempPin2, INPUT);
	pinMode(TempPin3, INPUT);

	tempSP = 0;
	heaterDuty1 = 100;
	Duty1 = 100.0;
	pTerm1 = 0.0;
	err1 = 0.0;
	iTerm1 = 0.0;
	dTerm1 = 0.0;
	lastErr1 = 0.0;
	heaterDuty2 = 100;
	Duty2 = 100.0;
	pTerm2 = 0.0;
	err2 = 0.0;
	iTerm2 = 0.0;
	dTerm2 = 0.0;
	lastErr2 = 0.0;
	heaterDuty3 = 100;
	Duty3 = 100.0;
	pTerm3 = 0.0;
	err3 = 0.0;
	iTerm3 = 0.0;
	dTerm3 = 0.0;
	lastErr3 = 0.0;

	//initialize temperature values
	aValue = analogRead(TempPin1);
	Temp1 = (((float)(aValue) * 500.0) / 1024.0);

	aValue = analogRead(TempPin2);
	Temp2 = (((float)(aValue) * 500.0) / 1024.0);

	aValue = analogRead(TempPin3);
	Temp3 = (((float)(aValue) * 500.0) / 1024.0);

	for (int i = 0; i < 100; i++) {
		readInputs();
		delay(10);
	}
	readTime = 100;
	for (int c = 0; c <= 5; c++) {
		limitArray[c] = true;
	}
}

void loop() {
	checkStart();
	readInputs();
	setHeaterDuty();
	setOutputs();
	sendComs_v2();
	if (is150on) {
		digitalWrite(led150Pin, HIGH);
		runningTest.is150 = true;
	} else {
		digitalWrite(led150Pin, LOW);
		runningTest.is150 = false;
	}
	if ((digitalRead(ledPin)) && !paused) {
		burnInTimeMillis = millis() - burnInStartTime;
		runningTest.elapsed = burnInTimeMillis;
		runningTest.running = true;
		runningTest.paused = false;
		realArray[9] = (float)(burnInTimeMillis / 1000.0);
		if (burnInTimeMillis >= burnInTimeLength) {
			digitalWrite(ledPin, LOW);
			tempSP = 0;
			runningTest.elapsed = 0;
			runningTest.is150 = is150on;
			runningTest.running = false;
			runningTest.paused = false;
			EEPROM_write(0, runningTest);
			Serial.println("[T]{Burn-In Complete.  Heaters Off}");
			Serial.println("[T]{Reset before starting next Burn-In}");
		}
	}
}

void readInputs() {
	if (millis() >= (lastReadTime + readTime)) {
		lastReadTime = lastReadTime + readTime;

		aValue = analogRead(TempPin1);
		delay(readDelay);

		Temp1 = Temp1 + ((((float)(aValue) * 500.0) / 1024.0) + tempOffset - Temp1) * fWeight;

		aValue = analogRead(TempPin2);
		delay(readDelay);

		Temp2 = Temp2 + ((((float)(aValue) * 500.0) / 1024.0) + tempOffset - Temp2) * fWeight;

		aValue = analogRead(TempPin3); delay(readDelay);

		Temp3 = Temp3 + ((((float)(aValue) * 500.0) / 1024.0) + tempOffset - Temp3) * fWeight;

		aValue = analogRead(analogPins.a0); delay(readDelay);

		V1 = V1 + ((((float)(aValue) * systemVoltage) / 1024.0) - V1) * fWeight;

		aValue = analogRead(analogPins.a1); delay(readDelay);

		V2 = V2 + ((((float)(aValue) * systemVoltage) / 1024.0) - V2) * fWeight;

		aValue = analogRead(analogPins.a2); delay(readDelay);

		V3 = V3 + ((((float)(aValue) * systemVoltage) / 1024.0) - V3) * fWeight;

		aValue = analogRead(analogPins.a3); delay(readDelay);

		V4 = V4 + ((((float)(aValue) * systemVoltage) / 1024.0) - V4) * fWeight;

		aValue = analogRead(analogPins.a4); delay(readDelay);

		V5 = V5 + ((((float)(aValue) * systemVoltage) / 1024.0) - V5) * fWeight;

		aValue = analogRead(analogPins.a5); delay(readDelay);

		V6 = V6 + ((((float)(aValue) * systemVoltage) / 1024.0) - V6) * fWeight;

		boolArray[0] = digitalRead(ledPin) || paused;
		boolArray[1] = digitalRead(heatPin1);
		boolArray[2] = digitalRead(heatPin2);
		boolArray[3] = digitalRead(heatPin3);
		boolArray[4] = paused;
		realArray[0] = V1;
		realArray[1] = V2;
		realArray[2] = V3;
		realArray[3] = V4;
		realArray[4] = V5;
		realArray[5] = V6;
		realArray[6] = Temp1;
		realArray[7] = Temp2;
		realArray[8] = Temp3;
		realArray[10] = tempSP;
		realArray[11] = SetCurrent;

		for (int c = 0; c <= 5; c++) {
			if ((realArray[c] >= VoltageLimit) && !limitArray[c]) {
				limitArray[c] = true;
				Serial.println("[T]{V" + String(c + 1) + " is open circuit.  Check Log}");
			}
		}
	}
}

void setHeaterDuty() {
	if (firstRun) {
		lastErr1 = err1; err1 = tempSP - Temp1;
		lastErr2 = err2; err2 = tempSP - Temp2;
		lastErr3 = err3; err3 = tempSP - Temp3;
		firstRun = false;
	}
	if (millis() >= (lastDutyTime + dutyTime)) {
		lastDutyTime = lastDutyTime + dutyTime;
		lastErr1 = err1; err1 = tempSP - Temp1;
		lastErr2 = err2; err2 = tempSP - Temp2;
		lastErr3 = err3; err3 = tempSP - Temp3;
		if ((abs(err1) * 100 / tempSetpoint < tempDeviation) && (abs(err2) * 100 / tempSetpoint < tempDeviation) && (abs(err3) * 100 / tempSetpoint < tempDeviation)) {
			tempsOK = true;
		} else {
			tempsOK = false;
		}

		iTerm1 = iTerm1 + err1 * Ki; if (iTerm1 > 100.0) {
			iTerm1 = 100.0;
		} else if (iTerm1 < 0) {
			iTerm1 = 0;
		}
		iTerm2 = iTerm2 + err2 * Ki; if (iTerm2 > 100.0) {
			iTerm2 = 100.0;
		} else if (iTerm2 < 0) {
			iTerm2 = 0;
		}
		iTerm3 = iTerm3 + err3 * Ki; if (iTerm3 > 100.0) {
			iTerm3 = 100.0;
		} else if (iTerm3 < 0) {
			iTerm3 = 0;
		}
		pTerm1 = Kp * err1; pTerm2 = Kp * err2; pTerm3 = Kp * err3;
		dTerm1 = dTerm1 + ((Kd * (err1 - lastErr1)) - dTerm1) * 0.05;
		dTerm2 = dTerm2 + ((Kd * (err2 - lastErr2)) - dTerm2) * 0.05;
		dTerm3 = dTerm3 + ((Kd * (err3 - lastErr3)) - dTerm3) * 0.05;
		Duty1 = pTerm1 + iTerm1 + dTerm1; if (Duty1 > 100.0) {
			Duty1 = 100.0;
		} else if (Duty1 < 0.0) {
			Duty1 = 0.0;
		}
		Duty2 = pTerm2 + iTerm2 + dTerm2; if (Duty2 > 100.0) {
			Duty2 = 100.0;
		} else if (Duty2 < 0.0) {
			Duty2 = 0.0;
		}
		Duty3 = pTerm3 + iTerm3 + dTerm3; if (Duty3 > 100.0) {
			Duty3 = 100.0;
		} else if (Duty3 < 0.0) {
			Duty3 = 0.0;
		}

		if (Temp1 > tempHLimit) {
			heaterDuty1 = 0;
		} else if (Temp1 < tempLLimit) {
			heaterDuty1 = maxDuty;
		} else if (Temp1 < tempHLimit) {
			heaterDuty1 = (int(Duty1) * maxDuty) / 100;
			if ((heaterDuty1 > hiDuty) || (heaterDuty1 > maxDuty)) {
				heaterDuty1 = maxDuty;
			} else if (heaterDuty1 < lowDuty) {
				heaterDuty1 = 0;
			}
		}
		if (Temp2 > tempHLimit) {
			heaterDuty2 = 0;
		} else if (Temp2 < tempLLimit) {
			heaterDuty2 = maxDuty;
		} else if (Temp2 < tempHLimit) {
			heaterDuty2 = (int(Duty2) * maxDuty) / 100;
			if ((heaterDuty1 > hiDuty) || (heaterDuty2 > maxDuty)) {
				heaterDuty2 = maxDuty;
			} else if (heaterDuty2 < lowDuty) {
				heaterDuty2 = 0;
			}
		}
		if (Temp3 > tempHLimit) {
			heaterDuty3 = 0;
		} else if (Temp3 < tempLLimit) {
			heaterDuty3 = maxDuty;
		} else if (Temp3 < tempHLimit) {
			heaterDuty3 = (int(Duty3) * maxDuty) / 100;
			if ((heaterDuty3 > hiDuty) || (heaterDuty3 > maxDuty)) {
				heaterDuty3 = maxDuty;
			} else if (heaterDuty3 < lowDuty) {
				heaterDuty3 = 0;
			}
		}
	}
}

void setOutputs() {
	if (millis() >= (lastRunTime + runTime)) {
		lastRunTime = lastRunTime + runTime;
		if (is150on) {
			digitalWrite(led150Pin, HIGH);
		} else {
			digitalWrite(led150Pin, LOW);
		}
		if (millis() >= (lastOutputTime + outputPeriod)) {
			lastOutputTime = lastOutputTime + outputPeriod;
			if (heaterDuty1 > 0) {
				digitalWrite(heatPin1, HIGH);
			} else {
				digitalWrite(heatPin1, LOW);
			}
			if (heaterDuty2 > 0) {
				digitalWrite(heatPin2, HIGH);
			} else {
				digitalWrite(heatPin2, LOW);
			}
			if (heaterDuty3 > 0) {
				digitalWrite(heatPin3, HIGH);
			} else {
				digitalWrite(heatPin3, LOW);
			}
		}
		if (millis() >= (lastOutputTime + (outputPeriod / 100) * heaterDuty1)) {
			digitalWrite(heatPin1, LOW);
		}
		if (millis() >= (lastOutputTime + (outputPeriod / 100) * heaterDuty2)) {
			digitalWrite(heatPin2, LOW);
		}
		if (millis() >= (lastOutputTime + (outputPeriod / 100) * heaterDuty3)) {
			digitalWrite(heatPin3, LOW);
		}
	}
}

void sendComs() {
	if (millis() >= (lastComTime + comTime)) {
		lastComTime = lastComTime + comTime;
		String buff = "";
		buff = "[R" + (String)i + "]{" + (String)realArray[i] + "}";
		Serial.println(buff);
		if (boolArray[i]) {
			buff = "[B" + (String)i + "]{1}";
		} else {
			buff = "[B" + (String)i + "]{0}";
		}
		Serial.println(buff);
		i++;
		if (i >= 12) {
			i = 0;
		}
	}
}

void sendComs_v2() {
	String buffer = "";
	for (int x = 0; x <= 12; x++) {
		buffer += "[R" + (String)x + "]{" + (String)realArray[x] + "}";
	}
	for (int x = 0; x <= 4; x++) {
		buffer += "[B" + (String)x + "]{" + (String)boolArray[x] + "}";
	}
	Serial.println(buffer);
	if (millis() >= (lastEEpromTime + eepromDelay)) {
		lastEEpromTime = millis();
		runningTest.elapsed = burnInTimeMillis;
		runningTest.is150 = is150on;
		runningTest.running = boolArray[0];
		EEPROM_write(0, runningTest);
	}
}

void serialEvent() {
	byte inByte1 = 0;
	word buff = 0;
	while (Serial.available()) {
		// get the new byte:
		inByte1 = (byte)Serial.read();
		if (((char)inByte1 == 'S') && (!digitalRead(ledPin))) {
			for (int c = 0; c <= 5; c++) {
				limitArray[c] = false;
			}
			if (tempsOK) {
				digitalWrite(ledPin, HIGH);
				burnInStartTime = millis();
				runningTest.elapsed = 0;
				runningTest.running = true;
				runningTest.is150 = is150on;
				runningTest.paused = false;
				EEPROM_write(0, runningTest);
				if (is150on) {
					burnInTimeLength = BurnTime150;
					SetCurrent = Current1;
				} else {
					burnInTimeLength = BurnTime120;
					SetCurrent = Current2;
				}
				unsigned long timeLeft = burnInTimeLength/1000;
				unsigned long hrs = timeLeft / 3600;
				unsigned long mins = (timeLeft / 60) % 60;
				unsigned long seconds = (timeLeft % 60);
				String time = String(hrs) + ':' + String(mins) + ':' + String(seconds);
				Serial.println("[T]{Starting " + String(SetCurrent) + "mA Test Now:Runtime= " + time + "}");
			} else {
				Serial.println("[T]{Temperatures are not in range}");
			}
		} else if ((char)inByte1 == 'R') {
			Serial.println("[T]{Resetting Device}");
			runningTest.elapsed = 0;
			runningTest.running = false;
			runningTest.is150 = is150on;
			runningTest.paused = false;
			EEPROM_write(0, runningTest);
			delay(1000);
			resetFunc();
		} else if (((char)inByte1 == 'T') && (!digitalRead(ledPin))) {
			Serial.println("[T]{Testing Probe Contact - Probe on for 1000msec}");
			digitalWrite(ledPin, HIGH);
			delay(1000);
			digitalWrite(ledPin, LOW);
			Serial.println("[T]{Testing Complete}");
		} else if (((char)inByte1 == 'H') && (!digitalRead(ledPin))) {
			if (tempSP != tempSetpoint) {
				Serial.println("[T]{Setting Temperature to " + String(tempSetpoint) + "}");
				tempSP = tempSetpoint;
			} else {
				Serial.println("[T]{Setting Temperature to 0}");
				tempSP = 0;
			}
		} else if ((char)inByte1 == 'P') {
			if (!paused) {
				paused = true;
				burnInPauseTime = millis();
				runningTest.paused = true;
				EEPROM_write(0, runningTest);
				Serial.println("[T]{Device Paused}");
				digitalWrite(ledPin, LOW);
			} else if (paused) {
				paused = false;
				burnInStartTime = burnInStartTime + (millis() - burnInPauseTime);
				runningTest.elapsed = burnInTimeMillis;
				runningTest.paused = false;
				EEPROM_write(0, runningTest);
				Serial.println("[T]{Device Resumed}");
				digitalWrite(ledPin, HIGH);
				tempSP = tempSetpoint;
				for (int c = 0; c <= 5; c++) {
					limitArray[c] = false;
				}
			}
		} else if (((char)inByte1 == 'C') && (!digitalRead(ledPin))) {
			if (!is150on) {
				Serial.println("[T]{Setting Current to " + String(Current1) + "mA}");
				is150on = true;
				SetCurrent = Current1;
				burnInTimeLength = BurnTime150;
				runningTest.is150 = true;
				EEPROM_write(0, runningTest);
			} else {
				if (SwitchingEnabled) {
					Serial.println("[T]{Setting Current to " + String(Current2) + "mA}");
					is150on = false;
					burnInTimeLength = BurnTime120;
					SetCurrent = Current2;
					runningTest.is150 = false;
					EEPROM_write(0, runningTest);
				} else {
					Serial.println("[T]{Switching Not Installed On This Station}");
				}
			}
		} else if (((char)inByte1 == 'U') && (!digitalRead(ledPin))) {
			int isEnabled = ((char)Serial.read()) - '0';  //0
			int aversion = ((char)Serial.read()) - '0';   //1
			int vversion = ((char)Serial.read()) - '0';   //2
			int ch = ((char)Serial.read()) - '0';         //3
			int ct = ((char)Serial.read()) - '0';		  //4
			int co = ((char)Serial.read()) - '0';		  //5
			int tt=((char)Serial.read()) - '0';			  //6
			int to = ((char)Serial.read()) - '0';         //7
			int current = (ch * 100) + (ct * 10) + co;
			int temp = (tt * 10) + (to);
			systemSettings.switchingEnabled = (bool)isEnabled;
			systemSettings.current2 = current;
			systemSettings.temperature = temp;
			systemSettings.analogVersion = aversion;
			systemSettings.voltageVersion = vversion;
			if (checkMemoryData(false)) {
				EEPROM_write(settingsAddr, systemSettings);
				Serial.println("[T]{System settings received: Current: " + String(current) + " Temp: "+ String(temp) + " Voltage: " + String(vversion) + "}");
			} else {
				Serial.println("[T]{Error receiving one or more settings}");
			}
		}
	}
}

void checkStart() {
	if (!firstTimeCheck) {
		firstTimeCheck = true;
		if (runningTest.running || runningTest.paused) {
			for (int c = 0; c <= 5; c++) {
				limitArray[c] = false;
			}
			runningTest.running = true;
			runningTest.paused = false;
			tempSP = tempSetpoint;
			if (is150on) {
				digitalWrite(led150Pin, HIGH);
				burnInTimeLength = BurnTime150;
				SetCurrent = Current1;
			} else {
				digitalWrite(led150Pin, LOW);
				burnInTimeLength = BurnTime120;
				SetCurrent = Current2;
			}
			burnInStartTime = millis() - runningTest.elapsed;
			digitalWrite(ledPin, HIGH);
			Serial.println("[T]{Setting Temperature to " + String(tempSetpoint) + "}");
			unsigned long timeLeft = (burnInTimeLength - runningTest.elapsed)/1000;
			unsigned long hrs = timeLeft / 3600;
			unsigned long mins = (timeLeft / 60) % 60;
			unsigned long seconds = (timeLeft % 60);
			String time = String(hrs) +':'+ String(mins) + ':'+ String(seconds);
			Serial.println("[T]{Continuing " + String(SetCurrent) + "mA Test Now:Runtime= " + time + "}");
		}
	}
}

void loadFromMemory(bool read) {
	settingsAddr = EEPROM_read(0, runningTest);
	EEPROM_read(settingsAddr, systemSettings);
	checkMemoryData(true);
}

bool checkMemoryData(bool checkRunning) {
	bool success = true;
	if (systemSettings.switchingEnabled == 0 || systemSettings.switchingEnabled == 1) {
		SwitchingEnabled = systemSettings.switchingEnabled;
	} else {
		success = false;
	}

	if (systemSettings.temperature > 0) {
		tempSetpoint = systemSettings.temperature;
	} else {
		success = false;
	}

	if (systemSettings.current2 == 60 || systemSettings.current2 == 120) {
		Current2 = systemSettings.current2;
	} else {
		success = false;
	}

	if (systemSettings.analogVersion == 2) {
		AnalogVersion = 2;
		analogPins.a0 = A9;
		analogPins.a1 = A10;
		analogPins.a2 = A11;
		analogPins.a3 = A12;
		analogPins.a4 = A13;
		analogPins.a5 = A14;
	} else if(systemSettings.analogVersion == 1) {
		AnalogVersion = 1;
		analogPins.a0 = A0;
		analogPins.a1 = A1;
		analogPins.a2 = A2;
		analogPins.a3 = A3;
		analogPins.a4 = A4;
		analogPins.a5 = A5;
	} else {
		AnalogVersion = 1;
		analogPins.a0 = A0;
		analogPins.a1 = A1;
		analogPins.a2 = A2;
		analogPins.a3 = A3;
		analogPins.a4 = A4;
		analogPins.a5 = A5;
		success = false;
	}

	if (systemSettings.voltageVersion==1) {
		systemVoltage = 105.00;
	} else if(systemSettings.voltageVersion==2) {
		systemVoltage = 72.00;
	} else {
		systemVoltage = 105.00;
	}

	if ((runningTest.running || runningTest.paused) && checkRunning) {
		is150on = runningTest.is150;
	}

	return success;
}
