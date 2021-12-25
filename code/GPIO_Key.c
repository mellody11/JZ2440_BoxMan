#include "GPIO_Key.h"

/* GPIO_Interrupt*/
#define GPF0_eint     (0x2<<(0*2))
#define GPF2_eint     (0x2<<(2*2))
#define GPG3_eint     (0x2<<(3*2))
#define GPG11_eint    (0x2<<(11*2))

#define GPF0_msk    (3<<(0*2))
#define GPF2_msk    (3<<(2*2))
#define GPG3_msk    (3<<(3*2))
#define GPG11_msk   (3<<(11*2))

extern void (*isr_handle_array[50])(void);

// 外部变量声明
extern volatile int led_flag1;
// extern volatile int led_flag2;
// extern volatile int led_flag3;
extern volatile int Next;
extern void delay(int us);

// 中断服务函数声明
void EINT_0(void);
void EINT_2(void);
void EINT_5(void);

// 按下S2
void EINT_0(void)
{

    led_flag1 = 1;
    // led_flag2 == 0;
    // led_flag3 == 0;

    printf("a");
    delay(3);
}

// 按下S3
void EINT_2(void)
{
    led_flag1 = 2;
    // led_flag2 == 1;
    // led_flag3 == 0;

    printf("b");
    delay(3);
}

//按下S4
void EINT_5(void)
{
    // 清除
    if( ((EINTPEND >> 11)&(0x1)) == 1 )
    {
        EINTPEND = (1<<11);   
    }
    else if(((EINTPEND >> 19)&(0x1)) == 1)
    {
        EINTPEND = (1<<19);
    }
    // 执行
    if(((EINTPEND >> 11)&(0x1)) == 1) 
    {
        led_flag1 = 3;
        // led_flag2 == 0;
        // led_flag3 == 1;
        printf("d");           
    }
    else if(((EINTPEND >> 19)&(0x1)) == 1) // EINT19
    {
        Next = 1;
        printf("c");
    }  

    delay(3);

}


void Key_Int_Init(void)
{
    // S2,S3对应的2根引脚设为中断引脚 EINT0,EINT2
    GPFCON &= ~(GPF0_msk | GPF2_msk);
    GPFCON |= GPF0_eint | GPF2_eint;

    // S4,S5对应的2根引脚设为中断引脚 EINT11,EINT19
    GPGCON &= ~(GPG3_msk | GPG11_msk);
    GPGCON |= GPG3_eint | GPG11_eint;

    isr_handle_array[0] = EINT_0;
    isr_handle_array[2] = EINT_2;
    isr_handle_array[5] = EINT_5;

    
    // 对于EINT11和EINT19，需要在EINTMASK寄存器中使能它
    EINTMASK &= ~((1<<11) | (1<<19));
        
    /*
     * 设定优先级：
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3，即EINT0 > EINT2
     * 仲裁器1、6无需设置
     * 最终：
     * EINT0 > EINT2 > EINT11即K2 > K3 > K4
     */
    PRIORITY = (PRIORITY & ((~0x01) | (0x3<<7))) | (0x0 << 7) ;

    INTMSK   &= (~(1<<0)) & (~(1<<2)) & (~(1<<5));
}