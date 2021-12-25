/*
 * FILE: adc_ts.c
 * ADC和触摸屏的测试函数
 */

#include <stdio.h>
#include "adc_ts.h"
#include "s3c24xx.h"
#include "serial.h"

// 外部变量声明
extern volatile int led_flag1;
// extern volatile int led_flag2;
// extern volatile int led_flag3;
extern volatile int game_flag1;
extern volatile int Next;
extern void delay(int us);
extern volatile int state;


volatile float xdata = 1;
volatile float ydata = 1;

// ADCCON寄存器
#define PRESCALE_DIS        (0 << 14)
#define PRESCALE_EN         (1 << 14)
#define PRSCVL(x)           ((x) << 6)
#define ADC_INPUT(x)        ((x) << 3)
#define ADC_START           (1 << 0)
#define ADC_ENDCVT          (1 << 15)

// ADCTSC寄存器
#define UD_SEN          (1 << 8)
#define DOWN_INT        (UD_SEN*0)
#define UP_INT          (UD_SEN*1)
#define YM_SEN          (1 << 7)
#define YM_HIZ          (YM_SEN*0)
#define YM_GND          (YM_SEN*1)
#define YP_SEN          (1 << 6)
#define YP_EXTVLT       (YP_SEN*0)
#define YP_AIN          (YP_SEN*1)
#define XM_SEN          (1 << 5)
#define XM_HIZ          (XM_SEN*0)
#define XM_GND          (XM_SEN*1)
#define XP_SEN          (1 << 4)
#define XP_EXTVLT       (XP_SEN*0)
#define XP_AIN          (XP_SEN*1)
#define XP_PULL_UP      (1 << 3)
#define XP_PULL_UP_EN   (XP_PULL_UP*0)
#define XP_PULL_UP_DIS  (XP_PULL_UP*1)
#define AUTO_PST        (1 << 2)
#define CONVERT_MAN     (AUTO_PST*0)
#define CONVERT_AUTO    (AUTO_PST*1)
#define XP_PST(x)       (x << 0)

#define NOP_MODE        0
#define X_AXIS_MODE     1
#define Y_AXIS_MODE     2
#define WAIT_INT_MODE   3


/* 设置进入等待中断模式，XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En
 * (1)对于S3C2410，位[8]只能为0，所以只能使用下面的wait_down_int，
 *    它既等待Pen Down中断，也等待Pen Up中断
 * (2)对于S3C2440，位[8]为0、1时分别表示等待Pen Down中断或Pen Up中断
 */
/* 进入"等待中断模式"，等待触摸屏被按下 */
#define wait_down_int() { ADCTSC = DOWN_INT | XP_PULL_UP_EN | \
                          XP_AIN | XM_HIZ | YP_AIN | YM_GND | \
                          XP_PST(WAIT_INT_MODE); }
/* 进入"等待中断模式"，等待触摸屏被松开 */
#define wait_up_int()   { ADCTSC = UP_INT | XP_PULL_UP_EN | XP_AIN | XM_HIZ | \
                          YP_AIN | YM_GND | XP_PST(WAIT_INT_MODE); }

/* 进入自动(连续) X/Y轴坐标转换模式 */
#define mode_auto_xy()  { ADCTSC = CONVERT_AUTO | XP_PULL_UP_DIS | XP_PST(NOP_MODE); }

extern void (*isr_handle_array[50])(void);

void React(void);

/* 
 * INT_TC的中断服务程序
 * 当触摸屏被按下时，进入自动(连续) X/Y轴坐标转换模式；
 * 当触摸屏被松开时，进入等待中断模式，再次等待INT_TC中断
 */       
static void Isr_Tc(void)
{
    if (ADCDAT0 & 0x8000)
    {
        //printf("Stylus Up!!\n\r");
        wait_down_int();    /* 进入"等待中断模式"，等待触摸屏被按下 */
    }
    else 
    {
        //printf("Stylus Down: ");

        mode_auto_xy();     /* 进入自动(连续) X/Y轴坐标转换模式 */ // 计算完成后触发Isr_Adc中断
    
        /* 设置位[0]为1，启动A/D转换
         * 注意：ADCDLY为50000，PCLK = 50MHz，
         *       要经过(1/50MHz)*50000=1ms之后才开始转换X坐标
         *       再经过1ms之后才开始转换Y坐标
         */
        ADCCON |= ADC_START;
    }
    
    // 清INT_TC中断
    SUBSRCPND |= BIT_SUB_TC;
    SRCPND    |= BIT_ADC;
    INTPND    |= BIT_ADC;
}


/* 
 * INT_ADC的中断服务程序
 * A/D转换结束时发生此中断
 * 先读取X、Y坐标值，再进入等待中断模式
 */       
static void Isr_Adc(void)
{
    // 打印X、Y坐标值    
    ydata = (float)(ADCDAT0 & 0x3ff);
    xdata = (float)(ADCDAT1 & 0x3ff);

    // 将触摸屏的xy坐标映射到LED的坐标中
    xdata = (xdata/1.7) - 52.94;
    ydata = (ydata/2.583) - 61.9435;


    printf("xdata = %4d, ydata = %4d\r\n", (int)xdata, (int)ydata);

    /* 判断是S3C2410还是S3C2440 */
    if ((GSTATUS1 == 0x32410000) || (GSTATUS1 == 0x32410002))
    {   // S3C2410
        wait_down_int();    /* 进入"等待中断模式"，等待触摸屏被松开 */
    }
    else
    {   // S3C2440
        wait_up_int();      /* 进入"等待中断模式"，等待触摸屏被松开 */
    }

    // 清INT_ADC中断
    SUBSRCPND |= BIT_SUB_ADC;
    SRCPND    |= BIT_ADC;
    INTPND    |= BIT_ADC;
}

/* 
 * ADC、触摸屏的中断服务程序
 * 对于INT_TC、INT_ADC中断，分别调用它们的处理程序
 */       
void AdcTsIntHandle(void)
{
    if (SUBSRCPND & BIT_SUB_TC)
        Isr_Tc();

    if (SUBSRCPND & BIT_SUB_ADC)
    {
        Isr_Adc();
        //printf("\n调用一次\n");
        React(); // 调用逻辑反应函数
    }
        


        


}

/* 
 * 测试触摸屏，打印触点坐标
 */       
void Test_Ts(void)
{
    isr_handle_array[ISR_ADC_OFT] = AdcTsIntHandle;    // 设置ADC中断服务程序
    INTMSK &= ~BIT_ADC;          // 开启ADC总中断
    INTSUBMSK &= ~(BIT_SUB_TC);  // 开启INT_TC中断，即触摸屏被按下或松开时产生中断
    INTSUBMSK &= ~(BIT_SUB_ADC); // 开启INT_ADC中断，即A/D转换结束时产生中断
    
    // 使能预分频功能，设置A/D转换器的时钟 = PCLK/(49+1)
    ADCCON = PRESCALE_EN | PRSCVL(49);

    /* 采样延时时间 = (1/3.6864M)*50000 = 13.56ms
     * 即按下触摸屏后，再过13.56ms才采样
     */
    ADCDLY = 50000;

    wait_down_int();    /* 进入"等待中断模式"，等待触摸屏被按下 */

    printf("Touch the screem to test, press any key to exit\n\r");    
    getc();

    // 屏蔽ADC中断
    INTSUBMSK |= BIT_SUB_TC;
    INTSUBMSK |= BIT_SUB_ADC;
    INTMSK |= BIT_ADC;
}

void Touch_Init(void)
{
    isr_handle_array[ISR_ADC_OFT] = AdcTsIntHandle;
    INTMSK &= ~BIT_ADC;          // 开启ADC总中断
    INTSUBMSK &= ~(BIT_SUB_TC);  // 开启INT_TC中断，即触摸屏被按下或松开时产生中断
    INTSUBMSK &= ~(BIT_SUB_ADC); // 开启INT_ADC中断，即A/D转换结束时产生中断
    ADCCON = PRESCALE_EN | PRSCVL(49);
    ADCDLY = 50000;
    wait_down_int();
}

void React(void)
{
    // 退出按键
    if( (xdata>429&&xdata<479) && (ydata>0 && ydata<50) )
    {
        state = 0;
    }

    if(state == 0)
    {
        if( (xdata>160&&xdata<320) && (ydata>10&&ydata<90) )
        {
            state = 1;
        }
        else if( (xdata>160&&xdata<320) && (ydata>100&&ydata<180) )
        {
            state = 2;
        }
        else if( (xdata>160&&xdata<320) && (ydata>190&&ydata<270) )
        {
            state = 3;
        }
    }
    else if(state == 2)
    {
        if( (xdata>20&&xdata<120) && (ydata>200 && ydata<250) )
        {
            led_flag1 = 1;
        }
        else if( (xdata>180&&xdata<280) && (ydata>200 && ydata<250) )
        {
            led_flag1 = 2;
        }
        else if( (xdata>340&&xdata<440) && (ydata>200 && ydata<250) )
        {
            led_flag1 = 3;
        }
    }
    else if(state == 3)
    {
        if( (xdata>20&&xdata<70) && (ydata>210&&ydata<260) )
        {
            game_flag1 = 1; // up
        }
        else if( (xdata>90&&xdata<140) && (ydata>210&&ydata<260) )
        {
            game_flag1 = 2; // down
        }
        else if( (xdata>160&&xdata<210) && (ydata>210&&ydata<260) )
        {
            game_flag1 = 3; // left
        }
        else if( (xdata>230&&xdata<280) && (ydata>210&&ydata<260) )
        {
            game_flag1 = 4; // right
        }
        else if( (xdata>300&&xdata<350) && (ydata>210&&ydata<260) )
        {
            game_flag1 = 5; // restart
        }
        else if( (xdata>370&&xdata<420) && (ydata>210&&ydata<260) ) 
        {
            game_flag1 = 6; // next
        }

    }
}