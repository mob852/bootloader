# 项目名称
实现串口远程升级，刷新固件，包含一个bootloader程序，两个application。

## 目录
- [背景](#背景)
- [功能](#功能)
- [安装与运行](#安装与运行)
- [使用说明](#使用说明)
- [文件结构](#文件结构)
- [引用](#引用)
  
## 背景
用于bootloader远程升级，刷新固件。使用串口，蓝牙，基于stm32f407zgt6，使用xshell的ymodem传输文件。

## 功能
stm32f407zgt6_bootloader 后台刷新固件
template_test 应用程序样例 不同应用程序使用其中的串口 stmflash实现跳转到bootloader
stm32f407zgt6_flashwr flash读写样例

## 安装与运行
1. **环境**：
    -stm32f407zgt6
3. **安装步骤**：
    - 直接烧录一次bootloader
    - 远程连接蓝牙 烧录程序
4. **运行说明**：
    - 遵循bootloader中指令进行运行
    - 运行app
    - 擦除flash
    - 跳转menu

## 使用说明
首先需要设置单片机从flash启动 boot0引脚为0
bootloader               程序使用起始flash地址               0x8000000 -0x8004000
settings                （程序各种跳转指令存放）的flash地址   0x8004000 -0x8008000
ApplicationAddress       应用程序的起始地址                  0x8010000 -0x8020000

编译application需要选择irom range

跳转app
直接跳转到ApplicationAddress

erase
擦除secotr 0x8010000 -0x8020000

application 
使用一个蓝牙HC-06 波特率115200 应用单片机串口2
设置接收中断 如果接收到menu/update等指令 改写setting 然后reset 进入bootlader 执行相应操作


## 文件结构

```markdown
├── stm32f407zgt6_bootloader               # bootloader
├── template_test/                         # 应用程序样例
├── stm32f407zgt6_flashwr/                 # flash读写样例
└── README.md           # 项目说明
```

## 引用
该项目基于(https://github.com/havenxie/stm32-iap-uart-boot)进行
修改了部分代码适配stm32f407
关于flash的擦写 从page到sector
application address的选择
修改生成bin文件的方式 直接修改keil user setting
串口接收中断设置 命令解析

