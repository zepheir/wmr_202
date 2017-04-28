#ifndef __SIM808_H__
#define __SIM808_H__

#define _DEBUG

#include <Arduino.h>
#include <avr/pgmspace.h>

// Macros of using Program Flash space
typedef const __FlashStringHelper *	FlashStringPtr;

#define prog_char  					char PROGMEM

#define prog_char_strcmp(a, b)					strcmp_P((a), (b))
// define prog_char_strncmp(a, b, c)				strncmp_P((a), (b), (c))
#define prog_char_strstr(a, b)					strstr_P((a), (b))
#define prog_char_strlen(a)						strlen_P((a))
#define prog_char_strcpy(to, fromprogmem)		strcpy_P((to), (fromprogmem))
//define prog_char_strncpy(to, from, len)		strncpy_P((to), (fromprogmem), (len))
#define prog_char_strcat(a, b)					strcat_P((a), (b))


//
// #define BAUDRATE 19200
// #define BAUDRATE 38400
#define BAUDRATE 57600

#define SIM808
#define SIM_TYPE	"SIM808"
// #define MODU 		"WMR"

#define SIM808_DEFALUT_TIMEOUT 500


#define client808 Serial
#define SIM_PWR_PIN


// AT Commands
#define CMD_TEST		"AT"
#define RE_TEST			"OK"
#define ERR_TEST		"ERR:TEST Fail"

#define CMD_ECHO_ON 	"ATE1"
#define CMD_ECHO_OFF 	"ATE0"
#define RE_ECHO			"OK"
#define ERR_ECHO_ON		"ERR:Set ECHO ON Fail"
#define ERR_ECHO_OFF 	"ERR:Set ECHO OFF Fail"

#define CMD_SINGAL_LV 	"AT+CSQ"
#define RE_SIGNAL_LV	"OK"
#define ERR_SIGNAL_LV	"ERR:RD Signal LV Fail"

#define CMD_TCP_CONNECT	"AT+CIPSTART=\"TCP\",\"121.199.16.44\",\"8889\""
#define RE_TCP_CONNECT	"CONNECT OK"
#define ERR_TCP_CONNECT	"ERR:Connect SVR Fail"
#define TCP_SERVER		"121.199.16.44"
#define TCP_PORT		"8889"

#define CMD_SHUT_DOWN	"AT+CIPSHUT"
#define RE_SHUT_DOWN	"SHUT OK"
#define ERR_SHUT_DOWN	"ERR:SHUT DOWN Fail"





class Sim808
{
private:
	// Serial _client;
	// char* readData(uint16_t timeout);
	uint8_t _status=0x0F;
	char _imei[15]={0};

	FlashStringPtr ok_reply;
	FlashStringPtr _ip;

	bool _busy = false;
	// bool _sms_ready = false;
	// bool _call_ready = false;

public:
	Sim808();
	// ~Sim808();

	unsigned char state=0;
	// uint8_t status=0x0F;
	uint8_t sendCount=0;
	char replybuffer[255];


	char* signalLevelBar="______";
	// unsigned int signalLevel = 0;
	uint8_t signalLevel = 0;

	// char *imei = "865067020893791";
	// char imei[15]={0};
	// char temp = 0;
	char *getIMEI(void);

	bool busy(void);

	void intialBaudrate(void);
	void setBaudrate(uint16_t baudrate);
	// char* readData(char* replybuffer, uint16_t timeout);
	char* readData(char* buff, uint16_t timeout);
	char* readData(char* buff);
	uint8_t readline(uint16_t timeout);

	void sendCmd(char* cmd);

	void flushInput(void);
	uint8_t getReply(char *send, uint16_t timeout);
	uint8_t getReply(FlashStringPtr send, uint16_t timeout);
	uint8_t getReply(FlashStringPtr send);
	bool sendCheckReply(char* send, char* reply, uint16_t timeout);
	bool sendCheckReply(char* send, FlashStringPtr reply, uint16_t timeout);
	bool sendCheckReply(FlashStringPtr send, FlashStringPtr reply, uint16_t timeout);
	bool checkReply(char* reply, uint16_t timeout);
	bool checkReply(FlashStringPtr reply, uint16_t timeout);
	bool checkReply(FlashStringPtr reply);


	// TCP Funcitons
	bool TCPConnect(char* server, char* port);
	bool TCPSend(char* data);
	bool TCPClose(void);
	bool TCPShut(void);

	// IMEI
	// uint8_t readIMEI(char *imei);
	uint8_t readIMEI(void);

	// Status
	bool checkTCPStatus(void);
	uint8_t getStatus(void);

	uint8_t getSignalLevel(void);

	//
	bool begin(void);

	//
	bool TCPSendDataPack(char* module, uint32_t data0, uint32_t data1, uint32_t data2, uint32_t data3);
	bool TCPGetDataPack(char* module, uint32_t *data0, uint32_t *data1, uint32_t *data2, uint32_t *data3);
	
    
    //
    void simPwrUpDown(uint8_t pwrPin);


};

#endif
