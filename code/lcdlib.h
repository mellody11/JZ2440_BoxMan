/*
 * FILE: lcdlib.h
 * TFT LCD的测试函数接口
 */

#ifndef __LCDLIB_H__
#define __LCDLIB_H__

#include <stdio.h>
#include "lcddrv.h"
#include "framebuffer.h"



void ShowPicture(UINT32 x, UINT32 y, UINT32 x_size, UINT32 y_size, unsigned char* pic);
void Lcd_Led(void);
void Lcd_game(void);


#endif /*__LCDLIB_H__*/
