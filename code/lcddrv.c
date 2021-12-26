/*
 * FILE: lcddrv.c
 * �ṩ����LCD����������ɫ��ȵĵײ㺯��
 */

#include <stdio.h>
#include "s3c24xx.h"
#include "lcddrv.h"

#define GPB0_tout0  (2<<(0*2))
#define GPB0_out    (1<<(0*2))
#define GPB1_out    (1<<(1*2))

#define GPB0_MSK    (3<<(0*2))
#define GPB1_MSK    (3<<(1*2))


unsigned int fb_base_addr;
unsigned int bpp;
unsigned int xsize;
unsigned int ysize;

static const unsigned short DEMO256pal[]={
    0x0b5e,0xce9a,0xffd9,0x9d99,0xb63a,0xae7c,0xdd71,0x6c57,0xfd4d,0x00ae,0x9c4d,0xb5f8,0xad96,0x0131,0x0176,0xefff,0xcedd,0x9556,0xe4bf,0x00b6,0x22b7,0x002b,0x89de,0x002c,0x57df,0xab5f,0x3031,0x14bf,0x797e,0x5391,0x93ab,0x7239,0x7453,0xafdf,0x71b9,0x8c92,0x014d,0x302e,0x5175,0x0029,0x0969,0x004e,0x2a6d,0x0021,0x3155,0x4b6e,0xd677,0xf6b6,0x9b5f,0x4bb5,0xffd5,0x0027,0xdfdf,0x74d8,0x1256,0x6bcd,0x9b08,0x2ab2,0xbd72,0x84b5,0xfe52,0xd4ad,0x00ad,0xfffc,0x422b,0x73b0,0x0024,0x5246,0x8e5e,0x28b3,0x0050,0x3b52,0x2a4a,0x3a74,0x8559,0x3356,0x1251,0x9abf,0x4034,0x40b1,
    0x8cb9,0x00b3,0x5c55,0xdf3d,0x61b7,0x1f5f,0x00d9,0x4c59,0x0926,0xac3f,0x925f,0x85bc,0x29d2,0xc73f,0xef5c,0xcb9f,0x827b,0x5279,0x4af5,0x01b9,0x4290,0xf718,0x126d,0x21a6,0x515e,0xefbd,0xd75e,0x42ab,0x00aa,0x10b3,0x7349,0x63b5,0x61a3,0xaadf,0xcb27,0x87df,0x6359,0xc7df,0x4876,0xb5bc,0x4114,0xfe2e,0xef5e,0x65be,0x43b9,0xe5df,0x21c9,0x7d16,0x6abb,0x5c11,0x49f7,0xbc0b,0x9e1a,0x3b0f,0x202b,0xff12,0x821b,0x842f,0xbccf,0xdefb,0x8a3e,0x68fa,0xa4f1,0x38ae,0x28b7,0x21ad,0x31d7,0x0073,0x182b,0x1831,0x3415,0xbdf6,0x2dbf,0x0a5d,0xc73d,0x182c,0x293e,0x7b3d,0x643d,0x3cbd,
    0x92dd,0x09d4,0x1029,0x7cdd,0x6239,0x182e,0x5aea,0x11eb,0x8abc,0x7bfa,0x00a7,0x2153,0x1853,0x1318,0x0109,0x54fa,0x72a7,0x89e3,0x01cf,0x3a07,0x7b17,0x1a14,0x2150,0x23dc,0x4142,0x1b33,0x00a4,0xf6df,0x08fc,0x18ae,0x3a7e,0x18d1,0xa51c,0xff5a,0x1a0f,0x28fa,0xdfbe,0x82de,0x60d7,0x1027,0x48fa,0x5150,0x6213,0x89d6,0x110d,0x9bbb,0xbedd,0x28e1,0x1925,0xf449,0xaa79,0xd5f4,0x693c,0x110a,0x2889,0x08a2,0x923d,0x10a6,0xd9bc,0x5b2e,0x32ec,0xcf7f,0x1025,0x2148,0x74b4,0x6d59,0x9d14,0x0132,0x00f0,0x56bf,0x00f1,0xffff,0x0173,0x0133,0x00b0,0x00b1,0xf7ff,0x08b1,0xfffe,0x08b0,
    0x0171,0xf7bf,0x10f3,0xf7fe,0x08ef,0x1192,0xefbe,0x1131,0x2177,0xff9f,0x1116,0xffbc,0x5914,0x22ef,0xb285,0xa6df,
};

/*
 * ��ʼ������LCD������
 */
void Lcd_Port_Init(void)
{
    GPCUP   = 0xffffffff;   // ��ֹ�ڲ�����
    GPCCON  = 0xaaaaaaaa;   // GPIO�ܽ�����VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
    GPDUP   = 0xffffffff;   // ��ֹ�ڲ�����
    GPDCON  = 0xaaaaaaaa;   // GPIO�ܽ�����VD[23:8]
  	GPBCON &= ~(GPB0_MSK);  // Power enable pin
    GPBCON |= GPB0_out;
    GPBDAT &= ~(1<<0);			// Power off
    printf("Initializing GPIO ports..........\n");
}

/*
 * ��ʼ��LCD������
 * ���������
 * type: ��ʾģʽ
 *      MODE_TFT_8BIT_240320  : 240*320 8bpp��TFT LCD
 *      MODE_TFT_16BIT_240320 : 240*320 16bpp��TFT LCD
 *      MODE_TFT_8BIT_640480  : 640*480 8bpp��TFT LCD
 *      MODE_TFT_16BIT_640480 : 640*480 16bpp��TFT LCD
 */
void Tft_Lcd_Init(int type)
{
    switch(type)
    {
    case MODE_TFT_8BIT_240320:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 8BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ8BPPʱ����ɫ���е����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    �ֽڽ���ʹ��
         */
        LCDCON1 = (CLKVAL_TFT_240320<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_8BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (VBPD_240320<<24) | (LINEVAL_TFT_240320<<14) | \
                  (VFPD_240320<<6) | (VSPW_240320);
        LCDCON3 = (HBPD_240320<<19) | (HOZVAL_TFT_240320<<8) | (HFPD_240320);
        LCDCON4 = HSPW_240320;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (BSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ����(8BPPʱ��֡�������е�����Ϊ��ɫ���е�����ֵ)��
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       239
         *     0   idx idx idx ... idx
         *     1   idx idx idx ... idx
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(240/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (LINEVAL_TFT_240320+1)*(HOZVAL_TFT_240320+1)*1)>>1);
        LCDSADDR3 = (0<<11) | (LCD_XSIZE_TFT_240320/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 8;
        xsize = 240;
        ysize = 320;
        
        break;

    case MODE_TFT_16BIT_240320:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 16BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ16BPPʱ�����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    ����(2�ֽ�)����ʹ��
         */
        LCDCON1 = (CLKVAL_TFT_240320<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_16BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (VBPD_240320<<24) | (LINEVAL_TFT_240320<<14) | \
                  (VFPD_240320<<6) | (VSPW_240320);
        LCDCON3 = (HBPD_240320<<19) | (HOZVAL_TFT_240320<<8) | (HFPD_240320);
        LCDCON4 = HSPW_240320;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (HWSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ���£�
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       239
         *     0   rgb rgb rgb ... rgb
         *     1   rgb rgb rgb ... rgb
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(240*2/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (LINEVAL_TFT_240320+1)*(HOZVAL_TFT_240320+1)*2)>>1);
        LCDSADDR3 = (0<<11) | (LCD_XSIZE_TFT_240320*2/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 16;
        xsize = 240;
        ysize = 320;

        break;

    case MODE_TFT_8BIT_480272:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 8BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ8BPPʱ����ɫ���е����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    �ֽڽ���ʹ��
         */
        LCDCON1 = (4<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_8BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (1<<24) | (271<<14) | \
                  (1<<6) | (9);
        LCDCON3 = (1<<19) | (479<<8) | (1);
        LCDCON4 = 40;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (BSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ����(8BPPʱ��֡�������е�����Ϊ��ɫ���е�����ֵ)��
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       239
         *     0   idx idx idx ... idx
         *     1   idx idx idx ... idx
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(240/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (480)*(272)*1)>>1);
        LCDSADDR3 = (0<<11) | (480/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 8;
        xsize = 480;
        ysize = 272;
        
        break;

    case MODE_TFT_16BIT_480272:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 16BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ16BPPʱ�����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    ����(2�ֽ�)����ʹ��
         */
        LCDCON1 = (4<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_16BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (1<<24) | (271<<14) | \
                  (1<<6) | (9);
        LCDCON3 = (1<<19) | (479<<8) | (1);
        LCDCON4 = 40;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (HWSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ���£�
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       239
         *     0   rgb rgb rgb ... rgb
         *     1   rgb rgb rgb ... rgb
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(240*2/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (480)*(272)*2)>>1);
        LCDSADDR3 = (0<<11) | (480*2/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 16;
        xsize = 480;
        ysize = 272;

        break;

    case MODE_TFT_8BIT_640480:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 8BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ8BPPʱ����ɫ���е����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    �ֽڽ���ʹ��
         */
        LCDCON1 = (CLKVAL_TFT_640480<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_8BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (VBPD_640480<<24) | (LINEVAL_TFT_640480<<14) | \
                  (VFPD_640480<<6) | (VSPW_640480);
        LCDCON3 = (HBPD_640480<<19) | (HOZVAL_TFT_640480<<8) | (HFPD_640480);
        LCDCON4 = HSPW_640480;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (BSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ����(8BPPʱ��֡�������е�����Ϊ��ɫ���е�����ֵ)��
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       639
         *     0   idx idx idx ... idx
         *     1   idx idx idx ... idx
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(640/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (LINEVAL_TFT_640480+1)*(HOZVAL_TFT_640480+1)*1)>>1);
        LCDSADDR3 = (0<<11) | (LCD_XSIZE_TFT_640480/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 8;
        xsize = 640;
        ysize = 480;
        
        break;
        
    case MODE_TFT_16BIT_640480:
        /* 
         * ����LCD�������Ŀ��ƼĴ���LCDCON1~5
         * 1. LCDCON1:
         *    ����VCLK��Ƶ�ʣ�VCLK(Hz) = HCLK/[(CLKVAL+1)x2]
         *    ѡ��LCD����: TFT LCD   
         *    ������ʾģʽ: 16BPP
         *    �Ƚ�ֹLCD�ź����
         * 2. LCDCON2/3/4:
         *    ���ÿ����źŵ�ʱ�����
         *    ���÷ֱ��ʣ�������������
         * ���ڣ����Ը��ݹ�ʽ�������ʾ����Ƶ�ʣ�
         * ��HCLK=100MHzʱ��
         * Frame Rate = 1/[{(VSPW+1)+(VBPD+1)+(LIINEVAL+1)+(VFPD+1)}x
         *              {(HSPW+1)+(HBPD+1)+(HFPD+1)+(HOZVAL+1)}x
         *              {2x(CLKVAL+1)/(HCLK)}]
         *            = 60Hz
         * 3. LCDCON5:
         *    ������ʾģʽΪ16BPPʱ�����ݸ�ʽ: 5:6:5
         *    ����HSYNC��VSYNC����ļ���(����Ҫ�ο�����LCD�Ľӿ��ź�): ��ת
         *    ����(2�ֽ�)����ʹ��
         */
        LCDCON1 = (CLKVAL_TFT_640480<<8) | (LCDTYPE_TFT<<5) | \
                  (BPPMODE_16BPP<<1) | (ENVID_DISABLE<<0);
        LCDCON2 = (VBPD_640480<<24) | (LINEVAL_TFT_640480<<14) | \
                  (VFPD_640480<<6) | (VSPW_640480);
        LCDCON3 = (HBPD_640480<<19) | (HOZVAL_TFT_640480<<8) | (HFPD_640480);
        LCDCON4 = HSPW_640480;
        LCDCON5 = (FORMAT8BPP_565<<11) | (HSYNC_INV<<9) | (VSYNC_INV<<8) | \
                  (HWSWP<<1);

        /*
         * ����LCD�������ĵ�ַ�Ĵ���LCDSADDR1~3
         * ֡�ڴ����ӿ�(view point)��ȫ�Ǻϣ�
         * ͼ�����ݸ�ʽ���£�
         *         |----PAGEWIDTH----|
         *    y/x  0   1   2       639
         *     0   rgb rgb rgb ... rgb
         *     1   rgb rgb rgb ... rgb
         * 1. LCDSADDR1:
         *    ����LCDBANK��LCDBASEU
         * 2. LCDSADDR2:
         *    ����LCDBASEL: ֡�������Ľ�����ַA[21:1]
         * 3. LCDSADDR3:
         *    OFFSIZE����0��PAGEWIDTH����(640*2/2)
         */
        LCDSADDR1 = ((LCDFRAMEBUFFER>>22)<<21) | LOWER21BITS(LCDFRAMEBUFFER>>1);
        LCDSADDR2 = LOWER21BITS((LCDFRAMEBUFFER+ \
                    (LINEVAL_TFT_640480+1)*(HOZVAL_TFT_640480+1)*2)>>1);
        LCDSADDR3 = (0<<11) | (LCD_XSIZE_TFT_640480*2/2);

        /* ��ֹ��ʱ��ɫ��Ĵ��� */
        TPAL = 0;

        fb_base_addr = LCDFRAMEBUFFER;
        bpp = 16;
        xsize = 640;
        ysize = 480;

        break;

    default:
        break;
    }   
}

/*
 * ���õ�ɫ��
 */
void Lcd_Palette8Bit_Init(void)
{
    int i;  
    volatile unsigned int *palette;

	LCDCON1 &= ~0x01;	// stop lcd controller
    
    LCDCON5 |= (FORMAT8BPP_565<<11); // ���õ�ɫ�������ݸ�ʽΪ5:6:5

    palette = (volatile unsigned int *)PALETTE;
    for (i = 0; i < 256; i++)
        *palette++ = DEMO256pal[i];

	LCDCON1 |= 0x01;	// re-enable lcd controller
}

/*
 * �ı��ɫ��Ϊһ����ɫ
 * ���������
 *     color: ��ɫֵ����ʽΪ0xRRGGBB
 */
void ChangePalette(UINT32 color)
{
    int i;
    unsigned char red, green, blue;
    UINT32 *palette;

	red   = (color >> 19) & 0x1f;
	green = (color >> 10) & 0x3f;
	blue  = (color >>  3) & 0x1f;
	color = (red << 11) | (green << 5) | blue; // ��ʽ5:6:5
    
    palette=(UINT32 *)PALETTE;
	LCDCON1 &= ~0x01;	// stop lcd controller
    for (i = 0; i < 256; i++)
    {
//        while (((LCDCON5>>15) & 0x3) == 2);     // �ȴ�ֱ��VSTATUS��Ϊ����Ч��
        *palette++ = color;
    }
	LCDCON1 |= 0x01;	// re-enable lcd controller
}

/*
 * �����Ƿ����LCD��Դ�����ź�LCD_PWREN
 * ���������
 *     invpwren: 0 - LCD_PWREN��ЧʱΪ��������
 *               1 - LCD_PWREN��ЧʱΪ��ת����
 *     pwren:    0 - LCD_PWREN�����Ч
 *               1 - LCD_PWREN�����Ч
 */
void Lcd_PowerEnable(int invpwren, int pwren)
{
    GPGCON = (GPGCON & (~(3<<8))) | (3<<8);   // GPG4����LCD_PWREN
    GPGUP  = (GPGUP & (~(1<<4))) | (1<<4);    // ��ֹ�ڲ�����    
        
    LCDCON5 = (LCDCON5 & (~(1<<5))) | (invpwren<<5);  // ����LCD_PWREN�ļ���: ����/��ת
    LCDCON5 = (LCDCON5 & (~(1<<3))) | (pwren<<3);     // �����Ƿ����LCD_PWREN
}    

/*
 * ����LCD�������Ƿ�����ź�
 * ���������
 * onoff: 
 *      0 : �ر�
 *      1 : ��
 */
void Lcd_EnvidOnOff(int onoff)
{
    if (onoff == 1)
    {
        LCDCON1 |= 1;         // ENVID ON
		GPBDAT |= (1<<0);			// Power on
    }
    else
    {
        LCDCON1 &= 0x3fffe;  // ENVID Off
	    GPBDAT &= ~(1<<0);	 // Power off
    }
}    

/*
 * ʹ����ʱ��ɫ��Ĵ��������ɫͼ��
 * ���������
 *     color: ��ɫֵ����ʽΪ0xRRGGBB
 */
void ClearScrWithTmpPlt(UINT32 color)
{
    TPAL = (1<<24)|((color & 0xffffff)<<0);
}

/*
 * ֹͣʹ����ʱ��ɫ��Ĵ���
 */
void DisableTmpPlt(void)
{
    TPAL = 0;
}

