# JZ2440_BoxMan
在裸机JZ2440上写一个推箱子游戏~

这是嵌入式系统期末大作业，做得有点简单哈，花的时间也不多，一共**两天晚上+周末两天**

## 首先声明：  
* 代码为裸机代码，没有uboot没有内核更没有文件系统，底层的驱动使用教程自带的裸机源码  
* 推箱子游戏的各类图片大多下载自<https://github.com/ntzw/ChestGame> ，在此感谢这位博主

## 此工程的一些概述：    
1. 代码成功烧录到机器上会黑屏，要等待一段时间（从nandflash中拷贝代码到sdram需要一段时间）  
2. pic文件夹下是本次工程所使用的图片，二级目录game是游戏用到的图片，三级目录use是小尺寸图片和对应的c数组  
3. image2lcd文件夹里的是image2lcd软件，注意使用的时候要激活，否则有水印（image2lcd的使用最好找找资料，多尝试，会有坑）
4. 使用触摸屏控制，触摸时最好使用附带的触摸笔，触屏只实现了点击触屏，没有实现拖拽触屏  
5. 代码开始运行后是一个简易的选择界面，由上至下前两个选项是作业要求，最后一个选项是推箱子游戏  
6. 游戏目前有5关（最后一关有点难，我也不太会玩）


## 关于代码文件的一些描述：  
1. 代码在code文件夹下，lib和include文件夹下是一些头文件和库文件，不需要对他们进行改动  
2. 图像数组存放在framebuffer.c文件中，数组名中包含分辨率信息，画图的底层函数（画点）也在该文件中
3. 画图函数、游戏代码、游戏数据在lcdlib.c文件中，游戏代码比较累赘，时间有限也没有做优化，有少量注释，希望对大家有所帮助
4. 触摸屏代码在adc_ts.c文件中，触摸时通过中断实现坐标的读取与映射转换计算（由于电阻屏坐标和lcd坐标范围不同，为方便处理，读取电阻屏坐标后直接映射到lcd坐标范围中）
5. 触摸屏的中断有些复杂，在这里作简要说明：按下屏幕触发第一次中断，进入坐标自动转换模式，转换完成自动触发第二次中断；第二次中断进行坐标的读取和映射计算；触摸笔离开屏幕后触发第三次中断

## 一些效果图：
<img src="https://github.com/mellody11/JZ2440_BoxMan/blob/main/1.jpg" width="400px"><img src="https://github.com/mellody11/JZ2440_BoxMan/blob/main/2.jpg" width="400px">
<img src="https://github.com/mellody11/JZ2440_BoxMan/blob/main/3.jpg" width="400px"><img src="https://github.com/mellody11/JZ2440_BoxMan/blob/main/4.jpg" width="400px">
  
  ***
  未完待续，最后更新时间2022.1.5
  快考完试了，又可以更新了
