#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t current;	//��ǰ״̬������
	uint8_t last; 		//����һ��
    uint8_t next; 		//����һ��
    uint8_t enter; 		//ȷ��
	uint8_t back; 		//�˳�
    void (*current_operation)(void); //��ǰ״̬Ӧ��ִ�еĲ���
} Menu_table;

typedef struct
{
    uint8_t current;			//������
	uint8_t displaysrt[16]; 	//�ַ���
} Menu2_Display;
 
#endif
