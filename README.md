# 磁定位系统 LocateMagnet

## 简介

​	这个项目来源于一个课设，要求使用AMR磁传感器对小磁铁进行三维空间定位，定位范围1cm x 1cm x 1cm，定位精度要求达到0.1mm。

​	本项目采用了5x5 AMR磁传感器阵列，在远超课设要求的范围下，同时满足精度要求，此外还能实时检测，画出移动轨迹。

<img src="./Render/PCBA.jpg" alt="PCBA" style="zoom:25%;" />

<img src="./Result/PCBA.jpg" style="zoom: 20%;" />

<img src="./Result/正向摆放.jpg" alt="正向摆放" style="zoom: 20%;" />

<img src="./Result/正向.png" alt="正向" style="zoom: 70%;" />

<img src="./Result/real_time.jpg" alt="Real time" style="zoom: 80%;" />



## 文件结构

- Firmware: STM32代码
- Hardware: PCB工程
- Mechanical: 测试架和磁铁胶囊的3D模型
- Result: 测试结果
- Software: MATLAB上位机代码
- Tools: WCHLINK固件、烧录器、配置器
- Render: 渲染图

## 运行环境

- Keil 5
- STM32CubeMX
- KiCAD 6
- MATLAB **2023b**

## 复刻流程

1. PCB投板：可以直接拿gerber压缩包去投板

2. 购买元件：bom文件夹里有元件清单

3. 焊接：建议使用回流焊，没有的可以用热风枪代替，建议**先焊接除传感器外的其他元件**，测试可用后，再焊接传感器。该芯片是WLP晶圆级封装，很小，可以先在焊盘上涂助焊剂，放上芯片，热风枪吹焊，同时在显微镜下观察，当芯片轻微移动归位时，用镊子轻轻推动，如果芯片被推后能够归位，说明焊接成功。

4. WCHLINK固件烧录：PCB板载一个WCHLINK，方便调试，先**短接**PCB上的**UPG焊盘**，连接PC，检测到设备后，使用**WCHISPTool**下载`WCHLINK_V02.03.bin`，然后把UPG焊盘断开，再次连接PC，初次烧录固件，WCHLINK处于RISC-V模式，打开WCH-LinkUtility，点Get获取当前Mode，切换成**WCH-LinkDAP-WINUSB**，点Set，配置好后WCHLINK会变成ARM模式，如果使用Keil能检测到stm32芯片就算成功

5. stm32固件烧录：正常使用Keil下载代码，调试器选择DAPLINK

    Optional：如果板载芯片是GD32，请安装`Tools/GigaDevice.GD32F10x_DFP.2.0.3.pack`，然后选择`..._GD32_Compatible`的Keil工程下载

6. 串口调试助手：使用DAPLINK自带的串口输出数据，如果传感器都正常，应该能读到U ··· V ··· W ···系列数据，不会有No Ack提示

7. 打印测试架： 注意测试架只能放入直径3mm的小磁铁

8. 打开`compute_NT.mlx`：串口号要改成自己的串口，逐块运行代码，先移开磁体，获得校准矩阵，然后放上磁体，填入磁铁真实位置，如果读到的是空矩阵，再运行两遍尝试，最后计算出N_T这个和磁铁磁化强度相关的常数

9. 打开`real_time.m`：注意MATLAB必须先定位到`real_time`文件夹，填入N_T常数，先移开磁体，运行，然后再放上磁铁，最开始会有点卡，连续运行就会变流畅

## 相关链接

> [Arm Keil | Devices](https://www.keil.arm.com/devices/)
