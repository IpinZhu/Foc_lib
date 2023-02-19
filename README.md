## 重启Foc项目(Great FOC Restart Project)

### 1.概论和目标

基于STM32系列芯片的HAL库Foc代码库，和配套硬件电路

1.采用基本的Foc算法,支持开环和闭环，提供标准API接口

2.自动PID校准功能（待定）

3.支持有感电机和无感电机

4.引入龙伯格观测器，实现无感滤波

5.Rtos程序，可加入用户自定APP

6.宽电压输入范围和输出大电流

![image-20230219190743278](https://raw.githubusercontent.com/IpinZhu/markdown/main/img/202302191907354.png?token=AZIZVKDHVJZMQJ4IPRBAS5LD6IBTW)

### 2.硬件

1.使用DRV8301作为三路逆变器的控制芯片，将功率模块和控制模块区分成上下半区部分。

2.主控采用STM32F405RGT6

3.提供一个外部SPI,一个CAN总线和一个复用的TypeC口，可同时将设备进行串口通信和作为USB设备使用

##### DRV8301

自带一个TPS54160Buck芯片，可以对6-60v输入进行降压处理，由于实际使用不超过25V，因此选取6-35V的输入范围，输出5V电压

![image-20230219190756580](https://raw.githubusercontent.com/IpinZhu/markdown/main/img/202302191907623.png?token=AZIZVKE3XI6M4E6L2A2CBCTD6IBUS)



### 3.软件

##### 1.foc.c和foc.h

主要由基本foc算法变换，参照st电机库，使用Q1.15形式（注.无法使用Q1.32格式，计算时会溢出）



##### SVPWM：

![image-20230219190812114](https://raw.githubusercontent.com/IpinZhu/markdown/main/img/202302191908186.png?token=AZIZVKEOOYIHSLXSHZ3T2VTD6IBVQ)

![image-20230219190823987](https://raw.githubusercontent.com/IpinZhu/markdown/main/img/202302191908060.png?token=AZIZVKCSHXMC7TIB7EKTXELD6IBWI)

##### 定时器配置

![image-20230219190843773](https://raw.githubusercontent.com/IpinZhu/markdown/main/img/202302191908815.png?token=AZIZVKAAOB26WPENRYQP7CLD6IBXS)

分频系数为1-1=0

计数模式为中心计数模式，作用频率为10k，为168M/10K/2

RCR设置为1

### 4.RTOS

主任务

屏幕显示任务



### 5.上位机

懒得写。。。。。。

 Foc_lib
