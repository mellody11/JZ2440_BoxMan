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

// �ⲿ��������
extern volatile int led_flag1;
// extern volatile int led_flag2;
// extern volatile int led_flag3;
extern volatile int Next;
extern void delay(int us);

// �жϷ���������
void EINT_0(void);
void EINT_2(void);
void EINT_5(void);

// ����S2
void EINT_0(void)
{

    led_flag1 = 1;
    // led_flag2 == 0;
    // led_flag3 == 0;

    printf("a");
    delay(3);
}

// ����S3
void EINT_2(void)
{
    led_flag1 = 2;
    // led_flag2 == 1;
    // led_flag3 == 0;

    printf("b");
    delay(3);
}

//����S4
void EINT_5(void)
{
    // ���
    if( ((EINTPEND >> 11)&(0x1)) == 1 )
    {
        EINTPEND = (1<<11);   
    }
    else if(((EINTPEND >> 19)&(0x1)) == 1)
    {
        EINTPEND = (1<<19);
    }
    // ִ��
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
    // S2,S3��Ӧ��2��������Ϊ�ж����� EINT0,EINT2
    GPFCON &= ~(GPF0_msk | GPF2_msk);
    GPFCON |= GPF0_eint | GPF2_eint;

    // S4,S5��Ӧ��2��������Ϊ�ж����� EINT11,EINT19
    GPGCON &= ~(GPG3_msk | GPG11_msk);
    GPGCON |= GPG3_eint | GPG11_eint;

    isr_handle_array[0] = EINT_0;
    isr_handle_array[2] = EINT_2;
    isr_handle_array[5] = EINT_5;

    
    // ����EINT11��EINT19����Ҫ��EINTMASK�Ĵ�����ʹ����
    EINTMASK &= ~((1<<11) | (1<<19));
        
    /*
     * �趨���ȼ���
     * ARB_SEL0 = 00b, ARB_MODE0 = 0: REQ1 > REQ3����EINT0 > EINT2
     * �ٲ���1��6��������
     * ���գ�
     * EINT0 > EINT2 > EINT11��K2 > K3 > K4
     */
    PRIORITY = (PRIORITY & ((~0x01) | (0x3<<7))) | (0x0 << 7) ;

    INTMSK   &= (~(1<<0)) & (~(1<<2)) & (~(1<<5));
}