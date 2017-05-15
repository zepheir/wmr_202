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

// Core library for code-sense - IDE-based
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(ROBOTIS) // Robotis specific
#include "libpandora_types.h"
#include "pandora.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#include "LRF.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#include "Arduino.h"
#elif defined(RFDUINO) // RFduino specific
#include "Arduino.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#include "application.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

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

//
uint32_t transData(char* data);

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
    uint8_t _i;
    
    //-----------------------------------------------------------------------
    // TODO: 常规时钟, 每2秒钟执行一次程序
    // @Notice: 需要注意的是每一步的执行时间不能超过2秒钟,
    //          而且综合考虑其他程序的执行时间, 希望可以在更短的时间内完成单组程序
    //
    static unsigned long timerInitial = millis();
    if (millis()- timerInitial >= 2000) {
        timerInitial = millis();
        
        // TODO: 每2秒慢闪, 初始化循环指示灯
        if(statement != SYS_RUN) blinkLED();
        
        // TODO: 初始化显示模块和SIM模块
        // 初始化开始
        switch (statement) {
            // TODO: 初始化显示模块
            // 显示输出: 初始化页面
            case INITIAL_DISPLAY:
                page = PAGE_WMR;
                strInitialInfo="Initial Display: OK!";
                picture_loop(drawInitialPage);
                statement = INITIAL_SIM808_1;
                break;
                
            // TODO: 开始初始化SIM模块
            // 执行动作: 控制POWERKEY, 给SIM模块上电
            // 显示输出: 初始化页面, 显示 "Waiting: "
            case INITIAL_SIM808_1:
                // 显示输出
                strInitialInfo="Waiting: ";
                strInitialInfo+=simTemp1;
                picture_loop(drawInitialPage);
                
                // 给sim808模块上点启动
//                // 如果之前模块没有上电，则给上电模块启动
//                sim808.flushInput();
//                if (!sim808.sendCheckReply(F("ATE0"), F("OK"), 500)) {
//                    sim808.simPwrUpDown(SIM_PWR_PIN);
//                }
                sim808.simPwrUpDown(SIM_PWR_PIN); // 直接上电
                
                // 下一步
                statement = INITIAL_SIM808_2;
                break;
            
            // TODO: 等待SIM模块完成初始化
            // 执行动作: 输出 ATE0 命令, 关闭SIM模块的ECHO功能
            // 显示输出: 初始化页面, 并显示"Waiting: 1" => "Waiting: 5"
            // 这个过程大概需要十秒钟
            case INITIAL_SIM808_2:
                // 循环5次, 大约10秒
                if (simTemp1<5) {
                    // 设置sim808回复模式：echo off
                    sim808.sendCheckReply(F("ATE0"), F("OK"), 500);
                    
                    // 显示输出
                    simTemp1++;
                    strInitialInfo="Waiting: ";
                    strInitialInfo+=simTemp1;
                    picture_loop(drawInitialPage);
                    
                }else{
                    
                    // 下一步
                    statement = INITIAL_SIM808_3;
                    simTemp1=0;
                }
                break;
            
            // TODO: 读取SIM模块的IMEI值
            // 执行动作: 调用readIMEI(), 读取SIM模块IMEI
            // 显示输出: 初始化页面, 并显示读取状态.
            //          如果读取失败, 显示 "IMEI READ FAIL!";
            //          如果读取成功, 显示IMEI
            case INITIAL_SIM808_3:
                // 读取IMEI
                if(sim808.readIMEI()==0){
                    
                    // 如果读取失败, 显示 "IMEI READ FAIL!"
                    strInitialInfo="IMEI READ FAIL!";
                    picture_loop(drawInitialPage);
                    
                    // 没有下一步
                    break;
                }
                
                // 读取成功, 显示IMEI内容
                strInitialInfo="IMEI:";
                strInitialInfo+=sim808.getIMEI();
                picture_loop(drawInitialPage);
                
                // 下一步
                statement = INITIAL_SIM808_4;
                break;
            
            // TODO: 读取SIM讯号强度
            // 执行动作: 调用sim808.getSignalLevel(), 读取信号质量
            // 显示输出: 初始化页面, 显示信号的强弱指示
            case INITIAL_SIM808_4:
                // 读取GPRS信号
                sim808.getSignalLevel();
                
                // 显示输出
                strInitialInfo="Get Signal Level: ";
                strInitialInfo+=sim808.signalLevel;
                picture_loop(drawInitialPage);
                
                // 下一步
                statement = INITIAL_SIM808_5;
                break;
            
            // TODO: 初始化结束
            // 显示输出: 初始化页面, 显示 "SIM Module OK!"
            case INITIAL_SIM808_5:
                // 显示输出
                strInitialInfo="SIM Module OK!";
                picture_loop(drawInitialPage);
                
                // 下一步
                statement = INITIAL_OK;
                break;
            
            // TODO: 进入正常循环
            case INITIAL_OK:
                // 下一步
                statement = SYS_RUN;
                break;
            default:
                break;
        }
    }
    
    //------------------------------------------------------------------------------
    // TODO: 主循环, 包括
    //      - 读取键盘状态
    //      - 显示刷新
    //      - 与服务器通信
    // @Description: 在完成了初始化之后, 显示和SIM模块都可以准备工作了
    //               主循环的工作包括 读取键盘状态 显示刷新 服务器通讯
    //               - 键盘读取: 采用的是AD输入的方法, 驱动函数在SANJI模块里面, 刷新频率200ms
    //               - 显示刷新: 频率是500ms, 配合500ms快闪指示灯
    //               - 服务器通讯: 处理程序包括TCP初始化, 内容请求, 数据发送等 刷新频率是5000ms
    //
    // @Notic: 因为所有最小刷新频率是200ms, 所以原则上, 所有程序模块的运行周期都应该小于200ms
    //
    if (statement == SYS_RUN) {
        
        // TODO: 读取键盘状态
        // 200ms时钟
        static unsigned long timer200ms = millis();
        if(millis() - timer200ms >= 200){
            timer200ms = millis();
            
            // 读取 joyStick 的数值 (键盘状态, 包括上,下,左,右,按下和悬空6种状态)
            joyStick = myDev.readJoystick();
            
//            // Keyboard actions (从旧的程序里面复制过来的)
//            if(joyStick == PUSH && page == PAGE_WMR) {wmr.clearAll();}
//            if(joyStick == RIGHT) page = PAGE_GPRS;
//            if(joyStick == LEFT) page = PAGE_WMR;
            
        }
        
        // TODO: 显示刷新
        // 500ms时钟
        static unsigned long timer500ms = millis();
        if(millis() - timer500ms >= 500){
            timer500ms = millis();
            
            // 闪烁 LED 运行灯，标志系统在正常运行
            blinkLED();
            
            
            // OLED 显示
            if(page == PAGE_WMR) picture_loop(drawWmrPage);
            delay(100); // 显示稳固, 避免其他干扰, 或许可以改成10ms或20ms
            
            
        }
        
        // TODO: 服务器通讯
        // 5000ms时钟
        static unsigned long timer5000ms = millis();
        if(millis() - timer5000ms >= 5000){
            timer5000ms = millis();
            
            // 刷新SIM808的信号强度
            sim808.getSignalLevel();
            
            // 为了与服务器之间的稳定操作, 采用10秒的单步运行 (5*2=10)
            if (simTemp1<2) {
                simTemp1++;
            }else{
                simTemp1=0;
                
                // 总的操作步骤为20步, 其中5步为有SIM操作和数据连接
                switch (simStep) {
                    
                    // TODO: 连接模块状态初始化
                    // 采用直接建立TCP链接的方式
                    // 如果使用先建立场景貌似有些问题, 所以采用了直接建立TCP服务器的方式
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
                        // 连接服务器, 建立TCP链接
                        Serial.print(F("AT+CIPSTART=\"TCP\",\""));
                        Serial.print(TCP_SERVER);
                        Serial.print(F("\",\""));
                        Serial.print(TCP_PORT);
                        Serial.println(F("\""));
//                        sim808.TCPConnect(TCP_SERVER, TCP_PORT); //sim模块程序调用方式
                        
                        // 下一步
                        simStep = SIM_TCP_2;
                        break;
                        
                    // TODO: 发送请求, REQ, 并且接受指令
                    case SIM_TCP_2:
//                        sim808.readline(500); // 清除之前的残留
                        // 发送指令
                        Serial.println(F("AT+CIPSEND"));
//                        delay(500);
                        
                        // 等待 '>'
                        _i = 0;
                        while (sim808.replybuffer[0] != '>') {
                            sim808.readline(500); // '>'
                            if (_i>3) break;
                            _i++;
                        }
                        
                        // 发送数据
                        Serial.print(F("{\"TYPE\":\"REQ\",\"IMEI\":\""));
                        Serial.print(sim808.getIMEI());
                        Serial.println(F("\"}\x1a"));

                        
                        // 等待发送结束
                        _i = 0;
                        while (!prog_char_strcmp(sim808.replybuffer, (prog_char*)F("SEND OK"))==0) {
                            sim808.readline(1000); // 'SEND OK'
                            if (_i>3) break;
                            _i++;
                        }
                        
                        
                        // 接收从服务器来的数据
                        sim808.readline(1000); // recived
                        
                        // 将信息保存至strSIMInfo
                        strncpy(strSIMInfo, sim808.replybuffer, 8);
                        
                        // 判断是否需要设置数据
                        if ( strcmp(strSIMInfo, "SETDATA\0")==0) {
//                            // DEBUG: 串口打印数据
//                            // Serial.println("set all data");
//                            
                            // 设置模块数据
//                            unsigned long _d;
//                            sim808.replybuffer[8]<<24+sim808.replybuffer[9]<<16++sim808.replybuffer[11]
//                            _d = sim808.replybuffer[8]<<8;
//                            _d += sim808.replybuffer[9];
//                            _d = _d<<8;
//                            _d += sim808.replybuffer[10];
//                            wmr.data.ch0 = _d;
                            wmr.data.ch0 = transData(sim808.replybuffer+8);
                            wmr.data.ch1 = transData(sim808.replybuffer+12);
                            wmr.data.ch2 = transData(sim808.replybuffer+16);;
                            wmr.data.ch3 = transData(sim808.replybuffer+20);;
//
                            // 设置完成后, 去关闭连接
                            simStep = SIM_TCP_10; // 发送所有的数据包
                            break;
                        }
                        
                        
                        // DEBUG: 串口打印数据
//                        Serial.print("=>");
//                        Serial.println(strSIMInfo);
                        
                        // 下一步
                        simStep = SIM_TCP_3;
                        break;
                        
                    // TODO: 收到命令,分析命令
                    // @描述: 如果
                    case SIM_TCP_3:
                        // DEBUG: 串口打印数据
//                        Serial.print("cmp str: ");
//                        Serial.println(strSIMInfo);
                        
                        // 判断是否收到的是 "READALL\0"
                        if ( strcmp(strSIMInfo, "READALL\0")==0) {
                            // DEBUG: 串口打印数据
//                            Serial.println("read all data");
                            
                            // 下一步发送数据
                            simStep = SIM_TCP_4; // 发送所有的数据包
                            break;
                        }
                        
                        // 下一步, 如果不是发送数据, 去关闭链接
                        simStep = SIM_TCP_10;
                        break;
                        
                    // TODO: 发送数据包
                    case SIM_TCP_4:
                        // 发送数据
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
                        
                        // 下一步, 关闭连接
                        simStep = SIM_TCP_10;
                        break;
                        
                    // TODO: 关闭链接和场景
                    case SIM_TCP_10:
                        // 关闭连接和场景
                        sim808.TCPShut();

                        // 下一步
                        simStep++;
//                        simStep = SIM_TCP_OFF;
                        break;
                        
                        
                    // TODO: 再次断开连接, 重新启动连接程序
                    case SIM_TCP_OFF:
                        // 确认场景已经断开
                        sim808.TCPShut();
                        
                        // 下一步, 重新开始连接
                        simStep = SIM_TCP_0;
                        break;
                    
                    // TODO:等待重新通讯
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

// TODO: 修改字体, 在一个画面里面显示IMEI码
void drawWmrPage (void)
{
    
    // HEADER Define
    myOled.drawHLine(0,12,128);
    myOled.setFont(u8g_font_4x6);
    myOled.setPrintPos(3,10);
    myOled.print(sim808.signalLevelBar);
    //    myOled.print(F("**____"));
    myOled.print(F(" "));
    myOled.print(simStep);
    myOled.print(F(" "));
    myOled.print(sim808.replybuffer);
    
//        myOled.setPrintPos(80,10);
//        myOled.print(F("CHINA MOBILE"));
    
    
    // Body Define
//    myOled.setFont(u8g_font_4x6);
    myOled.setFont(u8g_font_6x10);
    
    myOled.setPrintPos(3,22);
    myOled.print(F("CH0: "));
    myOled.print(wmr.data.ch0);
    
    
    myOled.setPrintPos(3,32);
    myOled.print(F("CH1: "));
    myOled.print(wmr.data.ch1);
    
    
    myOled.setPrintPos(3,42);
    myOled.print(F("CH2: "));
    myOled.print(wmr.data.ch2);

    
    myOled.setPrintPos(3,52);
    myOled.print(F("CH3: "));
    myOled.print(wmr.data.ch3);
    
    
    // FOOTER DEFINE
    myOled.drawHLine(0,55,128);
    myOled.setPrintPos(3,64);
    myOled.print(F("IMEI "));
    myOled.print(sim808.getIMEI());
    
    
}


unsigned long transData(char* data)
{
    int8_t i=0;
    unsigned long _d =0;
    
    while (i<4) {
        _d = _d<<8;
        _d = _d + *(data+i);
        i++;
    }
    
    return _d;
}


