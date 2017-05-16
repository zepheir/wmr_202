#include <Arduino.h>
#include "Sim808.h"
#include "ArduinoJson.h"

Sim808::Sim808(){
	state=0;

	ok_reply = F("OK");
}

// Sim808::~Sim808(){
// 	;
// }


void Sim808::intialBaudrate(void){
	Serial.begin(9600);
}


void Sim808::setBaudrate(uint16_t baudrate){
	sendCheckReply(F("AT+IPR=0"),F("OK"),500);
	// client808.print(F("AT+IPR="));
	// client808.println(baudrate);
	delay(200);
	Serial.begin(baudrate);		
}


bool Sim808::busy(void){
	return _busy;
}


uint8_t Sim808::getStatus(void){
	return _status;
}


char* Sim808::getIMEI(void){
	return _imei;
}

/** 
 * This function is to read the data from GPRS pins. This function is from Adafruit_FONA library
 * @arg timeout, time to delay until the data is transmited
 * @return replybuffer the data of the GPRS
 */
uint8_t Sim808::readline(uint16_t timeout){

	uint8_t idx = 0;
	// bool readed=false;


	while(timeout--){
		if(idx>=254) break;


		while(client808.available()){
			char c = client808.read();

			if (c=='\x0D') continue;

			if (c=='\x0A'){
				if (idx==0) continue; // the first 'nl' is ignored

				timeout = 0;
				break;
			}

			replybuffer[idx]=c;
			idx++;
		}

		if(timeout == 0) break;
		delay(1);
	}
	
	replybuffer[idx]='\0';
	// if(!_call_ready){
	// 	if(prog_char_strcmp(replybuffer, (prog_char*)F("CALL READY")) == 0){
	// 		_call_ready = true;
	// 	}
	// }
	// if(!_sms_ready){
	// 	if(prog_char_strcmp(replybuffer, (prog_char*)F("SMS READY")) == 0){
	// 		_sms_ready = true;
	// 	}
	// }

	return idx;
}

// 读写一组数据, 不是字符串, 不能用0d0a来判断
uint8_t Sim808::readBuff(uint16_t timeout){
    
    uint8_t idx = 0;
    // bool readed=false;
    
    
    while(timeout--){
        if(idx>=255) break;
        
        while(client808.available()){
            char c = client808.read();
            
//            if (c=='\x0A'){
//                if (idx==0) continue; // the first 'nl' is ignored
//            }
            
            replybuffer[idx]=c;
            idx++;
        }
        
        if(timeout == 0) break;
        delay(1);
    }
    
    
    return idx;
}


// from adafruit_FONA
void Sim808::flushInput(){
	int timeout = 0;
	while(timeout++ < 40){
		while(client808.available()){
			client808.read();
			timeout = 0;	// If char was received, reset the timeout
		}
		delay(1);
	}
}


uint8_t Sim808::getReply(char* send, uint16_t timeout){
	flushInput();

	client808.println(send);

	return readline(timeout);

}

uint8_t Sim808::getReply(FlashStringPtr send, uint16_t timeout){
	flushInput();

	client808.println(send);

	return readline(timeout);

}

uint8_t Sim808::getReply(FlashStringPtr send){
	flushInput();

	client808.println(send);

	return readline(500);

}


bool Sim808::sendCheckReply(char* send, char* reply, uint16_t timeout){
	if(!getReply(send,timeout))
		return false;

	return (strcmp(replybuffer, reply)==0);
}

bool Sim808::sendCheckReply(char* send, FlashStringPtr reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
	  return false;
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}

bool Sim808::sendCheckReply(FlashStringPtr send, FlashStringPtr reply, uint16_t timeout) {
  if (! getReply(send, timeout) )
	  return false;
  return (prog_char_strcmp(replybuffer, (prog_char*)reply) == 0);
}


bool Sim808::checkReply(char* reply, uint16_t timeout){

	readline(timeout);

	return (strcmp(replybuffer, reply)==0);
}

bool Sim808::checkReply(FlashStringPtr reply, uint16_t timeout){

	readline(timeout);

	return (prog_char_strcmp(replybuffer, (prog_char*)reply)==0);
}

bool Sim808::checkReply(FlashStringPtr reply){

	readline(500);

	return (prog_char_strcmp(replybuffer, (prog_char*)reply)==0);
}



//----------------
// TCP Functions
//----------------
bool Sim808::TCPClose(void){
	return sendCheckReply(F("AT+CIPCLOSE"),F("CLOSE OK"),1000);
}

bool Sim808::TCPShut(void){
	return sendCheckReply(F("AT+CIPSHUT"),F("SHUT OK"),10000);
}

bool Sim808::checkTCPStatus(){
	
	flushInput();

	if(!sendCheckReply(F("AT+CIPSTATUS"), F("OK"), 500) ){
		prog_char_strcat(replybuffer, (prog_char*)F("-Check Status Fail"));
		_status = 0xff;
		return false;
	}

	readline(500);

	if (prog_char_strcmp(replybuffer, (prog_char*)F("STATE: IP INITIAL"))==0){
		_status = 0x00;
		return true;
	}

	if (prog_char_strcmp(replybuffer, (prog_char*)F("STATE: CONNECT OK"))==0){
		_status = 0x06;
		return true;
	}
	
	if (prog_char_strcmp(replybuffer, (prog_char*)F("STATE: TCP CLOSED"))==0){
		_status = 0x08;
		return true;
	}

	if (prog_char_strcmp(replybuffer, (prog_char*)F("STATE: PDP DEACT"))==0){
		_status = 0x09;
		return true;
	}

	_status=0x0F; // Don't know the status
	return true;

}

bool Sim808::TCPConnect(char* server, char* port){
	// uint8_t __status;

	//
	flushInput();

	TCPShut();
	flushInput();

	// // Single connection at a time
	// if(! sendCheckReply(F("AT+CIPMUX=0"), "OK")) return false;

	//
	client808.print(F("AT+CIPSTART=\"TCP\",\""));
	client808.print(server);
	client808.print(F("\",\""));
	client808.print(port);
	client808.println(F("\""));


	if(! checkReply(F("OK"), 2000)){
		prog_char_strcat(replybuffer, (prog_char*)F("-CHECK OK ERROR!"));
		return false;
	} 
	if(! checkReply(F("CONNECT OK"), 2000)){
		prog_char_strcat(replybuffer, (prog_char*)F("-CHECK CONNECT OK ERROR!"));
		return false;
	}


	return true;

}

// bool Sim808::TCPConnect(char* server, char* port){
// 	// uint8_t __status;

// 	//
// 	flushInput();
// 	TCPShut();
// 	flushInput();

// 	// // Single connection at a time
// 	// if(! sendCheckReply(F("AT+CIPMUX=0"), "OK")) return false;

// 	//
// 	client808.print("AT+CIPSTART=\"TCP\",\"");
// 	client808.print(server);
// 	client808.print("\",\"");
// 	client808.print(port);
// 	client808.println("\"");

// 	if(! checkReply("OK", 2000)){
// 		strcat(replybuffer, "-CHECK OK ERROR!");
// 		return false;
// 	} 
// 	if(! checkReply("CONNECT OK", 2000)){
// 		strcat(replybuffer, "-CHECK CONNECT OK ERROR!");
// 		return false;
// 	}


// 	return true;

// }


bool Sim808::TCPSend(char* data){

	client808.println(F("AT+CIPSEND"));
	readline(500);

	if(replybuffer[0] != '>') return false;

	client808.print(data);
	client808.println(F("\x1A"));
	readline(3000);


	return (prog_char_strcmp(replybuffer, (prog_char*)F("SEND OK"))==0);
}


bool Sim808::TCPSendDataPack(	char* module,
								unsigned long data0, 
								unsigned long data1,
								unsigned long data2,
								unsigned long data3 ){

	//
	client808.println(F("AT+CIPSEND"));
	readline(500);

	if(replybuffer[0] != '>') return false;

	// client808.print(F("{\"TYPE\":\"SIM808\",\"IMEI\":\""));
	client808.print(F("{\"TYPE\":\"DATA\",\"MODU\":\""));
	client808.print(module);
	client808.print(F("\",\"IMEI\":\""));
	client808.print(_imei);
	// client808.print("865067020893791");
	client808.print(F("\",\"D0\":\""));
	client808.print(data0);
	client808.print(F("\",\"D1\":\""));
	client808.print(data1);
	client808.print(F("\",\"D2\":\""));
	client808.print(data2);
	client808.print(F("\",\"D3\":\""));
	client808.print(data3);
	client808.println(F("\"}\x1A"));
	readline(3000);



	if(!prog_char_strcmp(replybuffer, (prog_char*)F("SEND OK"))==0){
		prog_char_strcat(replybuffer, (prog_char*)F("SEND DATA ERROR!"));
		return false;
	}


	// strcpy(replybuffer, "SEND OK!");
	sendCount++; //Count send numbers
	return true;
}


bool Sim808::TCPGetDataPack(char* module, uint32_t *data0, uint32_t *data1, uint32_t *data2, uint32_t *data3){
	StaticJsonBuffer<255> jsonBuffer;
	

	//sim connect
	if(!TCPConnect(TCP_SERVER,TCP_PORT)){
		prog_char_strcat(replybuffer, (prog_char*)F("CONNECT ERROR!"));
		return false;
	}

	// SIM send
	client808.println(F("AT+CIPSEND"));
	readline(500);

	if(replybuffer[0] != '>') return false;

	// client808.print(F("{\"TYPE\":\"SIM808\",\"IMEI\":\""));
	client808.print(F("{\"TYPE\":\"CMD\",\"CMD\":\"RDLAST\",\"MODU\":\""));
	client808.print(module);
	client808.print(F("\",\"IMEI\":\""));
	client808.print(_imei);
	client808.println(F("\"}\x1A"));
	readline(3000);

	if(!prog_char_strcmp(replybuffer, (prog_char*)F("SEND OK"))==0){
		prog_char_strcat(replybuffer, (prog_char*)F("SEND DATA ERROR!"));
		return false;
	}


	// {"TYPE":"CMD","CMD":"UPLAST","IMEI":"865067022375680","D0":"2","D1":"18","D2":"0","D3":"1"}
	readline(3000);


	JsonObject& root = jsonBuffer.parseObject(replybuffer);

	// sim808 Close
	TCPClose();
	delay(100);
	TCPShut();
	delay(100);

	// Test if parsing succeeds.
	if (!root.success()) {
		strcpy_P(replybuffer, (prog_char*)F("parseObject FAIL!"));
		return false;
	}

	if (!strcmp_P(root["CMD"], (prog_char*)F("UPLAST"))==0)
	{
		strcpy_P(replybuffer, (prog_char*)F("CMD NOT match!"));
		return false;
	}

	*data0 = atol(root["D0"]);
	*data1 = atol(root["D1"]);
	*data2 = atol(root["D2"]);
	*data3 = atol(root["D3"]);
	
	return true;
}


// uint8_t Sim808::readIMEI(char *_imei){
uint8_t Sim808::readIMEI(void){

	getReply(F("AT+GSN"),500);

	strncpy(_imei, replybuffer, 15);
	
	readline(500);

	// strcpy(_imei, "123456789012345");
	_imei[15]='\0';

	// flushInput();

	return strlen(_imei);

}


uint8_t Sim808::getSignalLevel(void){

	// check SIM808 signal level
	getReply(F("AT+CSQ"), 500);
	strtok_P(replybuffer, (prog_char*)F(" ,"));
	signalLevel = atoi(strtok_P(NULL, (prog_char*)F(" ,")));

	signalLevelBar[0]=(signalLevel>= 5)?'*':'_';
	signalLevelBar[1]=(signalLevel>=10)?'*':'_';
	signalLevelBar[2]=(signalLevel>=15)?'*':'_';
	signalLevelBar[3]=(signalLevel>=20)?'*':'_';
	signalLevelBar[4]=(signalLevel>=25)?'*':'_';
	signalLevelBar[5]=(signalLevel>=30)?'*':'_';

	return signalLevel;

}


bool Sim808::begin(void){
	//give 14 seconds to reboot
	int16_t _timeout = 20000;
	// unsigned int _timeout = 10000;

	while(_timeout>0){
		// delay(500);
		readline(500);
		_timeout-=500;

		// if(strcmp(replybuffer, "SMS READY")==0){
		// 	break;
		// }
		if ( prog_char_strcmp(replybuffer, (prog_char*)F("SMS READY"))==0){
			break;
		}
		
	}

	// Set SIM808 Baudrate
	// Set SIM808 ipx=0
	setBaudrate(BAUDRATE);

	if(_timeout<=0){
		sendCheckReply(F("AT"),ok_reply, 500);
		delay(100);
		sendCheckReply(F("AT"),ok_reply, 500);
		delay(100);
		sendCheckReply(F("AT"),ok_reply, 500);
		delay(100);
	}

	flushInput();

	delay(100);

	TCPShut();

	delay(2000);

	flushInput();

	// turn off echo
	sendCheckReply(F("ATE0"), ok_reply, 500);
	delay(100);

	if (!sendCheckReply(F("ATE0"), ok_reply, 500)){
		prog_char_strcpy(replybuffer, (prog_char*)F("Start up FAIL!"));
		TCPClose();
		delay(1000);
		TCPShut();
		delay(1000);
	}

	delay(100);
	flushInput();

	// read IMEI
	if(readIMEI()==0){
		prog_char_strcpy(replybuffer, (prog_char*)F("IMEI READ FAIL!"));
		return false;
	}

	// if(!sendCheckReply(F("AT+CGATT?"), F("+CGATT: 1")), 500)){
	// 	prog_char_strcpy(replybuffer, (prog_char*)F("CGATT ERR"));
	// 	return false;
	// }


	checkTCPStatus();
	if(_status!=0x0){
		TCPShut();
		delay(3000);
	}

	do
	{
		/* code */
		checkTCPStatus();
		delay(500);
	} while (_status!=0);

	// // return replybuffer = TCP STATUS
	// // strcpy(replybuffer, "StartUp OK!");

	return true;
}	


//
void Sim808::simPwrUpDown(uint8_t pwrPin){
    digitalWrite(pwrPin, LOW);
    delay(1500);
    digitalWrite(pwrPin, HIGH);
}


















