/*
 * FILE: framebuffer.h
 * ��framebuffer�ϻ��㡢���ߡ���ͬ��Բ�������ĺ����ӿ�
 */

#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include <types.h>

/* 
 * ����
 * ���������
 *     x��y : ��������
 *     color: ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void PutPixel(UINT32 x, UINT32 y, UINT32 color);

/* 
 * ����
 * ���������
 *     x1��y1 : �������
 *     x2��y2 : �յ�����
 *     color  : ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void DrawLine(int x1,int y1,int x2,int y2,int color);

/* 
 * ����ͬ��Բ
 */
void Mire(void);

/* 
 * ����Ļ��ɵ�ɫ
 * ���������
 *     color: ��ɫֵ
 *         ����16BPP: color�ĸ�ʽΪ0xAARRGGBB (AA = ͸����),
 *     ��Ҫת��Ϊ5:6:5��ʽ
 *         ����8BPP: colorΪ��ɫ���е�����ֵ��
 *     ����ɫȡ���ڵ�ɫ���е���ֵ
 */
void ClearScr(UINT32 color);

void DrawPixel(UINT32 x, UINT32 y, UINT8 col_high, UINT8 col_low);

void DrawPixel2(UINT32 x, UINT32 y, UINT8 col_low, UINT8 col_high);



#endif /*__FRAMEBUFFER_H__*/
