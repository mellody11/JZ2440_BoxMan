#include <stdio.h>
#include "serial.h"
#include "lcdlib.h"
#include "s3c24xx.h"
#include "lcddrv.h"
#include "framebuffer.h"
#include "GPIO_Key.h"
#include "adc_ts.h"

extern const unsigned char test_480_272[261120];
// 三个选择按键
extern const unsigned char gImage_my_photo_160_80[25600];
extern const unsigned char gImage_LED_160_80[25600];
extern const unsigned char gImage_boxman_160_80[25600];
// 退出按键
extern const unsigned char gImage_exit_50_50[5000];;

volatile int Next = 1;  // 下一步判断
volatile int Exit = 1;  // 退出判断
volatile int led_flag1 = 1;
volatile int led_flag2 = 1;
volatile int led_flag3 = 1;
volatile int game_flag1 = 1;

volatile int state = 1;


void delay(int us)
{
    
    for(;us>0;us--)
    {
        int i = 100000;
        for(;i>0;i--);
    }
}


int main()
{
    char c;
    Key_Int_Init();
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
    Touch_Init();

    led_flag1 = 0;
    Next = 0;
    state = 0;
    game_flag1 = 0;


    while (1)
    {
        Lcd_Port_Init();                     // 设置LCD引脚
        Tft_Lcd_Init(MODE_TFT_16BIT_480272); // 初始化LCD控制器
        Lcd_PowerEnable(0, 1);               // 设置LCD_PWREN有效，它用于打开LCD的电源
        Lcd_EnvidOnOff(1);                   // 使能LCD控制器输出信号
        ClearScr(0);  // 清屏，黑色

        
        while(1)
        {
            // 主界面
            ClearScr(0);
            state = 0;
            ShowPicture(160,10,160,80,gImage_my_photo_160_80);
            ShowPicture(160,100,160,80,gImage_LED_160_80);
            ShowPicture(160,190,160,80,gImage_boxman_160_80);
            while(state == 0);
            switch(state)
            {
                case 1:
                {
                    ClearScr(0);
                    ShowPicture(0,0,480,272,test_480_272);
                    ShowPicture(429,0,50,50,gImage_exit_50_50);
                    while(state);
                    break;
                }
                case 2:
                {
                    ClearScr(0);
                    Lcd_Led();  // 里面自带循环
                    break;
                }
                case 3:
                {
                    ClearScr(0);
                    Lcd_game(); // 里面自带循环
                    break;
                }
                default:
                {
                    break;
                }
            }


        }

    }
    
    return 0;
}
