#include "s3c24xx.h"

//extern void I2CIntHandle(void);



void (*isr_handle_array[50])(void);

void Dummy_isr(void)
{
    while(1);
}

void init_irq(void)
{
    int i = 0;
    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++)
    {
        isr_handle_array[i] = Dummy_isr;
    }
    
    INTMOD = 0x0;	      // �����ж϶���ΪIRQģʽ
    INTMSK = BIT_ALLMSK;  // �����������ж�
    

//	isr_handle_array[ISR_IIC_OFT]  = I2CIntHandle;
}

void IRQ_Handle(void)
{
	unsigned long oft = INTOFFSET;
    
	//���ж�  oft == 4 �� oft == 5�������Ӧ���ⲿ�ж��ں����ڲ����
	// if (oft == 4)
    //     EINTPEND = 1<<7;    //EINT4-7����IRQ4��ע��EINTPEND[3:0]����δ�ã�����Щλд��1���ܵ���δ֪���
    SRCPND = 1<<oft;  
    INTPND = 1<<oft; 

    /* �����жϷ������ */
    isr_handle_array[oft]();
}




