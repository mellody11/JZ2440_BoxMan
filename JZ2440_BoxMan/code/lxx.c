#include <stdio.h>
#include "serial.h"
#include "lcdlib.h"
#include "s3c24xx.h"
#include "lcddrv.h"
#include "framebuffer.h"
#include "GPIO_Key.h"
#include "adc_ts.h"

extern const unsigned char test_480_272[261120];
// ����ѡ�񰴼�
extern const unsigned char gImage_my_photo_160_80[25600];
extern const unsigned char gImage_LED_160_80[25600];
extern const unsigned char gImage_boxman_160_80[25600];
// �˳�����
extern const unsigned char gImage_exit_50_50[5000];;

volatile int Next = 1;  // ��һ���ж�
volatile int Exit = 1;  // �˳��ж�
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
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)
    Touch_Init();

    led_flag1 = 0;
    Next = 0;
    state = 0;
    game_flag1 = 0;


    while (1)
    {
        Lcd_Port_Init();                     // ����LCD����
        Tft_Lcd_Init(MODE_TFT_16BIT_480272); // ��ʼ��LCD������
        Lcd_PowerEnable(0, 1);               // ����LCD_PWREN��Ч�������ڴ�LCD�ĵ�Դ
        Lcd_EnvidOnOff(1);                   // ʹ��LCD����������ź�
        ClearScr(0);  // ��������ɫ

        
        while(1)
        {
            // ������
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
                    Lcd_Led();  // �����Դ�ѭ��
                    break;
                }
                case 3:
                {
                    ClearScr(0);
                    Lcd_game(); // �����Դ�ѭ��
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
