
  wmr_202
  Project
  ----------------------------------
  Developed with embedXcode

  Project wmr_202
  Created by Samuel Gao on 2017/4/28
  Copyright © 2017年 Samuel Gao
  Licence GNU General Public Licence



  References
  ----------------------------------



  embedXcode
  embedXcode+
  ----------------------------------
  Embedded Computing on Xcode
  Copyright © Rei VILO, 2010-2016
  All rights reserved
  http://embedXcode.weebly.com


主要功能:
1. 采集脉冲: 使用中断方式
2. GPRS数据通讯: SIM808
3. 数据远程设置: 使用SETDATA方法
4. 数据远程初始化: 使用INI进行请求
5. 模块远程请求指令: 使用REQ进行请求
6. EEPROM数据保存: 每小时保存一次, 在服务器连接初始化失败的时候, 读取eeprom内的数据进行初始化
7. oled显示: u8glib
