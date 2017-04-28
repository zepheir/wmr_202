// 

#ifndef __WMR_H__
#define __WMR_H__

// includes
#include <Arduino.h>
#include <EEPROM.h>			// 
#include <avr/pgmspace.h>



// Defines 
#define FALSE LOW
#define TRUE HIGH

#define Nop() asm("NOP")


// Define pins of input channels
#define PIN_CH0 4
#define PIN_CH1 5
#define PIN_CH2 6
#define PIN_CH3 7

// Define eeprom address of wmr datas
#define DATAADDR0 0x10
#define DATAADDR1 0x18
#define DATAADDR2 0x20
#define DATAADDR3 0x28

// Define data length of wmr datas
#define DATALENGTH 4


// Define wmr sample filter time
// real time = 50*2 = 100ms
#define FILTERTIME 10


typedef struct WMR_DATA
{
	unsigned long ch0=0;
	unsigned long ch1=0;
	unsigned long ch2=0;
	unsigned long ch3=0;
};

typedef struct ISACTIVE
{
	bool ch0;
	bool ch1;
	bool ch2;
	bool ch3;
};

typedef struct FILTER
{
	unsigned long ch0;
	unsigned long ch1;
	unsigned long ch2;
	unsigned long ch3;
};

class Wmr
{
// private:
	// void wmr_int();

public:
	char PROGMEM *moduleType= "WMR";

	WMR_DATA data;
	ISACTIVE isActive;
	FILTER filter;


	Wmr();
	// ~Wmr();

	void setPinMode(void);

	void begin(void);

	void clearAll(void);

	void releaseAllLock(void);

	
	// read/write/update data from eeprom
	unsigned long readRom(unsigned int addr);
	void writeRom(unsigned int addr, unsigned long data);
	void updateRom(unsigned int addr, unsigned long data);

	
};



#endif
