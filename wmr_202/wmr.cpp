#include "wmr.h"
#include <EEPROM.h>			// 
#include "PinChangeInterrupt.h"		// Pin Change Interrupt library

Wmr::Wmr(){;}

// Wmr::~Wmr(){;}


void Wmr::setPinMode(){
	pinMode(PIN_CH0, INPUT_PULLUP);
	pinMode(PIN_CH1, INPUT_PULLUP);
	pinMode(PIN_CH2, INPUT_PULLUP);
	pinMode(PIN_CH3, INPUT_PULLUP);

	// I think we don't need these...
	// digitalWrite(PIN_CH0, HIGH);
	// digitalWrite(PIN_CH1, HIGH);
	// digitalWrite(PIN_CH2, HIGH);
	// digitalWrite(PIN_CH3, HIGH);
}


void Wmr::begin(void){

	disablePCINT(digitalPinToPCINT(PIN_CH0));
	disablePCINT(digitalPinToPCINT(PIN_CH1));
	disablePCINT(digitalPinToPCINT(PIN_CH2));
	disablePCINT(digitalPinToPCINT(PIN_CH3));

	isActive.ch0 = TRUE;
	isActive.ch1 = TRUE;
	isActive.ch2 = TRUE;
	isActive.ch3 = TRUE;


	// writeRom(DATAADDR3,0);
	// writeRom(DATAADDR2,0);
	// writeRom(DATAADDR1,0);
	// writeRom(DATAADDR0,0);


	// data.ch0=readRom(DATAADDR0);
	// data.ch1=readRom(DATAADDR1);
	// data.ch2=readRom(DATAADDR2);
	// data.ch3=readRom(DATAADDR3);


	filter.ch0 = millis();
	filter.ch1 = millis();
	filter.ch2 = millis();
	filter.ch3 = millis();

	enablePCINT(digitalPinToPCINT(PIN_CH0));
	enablePCINT(digitalPinToPCINT(PIN_CH1));
	enablePCINT(digitalPinToPCINT(PIN_CH2));
	enablePCINT(digitalPinToPCINT(PIN_CH3));
}

void Wmr::clearAll(void){

	data.ch0=0;
	data.ch1=0;
	data.ch2=0;
	data.ch3=0;

}

void Wmr::releaseAllLock(void){

	isActive.ch0=(millis()-filter.ch0 >= FILTERTIME) && (digitalRead(PIN_CH0) != 0);
	isActive.ch1=(millis()-filter.ch1 >= FILTERTIME) && (digitalRead(PIN_CH1) != 0);
	isActive.ch2=(millis()-filter.ch2 >= FILTERTIME) && (digitalRead(PIN_CH2) != 0);
	isActive.ch3=(millis()-filter.ch3 >= FILTERTIME) && (digitalRead(PIN_CH3) != 0);

}



unsigned long Wmr::readRom(unsigned int addr){
	unsigned long tmp,data;
	byte i;

	i = 0;
	while(i < DATALENGTH){
		tmp = EEPROM.read(addr+i);
		data=data<<8;
		data+=tmp;
		i++;
	}

	return data;
}

void Wmr::writeRom(unsigned int addr, unsigned long data){
	byte i;

	i = DATALENGTH;
	while(i>0){
		i--;
		EEPROM.write(addr+i,byte(data));
		data>>=8;
	}
}

void Wmr::updateRom(unsigned int addr, unsigned long data){

	if(data!=readRom(addr)){
		writeRom(addr,data);
	}
}

