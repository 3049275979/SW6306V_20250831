#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__

#define BAUD_RATE           115200                                              //����ͨ�Ų�����

#define T_SYSTICK           10                                                  //Systick���ʱ�䣨��λ��ms��

#define IBUS_NOLOAD         50                                                  //BUS�����ж���������λ��mA��
#define IBAT_NOLOAD         50                                                  //BAT�����ж���������λ��mA��

#define TMAX_DOUBLECLICK    10                                                  //˫����ʱʱ�䣨��λ��x10ms��
#define REFRESH_DELAY       100                                                 //ˢ��ʱ�䣨��λ��x10ms��
#define LCD_DELAY       	30                                                 //ˢ��ʱ�䣨��λ��x10ms��
#define A_DEATTACH_DELAY    4                                                   //��������ʱA�ڶ����ȴ�����
#define SLEEP_DELAY         1000                                                 //���¼�������ʱ�䣨��λ��x10ms��
#define T_ULTRA_LONGPRESS   1000                                                //��������Ӧʱ�䣨��λ��x10ms��

#define LED_PWM_TOP         1000                                                //LED PWM�������ֵ/����
#define LED_PWM_MIN         1                                                   //LED����ʱ����СPWMֵ����������С����
#define LED_PWM_MAX         500                                                 //LED����ʱ�����PWMֵ���������������

#endif