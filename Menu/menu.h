#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t current;	//当前状态索引号
	uint8_t last; 		//向上一个
    uint8_t next; 		//向下一个
    uint8_t enter; 		//确定
	uint8_t back; 		//退出
    void (*current_operation)(void); //当前状态应该执行的操作
} Menu_table;

typedef struct
{
    uint8_t current;			//索引号
	uint8_t displaysrt[16]; 	//字符串
} Menu2_Display;
 
#endif
