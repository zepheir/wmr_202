///
/// @mainpage	wmr_202
///
/// @details	Description of the project
/// @n
/// @n
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Samuel Gao
/// @author		Samuel Gao
/// @date		2017/4/28 16:45
/// @version	2.0.2
///
/// @copyright	(c) Samuel Gao, 2017年
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
///


///
/// @file		wmr_202.ino
/// @brief		Main sketch
///
/// @details	这个版本是希望可以建立一个与服务器可以查询和通讯的wmr程序版本
///             
/// @n @a		Developed with [embedXcode+](http://embedXcode.weebly.com)
///
/// @author		Samuel Gao
/// @author		Samuel Gao
/// @date		2017/4/28 16:45
/// @version	2.0.2
///
/// @copyright	(c) Samuel Gao, 2017年
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
/// @n
///


//-------------------------------------
// Set parameters
#define RUNLED 13
static unsigned short _runLedState = 0;
uint8_t statement = 0;

// system statements
#define INITIAL_DISPLAY 	0
#define INITIAL_SIM808_1 	1
#define INITIAL_SIM808_2 	2
#define INITIAL_SIM808_3 	3
#define INITIAL_SIM808_4 	4
#define INITIAL_SIM808_5 	5
#define INITIAL_SIM808_6 	6
#define INITIAL_SIM808_7 	7
#define INITIAL_SIM808_8 	8
#define INITIAL_WMR 		9
#define INITIAL_OK			10
#define SYS_RUN             255

// sim808链接 数据发送 关闭状态
#define SIM_TCP_0   0
#define SIM_TCP_1   1
#define SIM_TCP_2   2
#define SIM_TCP_3   3
#define SIM_TCP_4   4
#define SIM_TCP_5   5
#define SIM_TCP_6   6
#define SIM_TCP_7   7
#define SIM_TCP_8   8
#define SIM_TCP_9   9
#define SIM_TCP_10   10
#define SIM_TCP_OFF   40

// Define Keyboard
#define UP 1
#define RIGHT 2
#define PUSH 3
#define DOWN 4
#define LEFT 5
#define MID 0

// Define display state
uint8_t page = 0;

#define PAGE_INITIAL	0
#define PAGE_WMR		1
#define PAGE_GPRS		2
#define PAGE_SETTING	3

//-------------------------------------
// Include application, user and local libraries
#include "Sanji.h"
#include "U8glib.h"
#include "wmr.h"
#include "PinChangeInterrupt.h"
#include "Sim808.h"
//#include "crc16.h"
//#include "word.h"

//-------------------------------------
// Define structures and classes


//-------------------------------------
// Define variables and constants
Sanji myDev;
int joyStick = 0;

// 定义水表
Wmr wmr;

// 定义显示器
// U8GLIB_SSD1306_128X64 myOled(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI
U8GLIB_SSD1306_128X64 myOled(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI
// U8GLIB_SSD1306_128X64 myOled(U8G_I2C_OPT_NO_ACK);	// Display which does not send AC
// U8GLIB_SSD1306_128X64 myOled(U8G_I2C_OPT_NONE);myOled
// String oledDisplay="";      //the display string recieved from the mobile device
String strInitialInfo="";

// 定义通讯模块 sim808
Sim808 sim808;
#define SIM_PWR_PIN 8         // 定义SIM808模块的电源控制引脚
char* strSimSignalLevel="______";
uint8_t simSignalLevel=0;
uint8_t simTemp1=0;
uint8_t simStep=0;
char strSIMInfo[8];
#define TCP_SERVER		"121.199.16.44"
#define TCP_PORT		"6969"
// GPRS data send every 60*5=300000 ms
#define UPLOADTIMER 300000

//-------------------------------------
// Prototypes
#define FALSE LOW
#define TRUE HIGH

#define Nop() asm("NOP")

// Functions
void blinkLED(void);

void draw (void);			// Display main program
void drawInitialPage (void);
//void drawInitialPage2 (void);
void drawWmrPage (void);
void picture_loop(void (*draw_fn)(void));


//------------------------------------------
// Without the time delay filter
void wmr_int(void){
    
#ifdef _RUNLEN
    // digitalWrite(RUNLED, HIGH);
#endif
    
    if(!digitalRead(PIN_CH0)){
        if(wmr.isActive.ch0){
            wmr.data.ch0++;
            wmr.isActive.ch0=false;
        }
    }else{wmr.isActive.ch0=true;}
    
    if(!digitalRead(PIN_CH1)){
        if(wmr.isActive.ch1){
            wmr.data.ch1++;
            wmr.isActive.ch1=false;
        }
    }else{wmr.isActive.ch1=true;}
    
    if(!digitalRead(PIN_CH2)){
        if(wmr.isActive.ch2){
            wmr.data.ch2++;
            wmr.isActive.ch2=false;
        }
    }else{wmr.isActive.ch2=true;}
    
    if(!digitalRead(PIN_CH3)){
        if(wmr.isActive.ch3){
            wmr.data.ch3++;
            wmr.isActive.ch3=false;
        }
    }else{wmr.isActive.ch3=true;}
    
    
#ifdef _RUNLEN
    // digitalWrite(RUNLED, LOW);
#endif
}
//---------------------------------------



//-------------------------------------
// Add setup code
void setup()
{
    myDev.begin();
    pinMode(RUNLED, OUTPUT);
    pinMode(SIM_PWR_PIN, OUTPUT); // sim808 powerkey
    
    // Set the Pins Pullup adn Input
    wmr.setPinMode();
    
    // // attach the new PinChangeInterrupt and enable event function below
    // // Valid interrupt modes are: RISING, FALLING, CHANGE
    attachPCINT(digitalPinToPCINT(PIN_CH0), wmr_int, CHANGE);
    attachPCINT(digitalPinToPCINT(PIN_CH1), wmr_int, CHANGE);
    attachPCINT(digitalPinToPCINT(PIN_CH2), wmr_int, CHANGE);
    attachPCINT(digitalPinToPCINT(PIN_CH3), wmr_int, CHANGE);
    
    // // The another mode of declare:
    // // attachPinChangeInterrupt(PCINT_No, foo, RISING);
    
    
    wmr.begin();
    
    
    // Set the Mode of SIM808
    // Default bandrate is 9600
    sim808.intialBaudrate();
    
}

//-------------------------------------
// Add loop code
void loop()
{
    
    // Initial step
    static unsigned long timerInitial = millis();
    if (millis()- timerInitial >= 2000) {
        timerInitial = millis();
        
        if(statement != SYS_RUN) blinkLED();
        
        switch (statement) {
            case INITIAL_DISPLAY:
                page = PAGE_WMR;
                strInitialInfo="Initial Display: OK!";
                picture_loop(drawInitialPage);
                statement = INITIAL_SIM808_1;
                break;
                
            case INITIAL_SIM808_1:
                // 给sim808模块上点启动
                strInitialInfo="Waiting: ";
                strInitialInfo+=simTemp1;
                picture_loop(drawInitialPage);
                //                // 如果之前模块没有上电，则给上电模块启动
                //                sim808.flushInput();
                //                if (!sim808.sendCheckReply(F("ATE0"), F("OK"), 500)) {
                //                    sim808.simPwrUpDown(SIM_PWR_PIN);
                //                }
                sim808.simPwrUpDown(SIM_PWR_PIN);
                statement = INITIAL_SIM808_2;
                break;
                
            case INITIAL_SIM808_2:
                //等10秒
                if (simTemp1<5) {
                    // 设置sim808回复模式：echo off
                    sim808.sendCheckReply(F("ATE0"), F("OK"), 500);
                    simTemp1++;
                    strInitialInfo="Waiting: ";
                    strInitialInfo+=simTemp1;
                    picture_loop(drawInitialPage);
                }else{
                    statement = INITIAL_SIM808_3;
                    simTemp1=0;
                }
                break;
                
            case INITIAL_SIM808_3:
                // 读取IMEI
                if(sim808.readIMEI()==0){
                    strInitialInfo="IMEI READ FAIL!";
                    picture_loop(drawInitialPage);
                    break;
                }
                strInitialInfo="IMEI:";
                strInitialInfo+=sim808.getIMEI();
                picture_loop(drawInitialPage);
                statement = INITIAL_SIM808_4;
                break;
                
            case INITIAL_SIM808_4:
                // 读取GPRS信号
                sim808.getSignalLevel();
                strInitialInfo="Get Signal Level: ";
                strInitialInfo+=sim808.signalLevel;
                picture_loop(drawInitialPage);
                statement = INITIAL_SIM808_5;
                break;
                
            case INITIAL_SIM808_5:
                // 通讯模块初始化完毕
                strInitialInfo="SIM Module OK!";
                //                strInitialInfo+=sim808.replybuffer;
                //                strInitialInfo="Get Signal Level: ";
                picture_loop(drawInitialPage);
                statement = INITIAL_OK;
                break;
                
            case INITIAL_OK:
                statement = SYS_RUN;
                break;
            default:
                break;
        }
    }
    
    if (statement == SYS_RUN) {
        // 200ms Timer
        static unsigned long timer200ms = millis();
        if(millis() - timer200ms >= 200){
            timer200ms = millis();
            
            // 读取 joyStick 的数值
            joyStick = myDev.readJoystick();
            
            // Keyboard actions
            //        if(joyStick == PUSH && page == PAGE_WMR) {wmr.clearAll();}
            //        if(joyStick == RIGHT) page = PAGE_GPRS;
            //        if(joyStick == LEFT) page = PAGE_WMR;
            
        }
        
        
        // 500ms Timer
        static unsigned long timer500ms = millis();
        if(millis() - timer500ms >= 500){
            timer500ms = millis();
            
            // 闪烁 LED 运行灯，标志系统在正常运行
            blinkLED();
            
            
            // OLED 显示
            if(page == PAGE_WMR) picture_loop(drawWmrPage);
            delay(100);
            
            
        }
        
        // 5000ms Timer
        static unsigned long timer5000ms = millis();
        if(millis() - timer5000ms >= 5000){
            timer5000ms = millis();
            
            // 刷新SIM808的信号强度
            sim808.getSignalLevel();
            
            // 10s
            if (simTemp1<2) {
                simTemp1++;
            }else{
                simTemp1=0;
                switch (simStep) {
                        // 连接模块状态初始化
                    case SIM_TCP_0:
                        //                        Serial.println(F("AT+CSTT=\"CMNET\""));
                        //                        delay(500);
                        //                        Serial.println(F("AT+CIICR"));
                        //                        delay(500);
                        //                        Serial.println(F("AT+CIFSR"));
                        //                        delay(2000);
                        //
                        //                        simStep = SIM_TCP_1;
                        //                        break;
                        //
                        //                    // 连接服务器
                        //                    case SIM_TCP_1:
                        Serial.print(F("AT+CIPSTART=\"TCP\",\""));
                        Serial.print(TCP_SERVER);
                        Serial.print(F("\",\""));
                        Serial.print(TCP_PORT);
                        Serial.println(F("\""));
                        //                        sim808.TCPConnect(TCP_SERVER, TCP_PORT);
                        
                        simStep = SIM_TCP_2;
                        break;
                        
                        // 发送请求
                    case SIM_TCP_2:
                        Serial.println(F("AT+CIPSEND"));
                        delay(500);
                        Serial.print(F("{\"TYPE\":\"REQ\",\"IMEI\":\""));
                        Serial.print(sim808.getIMEI());
                        Serial.println(F("\"}\x1a"));
                        
                        sim808.readline(500);
                        sim808.readline(500); // >
                        sim808.readline(500); //
                        sim808.readline(500); //
                        sim808.readline(500); // SEND OK
                        
                        sim808.readline(500); // recived
                        
                        strncpy(strSIMInfo, sim808.replybuffer, 8);
                        
                        // DEBUG: 串口打印数据
                        Serial.print("=>");
                        Serial.println(strSIMInfo);
                        
                        simStep = SIM_TCP_3;
                        break;
                        
                        // 收到命令,分析命令
                    case SIM_TCP_3:
                        Serial.print("cmp str: ");
                        Serial.println(strSIMInfo);
                        if ( strcmp(strSIMInfo, "READALL")==0) {
                            Serial.println("read all data");
                            simStep = SIM_TCP_4; // 发送所有的数据包
                            break;
                        }else if (strcmp(strSIMInfo, "CH")){
                            ;
                        }
                        simStep = SIM_TCP_10;
                        break;
                        
                        // 发送数据包
                    case SIM_TCP_4:
                        //                        sim808.TCPSendDataPack(sim808.getIMEI(), 100, 200, 300, 400);
                        Serial.println(F("AT+CIPSEND"));
                        delay(500);
                        
                        Serial.print(F("{\"TYPE\":\"DATA\",\"IMEI\":\""));
                        Serial.print(sim808.getIMEI());
                        // Serial.print("865067020893791");
                        Serial.print(F("\",\"D0\":\""));
                        Serial.print(wmr.data.ch0);
                        Serial.print(F("\",\"D1\":\""));
                        Serial.print(wmr.data.ch1);
                        Serial.print(F("\",\"D2\":\""));
                        Serial.print(wmr.data.ch2);
                        Serial.print(F("\",\"D3\":\""));
                        Serial.print(wmr.data.ch3);
                        Serial.println(F("\"}\x1A"));
                        
                        simStep = SIM_TCP_10;
                        break;
                        
                        //
                    case SIM_TCP_10:
                        simStep++;
                        //                        simStep = SIM_TCP_OFF;
                        sim808.TCPShut();
                        break;
                        
                        
                        // 断开连接
                    case SIM_TCP_OFF:
                        sim808.TCPShut();
                        simStep = SIM_TCP_0;
                        break;
                        
                    default:
                        simStep++;
                        break;
                }
            }
            
            //            // 发送数据包
            //            Serial.println(F("AT+CIPSEND"));
            //            delay(100);
            //            Serial.print(F("hello sim808!"));
            //            Serial.println(F("\x1a"));
        }
    }
    
    
}

//-------------------------------------
// Functions part
void blinkLED(void){
    if (_runLedState == 0) {
        _runLedState = 1;
        digitalWrite(RUNLED, HIGH);
    }
    else{
        _runLedState = 0;
        digitalWrite(RUNLED, LOW);
    }
}


// OLED Functions
void picture_loop(void (*draw_fn)(void)) {
    
    // picture loop
    myOled.firstPage();
    do {
        draw_fn();
    } while( myOled.nextPage() );
    
}


// 初始化屏幕
void drawInitialPage (void)
{
    
    // HEADER Define
    // myOled.setFont(u8g_font_unifont);
    // digitalWrite(RUNLED, HIGH);
    myOled.drawHLine(0,12,128);
    myOled.setFont(u8g_font_4x6);
    myOled.setPrintPos(3,10);
    myOled.print(sim808.signalLevelBar);
    //    myOled.print(F("______"));
    //     myOled.print(sim808.signalLevel);
    
    
    
    // myOled.setPrintPos(80,10);
    // myOled.print(F("CHINA MOBILE"));
    
    // myOled.setFont(u8g_font_6x10);
    myOled.setPrintPos(2,24);
    myOled.print(F("Initializing..."));
    myOled.setPrintPos(2,37);
    myOled.print(strInitialInfo);
    //    myOled.print(sim808.replybuffer);
    
    
    myOled.setPrintPos(2,64);
    myOled.print(statement);
    myOled.print(',');
    //    myOled.print(sim808.state);
    //    myOled.print(F("sim808.state"));
    myOled.print(joyStick);
    
}



void drawWmrPage (void)
{
    
    // HEADER Define
    // myOled.setFont(u8g_font_unifont);
    // digitalWrite(RUNLED, HIGH);
    myOled.drawHLine(0,12,128);
    myOled.setFont(u8g_font_4x6);
    myOled.setPrintPos(3,10);
    myOled.print(sim808.signalLevelBar);
    //    myOled.print(F("**____"));
    myOled.print(sim808.replybuffer);
    
    
    //    myOled.setPrintPos(80,10);
    //    myOled.print(F("CHINA MOBILE"));
    
    
    // Body Define
    // Body Font
    myOled.setFont(u8g_font_6x10);
    
    myOled.setPrintPos(3,25);
    myOled.print(F("CH0:"));
    myOled.setPrintPos(27,25);
    myOled.print(wmr.data.ch0);
    // myOled.setPrintPos(90,25);
    // myOled.print("2.10");
    
    
    myOled.setPrintPos(3,38);
    myOled.print(F("CH1:"));
    myOled.setPrintPos(27,38);
    myOled.print(wmr.data.ch1);
    // myOled.setPrintPos(90,38);
    // myOled.print("2.10");
    
    
    myOled.setPrintPos(3,51);
    myOled.print(F("CH2:"));
    myOled.setPrintPos(27,51);
    myOled.print(wmr.data.ch2);
    // myOled.setPrintPos(90,51);
    // myOled.print("2.10");
    
    
    myOled.setPrintPos(3,64);
    myOled.print(F("CH3:"));
    myOled.setPrintPos(27,64);
    myOled.print(wmr.data.ch3);
    // myOled.setPrintPos(90,64);
    // myOled.print("2.10");
    
    
}


