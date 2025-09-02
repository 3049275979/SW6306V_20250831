/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
#include "12864oled_drv.h"
#include "myspi_drv.h"
#include "sw6306.h"
#include "debounce_key.h"
#include "menu.h"
#include "shtc3_drv.h"
#include "ds3231_drv.h" 

static uint8_t ledsta=0;//LED״̬ȫ�ֱ���
static uint8_t timeusart=1;//
static uint8_t forceoff;//�ر������ȫ�ֱ���
static uint8_t keytrig1=0;
static uint8_t key_data=0;
static uint8_t TimeSaveTrig=0;//ʱ�����ñ���״̬��־λ
static uint8_t TimeSaveTrig2=0;//ʱ�����ñ���״̬��־λ
uint16_t cd_sleep;//˯�ߵ���ʱ
static float V1,V2,R2,Temp2;
int16_t Tim1Ch4Pwm=0;
static pid Temp1_pid = 
{
	
	35,	//Ŀ��ֵ
	0,		//ʵ��ֵ(����ֵ)
	0,		//ƫ��ֵ(��ǰƫ��)
	0,		//�ϴ�ƫ��ֵ
	0,		//����ֵ
	2.0,	//������
	0.1,	//������
	1.0,	//΢����
	0,		//���ֵ
};
static uint8_t ch[20];
static uint8_t time=0;
static uint8_t  func_index = 0;	//�������ʱ���ڳ��������ֵ
static void (*current_operation_index)(void);
static DateTime Getds3231Time,Setds3231Time;
static Menu_table  table[100]=
{
    {0,1,1,2,0,(&Home1)},//һ���˵�����ʼ�˵�����ҳ�棩 ����������һ��������һ����ȷ�����˳�
	{1,0,0,2,0,(&Home2)},//һ���˵�����ʼ�˵�����ҳ�棩 ����������һ��������һ����ȷ�����˳�
	
	{2,3,6,7,0,(&Home1_2)},		//�����˵���2�� LED����
	{3,4,2,8,0,(&Home1_2)},		//�����˵���3��ʱ������
	{4,5,3,4,0,(&Home1_2)},		//�����˵���4��NDP2450���翪��
	{5,6,4,15,0,(&Home1_2)},		//�����˵���5��PA8����PWM
	{6,2,5,16,0,(&Home1_2)},		//�����˵���6���¿��趨ֵ
//	
	{7,7,7,7,2,(&MCU1LEDSet)},					//�����˵���7��LED��������
	{8,8,8,9,3,(&Ds3231SetHome)},				//�����˵���8��ʱ�����ã���
	{9,9,9,10,3,(&Ds3231SetHome)},				//�����˵���9��ʱ�����ã���
	{10,10,10,11,3,(&Ds3231SetHome)},	//�����˵���10��ʱ�����ã���
	{11,11,11,12,3,(&Ds3231SetHome)},			//�����˵���11��ʱ�����ã�ʱ
	{12,12,12,13,3,(&Ds3231SetHome)},		//�����˵���12��ʱ�����ã���
	{13,13,13,14,3,(&Ds3231SetHome)},		//�����˵���13��ʱ�����ã���
	{14,14,14,8,3,(&Ds3231SetHome)},		//�����˵���14��ʱ�����ã���
	{15,15,15,15,5,(&ElectricArc)},				//�����˵���15��PA8����PWM
	{16,16,16,16,6,(&SetTempControl)},				//�����˵���16���¿��趨ֵ
};

extern uint8_t inttrig,keytrig;//bsp_exti.c�ļ������ָʾ����
static uint16_t deattach_delay;

THRD_DECLARE(thread_app)
{
    THRD_BEGIN;
    THRD_UNTIL(SW6306_Init());
    THRD_UNTIL(SW6306_IextDirSet(1));//�ⲿϵͳ(LED)һ���ŵ�
    while(1)
    {
		if(timeusart>=10)
		{
			timeusart=10;
		V1 = (float)3.3/4096*BSP_AdcConvert(LL_ADC_CHANNEL_4);//ADCͨ��4��SW6306V��·���¶�
		V2 = 3.32-V1;
		R2 = 100000.0f*(V1/V2);
		Temp1_pid.Current = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//���������¶�ֵ
		
		V1 = (float)3.3/4096*BSP_AdcConvert(LL_ADC_CHANNEL_5);//ADCͨ��5������
		V2 = 3.32-V1;
		R2 = 100000.0f*(V1/V2);
		Temp2 = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//���������¶�ֵ
		
		Temp1_pid.Error =  SW6306_ReadTCHIP()-Temp1_pid.Target ;//ʵ��ֵ- Ŀ��ֵ 
		Temp1_pid.Error=Temp1_pid.Error>50?50:(Temp1_pid.Error<0.001f?0.001f:Temp1_pid.Error);//�޷����ʵ��ֵ
		Temp1_pid.Out=Temp1_pid.Error*10+800;//���ֵ��10�ӻ���ֵ800
		Temp1_pid.Out=Temp1_pid.Out>1000?1000:(Temp1_pid.Out<100?100:Temp1_pid.Out);//�޷����ʵ��ֵ
		TIM1->CCR2=(uint16_t)Temp1_pid.Out;//��ʱ��1ͨ��2��ɢ�ȷ���
		if((TimeSaveTrig==1)&&((func_index==8)||(func_index==9)||(func_index==10)||(func_index==11)
					||(func_index==12)||(func_index==13)||(func_index==14))){
			TimeSaveTrig=0;
			TimeSaveTrig2=1;
			DS3231_setDate(Setds3231Time.year%100,Setds3231Time.month,Setds3231Time.dayofmonth,Setds3231Time.dayOfWeek);
			DS3231_setTime(Setds3231Time.hour,Setds3231Time.minute,Setds3231Time.second);
		}else{
			DS3231_gettime(&Getds3231Time);
			DS3231_getdate(&Getds3231Time);
		}
		Shtc3_Read_Result();
		
        uprintf("\n���ߵ���ʱ:%d0ms",cd_sleep);                
        
        THRD_UNTIL(SW6306_ADCLoad());
        THRD_UNTIL(SW6306_PortStatusLoad());
        THRD_UNTIL(SW6306_StatusLoad());
        THRD_UNTIL(SW6306_PowerLoad());
        THRD_UNTIL(SW6306_CapacityLoad());
        
        if(SW6306_IsInitialized()==0)//���SW6306�Ƿ��ѳ�ʼ����
        {
            uprintf("\n���³�ʼ�� SW6306V..."); 
            THRD_UNTIL(SW6306_Init());
        }
        
        //A�ڷǿ�״̬�ҳ���л�BUS������С������һʱ��ʱ����A�ڰγ��¼����Ա�֤�����������ʱ�Ŀ��
        if(SW6306_ReadIBUS()<IBUS_NOLOAD&&(SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) deattach_delay++;
        else if((SW6306_IsCharging()||SW6306_IsPortC1ON()||SW6306_IsPortC2ON())&&(SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) deattach_delay = A_DEATTACH_DELAY;
        else deattach_delay = 0;
        //���״̬������״̬��BUS��BAT�����㹻��ʱˢ��˯�ߵ���ʱ
        if((SW6306_ReadIBAT()>IBAT_NOLOAD)||(SW6306_ReadIBUS()>IBUS_NOLOAD)||SW6306_IsPortC1ON()||SW6306_IsPortC2ON()||SW6306_IsPortA1ON()||SW6306_IsPortA2ON()) cd_sleep = SLEEP_DELAY;

        //��ŵ�״̬��ʾ
        if(SW6306_IsErrorinCharging()) uprintf("\n��緢������");
        if(SW6306_IsErrorinDischarging()) uprintf("\n�ŵ緢������");
        if(SW6306_IsCharging()) uprintf("\n�ڳ��");
        if(SW6306_IsDischarging()) uprintf("\n���ڷŵ�");
        if(SW6306_IsFullCharged()) uprintf("\n���ڳ��");
        //�˿�״̬��ʾ
        if(SW6306_IsPortC1ON()) uprintf("\nC1 �˿�������");
        if(SW6306_IsPortC2ON()) uprintf("\nC2 �˿�������");
        if(SW6306_IsPortA1ON()) uprintf("\nA1 �˿�������");
        if(SW6306_IsPortA2ON()) uprintf("\nA2 �˿�������");
		uprintf("\n���ؼ��������:%f",SW6306_ReadMaxGuageCap());
		uprintf("\n���ؼƵ�ǰ����%f",SW6306_ReadPresentGuageCap());
		if(SW6306_IsQCStatVQC()) uprintf("\n���ڿ���ѹ");
		if(SW6306_IsQCStatPQC()) uprintf("�����ڿ��Э��");
		if(SW6306_IsQCStatUFCS()) uprintf("��UFCSЭ��");
		if(SW6306_IsQCStatAFC()) uprintf("��AFCЭ��");
		if(SW6306_IsQCStatSFCP()) uprintf("��SFCPЭ��");
		if(SW6306_IsQCStatSVOOC()) uprintf("��Super VOOCЭ��");
		if(SW6306_IsQCStatVOOC4()) uprintf("��VOOC 4.0Э��");
		if(SW6306_IsQCStatVOOC1()) uprintf("��VOOC 1.0Э��");
		if(SW6306_IsQCStatPE20()) uprintf("��MTK PE2.0Э��");
		if(SW6306_IsQCStatPE11()) uprintf("��MTK PE1.1Э��");
		if(SW6306_IsQCStatPDPPS()) uprintf("��PD PPS��ѹ");
		if(SW6306_IsQCStatPDFIX()) uprintf("��PD FIX��ѹ");
		if(SW6306_IsQCStatSCP()) uprintf("��SCPЭ��");
		if(SW6306_IsQCStatFCP()) uprintf("��FCPЭ��");
		if(SW6306_IsQCStatQC3P()) uprintf("��QC3+Э��");
		if(SW6306_IsQCStatQC3()) uprintf("��QC3Э��");
		if(SW6306_IsQCStatQC2()) uprintf("��QC2Э��");
		if(SW6306_IsQCStatNONE()) uprintf("\n��Э��");
        //����������ʾ
        uprintf("\n����:%d%%,LED����:%d",SW6306_ReadCapacity(),TIM1->CCR4);//��ʱ��1ͨ��4��LED����
        uprintf("\n�ӿڵ�ѹ:%dmV,����ת��%d",SW6306_ReadVBUS(),TIM1->CCR2);//��ʱ��1ͨ��2��ɢ�ȷ��� 
        uprintf("\n�ӿڵ���:%dmA,%d",SW6306_ReadIBUS(),LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_11));
        uprintf("\n��ص�ѹ:%dmV",SW6306_ReadVBAT());
        uprintf("\n��ص���:%dmA,SHTC3�¶�:%.02f��C,SHTC3ʪ��:%.02f%%",SW6306_ReadIBAT(),GetShtc3Data(0),GetShtc3Data(1));
        uprintf("\nSW6306V����NTC�¶�:%d��C,����1:%.02f��C",SW6306_ReadTNTC(),Temp1_pid.Current);//ADCͨ��4��SW6306V��·���¶�
        uprintf("\nSW6306V�ں��¶�:%.2f��C,����2:%.02f��C",SW6306_ReadTCHIP(),Temp2);//ADCͨ��5������
		uprintf("\n%04d_%02d_%02d-%02d:%02d:%02d.%d\n",Getds3231Time.year,Getds3231Time.month,Getds3231Time.dayofmonth,
					Getds3231Time.hour,Getds3231Time.minute,Getds3231Time.second,Getds3231Time.dayOfWeek);
		if(SW6306_ReadTCHIP()>180.0f||SW6306_ReadTNTC()>180.0f||
			SW6306_ReadTCHIP()<(-75.0f)||SW6306_ReadTNTC()<(-75.0f))
		{
			uprintf("\n���³�ʼ�� SW6306V..."); 
			THRD_UNTIL(SW6306_Init());
		}else{
			if(SW6306_ReadTCHIP()<=(Temp1_pid.Target-2)){
				LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12Vʧ��
			}else if(SW6306_ReadTCHIP()>=(Temp1_pid.Target+2)){
				LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_12);//12Vʹ��
			}
		}
        //A�ڶϿ�������ִ��
        if(deattach_delay >= A_DEATTACH_DELAY)
        {
            THRD_UNTIL(SW6306_PortA1Remove());
            THRD_UNTIL(SW6306_PortA2Remove());
            deattach_delay = 0;
        }
        //LED�ĵ�������
        if(ledsta)
        {
//            cd_sleep = SLEEP_DELAY;//ˢ��˯�ߵ���ʱ            
            if(!(SW6306_IsPortC1ON()||SW6306_IsPortC2ON()||SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) THRD_UNTIL(SW6306_Click());//û�пڴ�ʱ���������Գ�����ʾ����
            THRD_UNTIL(SW6306_IextEnSet(1));//ʹ�ܵ�������
            THRD_UNTIL(SW6306_IextSet(SW6306_ReadIBAT() - (uint32_t)SW6306_ReadIBUS()*SW6306_ReadVBUS()/SW6306_ReadVBAT()));//�������
        }
        else THRD_UNTIL(SW6306_IextEnSet(0));//��ֹ��������
	}
        THRD_DELAY(REFRESH_DELAY);
    }
    THRD_END;
}

THRD_DECLARE(thread_echo)
{
    uint8_t buf[32];
    uint8_t num;
    THRD_BEGIN;
    while(1)
    {
        THRD_UNTIL(USART_getvalidnum());
        num = USART_getvalidnum();
        USART_bufread(buf,num);
        USART_bufsend(buf,num);
    }
    THRD_END;
}

THRD_DECLARE(thread_key)
{
    static uint8_t step,cnt;//˫����ʱ�ñ���
    static uint8_t holding,leddir,ledind;//������ס��LED�Ƿ�򿪡����ⷽ����LED����
    static uint16_t cd_reset;
    THRD_BEGIN;
    while(1)
    {
		timeusart++; 
        
        Key_Scan();// ����ɨ�裨ÿ10ms����һ�Σ�
		key_data=Check_Key_Events();
		switch(key_data)
		{
			case 1://����1�̰�//ȷ��
				func_index=table[func_index].enter;	//ȷ��
				uprintf("��ֵ:%d\n",key_data);			
			break;
			case 2://����2�̰�//��//����һ��
				func_index=table[func_index].last;	//����һ��
				if(func_index==7)
				{
					Tim1Ch4Pwm++;
					if(Tim1Ch4Pwm>150)
					{
						Tim1Ch4Pwm=0;
					}
				}else if(func_index==8){//��
					Setds3231Time.year++;
					if(Setds3231Time.year>2099){
						Setds3231Time.year=2025;
					}
				}else if(func_index==9){//��
					Setds3231Time.month++;
					if(Setds3231Time.month>12){
						Setds3231Time.month=1;
					}
				}else if(func_index==10){//��
					Setds3231Time.dayofmonth++;
					if(Setds3231Time.dayofmonth>31){
						Setds3231Time.dayofmonth=1;
					}
				}else if(func_index==11){//ʱ
					Setds3231Time.hour++;
					if(Setds3231Time.hour>23){
						Setds3231Time.hour=0;
					}
				}else if(func_index==12){//��
					Setds3231Time.minute++;
					if(Setds3231Time.minute>59){
						Setds3231Time.minute=0;
					}
				}else if(func_index==13){//��
					Setds3231Time.second++;
					if(Setds3231Time.second>59){
						Setds3231Time.second=0;
					}
				}else if(func_index==14){//��
					Setds3231Time.dayOfWeek++;
					if(Setds3231Time.dayOfWeek>7){
						Setds3231Time.dayOfWeek=1;
					}
				}else if(func_index==15){//PA8PWM
					TIM1->CCR1++;
					if(TIM1->CCR1>=1000){
						TIM1->CCR1=0;
					}
				}else if(func_index==16){//�¿��趨ֵ
					Temp1_pid.Target++;
					if(Temp1_pid.Target>=70){
						Temp1_pid.Target=0;
					}
				}
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 3://����3�̰�//��//����һ��
				func_index=table[func_index].next;	//����һ��
				if(func_index==7)
				{
					Tim1Ch4Pwm--;
					if(Tim1Ch4Pwm<0)
					{
						Tim1Ch4Pwm=150;
					}
				}else if(func_index==8){//��
					Setds3231Time.year--;
					if(Setds3231Time.year<2025){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//��
					Setds3231Time.month--;
					if(Setds3231Time.month==0){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//��
					Setds3231Time.dayofmonth--;
					if(Setds3231Time.dayofmonth==0){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//ʱ
					if(Setds3231Time.hour==0){
						Setds3231Time.hour=24;
					}
					Setds3231Time.hour--;
				}else if(func_index==12){//��
					if(Setds3231Time.minute==0){
						Setds3231Time.minute=60;
					}
					Setds3231Time.minute--;
				}else if(func_index==13){//��
					if(Setds3231Time.second==0){
						Setds3231Time.second=60;
					}
					Setds3231Time.second--;
				}else if(func_index==14){//��
					Setds3231Time.dayOfWeek--;
					if(Setds3231Time.dayOfWeek==0){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					if(TIM1->CCR1==0){
						TIM1->CCR1=1000;
					}
					TIM1->CCR1--;
				}else if(func_index==16){//�¿��趨ֵ
					Temp1_pid.Target--;
					if(Temp1_pid.Target<=-70){
						Temp1_pid.Target=70;
					}
				}
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 4://����4�̰�//�˳�
				func_index=table[func_index].back;	//�˳�
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 5://����1����
				uprintf("��ֵ:%d\n",key_data);			
			break;
			case 6://����2����//һֱ��
				if(func_index==7)
				{
					Tim1Ch4Pwm++;
					if(Tim1Ch4Pwm>150)
					{
						Tim1Ch4Pwm=150;
					}
				}else if(func_index==8){//��
					Setds3231Time.year++;
					if(Setds3231Time.year>2099){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//��
					Setds3231Time.month++;
					if(Setds3231Time.month>12){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//��
					Setds3231Time.dayofmonth++;
					if(Setds3231Time.dayofmonth>31){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//ʱ
					Setds3231Time.hour++;
					if(Setds3231Time.hour>23){
						Setds3231Time.hour=23;
					}
				}else if(func_index==12){//��
					Setds3231Time.minute++;
					if(Setds3231Time.minute>59){
						Setds3231Time.minute=59;
					}
				}else if(func_index==13){//��
					Setds3231Time.second++;
					if(Setds3231Time.second>59){
						Setds3231Time.second=59;
					}
				}else if(func_index==14){//��
					Setds3231Time.dayOfWeek++;
					if(Setds3231Time.dayOfWeek>7){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					TIM1->CCR1++;
					if(TIM1->CCR1>=1000){
						TIM1->CCR1=0;
					}
				}else if(func_index==16){//�¿��趨ֵ
					Temp1_pid.Target++;
					if(Temp1_pid.Target>=70){
						Temp1_pid.Target=0;
					}
				}
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 7://����3����//һֱ��
				if(func_index==7)
				{
					Tim1Ch4Pwm--;
					if(Tim1Ch4Pwm<=0)
					{
						Tim1Ch4Pwm=0;
					}
				}else if(func_index==8){//��
					Setds3231Time.year--;
					if(Setds3231Time.year<2025){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//��
					Setds3231Time.month--;
					if(Setds3231Time.month==0){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//��
					Setds3231Time.dayofmonth--;
					if(Setds3231Time.dayofmonth==0){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//ʱ
					if(Setds3231Time.hour==0){
						Setds3231Time.hour=24;
					}
					Setds3231Time.hour--;
				}else if(func_index==12){//��
					if(Setds3231Time.minute==0){
						Setds3231Time.minute=60;
					}
					Setds3231Time.minute--;
				}else if(func_index==13){//��
					if(Setds3231Time.second==0){
						Setds3231Time.second=60;
					}
					Setds3231Time.second--;
				}else if(func_index==14){//��
					Setds3231Time.dayOfWeek--;
					if(Setds3231Time.dayOfWeek==0){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					if(TIM1->CCR1==0){
						TIM1->CCR1=1000;
					}
					TIM1->CCR1--;
				}else if(func_index==16){//�¿��趨ֵ
					Temp1_pid.Target--;
					if(Temp1_pid.Target<=-70){
						Temp1_pid.Target=70;
					}
				}
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 8://����4����
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 9://����1�����ɿ�
				if((func_index==8)||(func_index==9)||(func_index==10)||(func_index==11)
					||(func_index==12)||(func_index==13)||(func_index==14))
				{
					TimeSaveTrig=1;
				}
				TIM1->CCR1=0;
				uprintf("��ֵ:%d\n",key_data);			
			break;
			case 10://����2�����ɿ�
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 11://����3�����ɿ�
				uprintf("��ֵ:%d\n",key_data);
			break;
			case 12://����4�����ɿ�
				uprintf("��ֵ:%d\n",key_data);
			break;
			default: 
			break;
		}
		
        if((SW6306_IsBatteryDepleted()||SW6306_IsOverHeated()))//�͵�ѹ
        {
            uprintf("�͵�ѹ!!!\n");
            ledsta = 0;
        }
		if(SW6306_IsOverHeated())//����
		{
            uprintf("����!!!\n");
            ledsta = 0;
		}
		if((func_index!=0)&&(func_index!=1))
		{
			cd_sleep = SLEEP_DELAY;//ˢ��˯�ߵ���ʱ
		}else
		{
			cd_sleep-=2;
		}
		TIM1->CCR4=(uint16_t)Tim1Ch4Pwm;
		
        THRD_DELAY(1);
    }
    THRD_END;
}

THRD_DECLARE(thread_trig)
{
    THRD_BEGIN;
    while(1)
    {
        if(inttrig)
        {
            uprintf("\nSW6306V�ж��¼�����!\n");
            THRD_UNTIL(SW6306_StatusLoad());
            inttrig = 0;
        }
        if(keytrig)
        {
            uprintf("\n�����ж��¼�����!\n");
			if(keytrig1==1)
			{
				keytrig1=0;
				THRD_UNTIL(SW6306_ByteWrite(SW6306_CTRG_PORTEVT, SW6306_PORTEVT_C1_INS|SW6306_PORTEVT_C2_INS|SW6306_PORTEVT_A1_INS));
				THRD_UNTIL(SW6306_Click());
			}
            keytrig = 0;
        }
//        if(forceoff)
//        {
//            forceoff = 0;
//            THRD_UNTIL(SW6306_ForceOff());
//        }
        THRD_YIELD;
    }
    THRD_END;
}

THRD_DECLARE(thread_lcdprintf)
{
	THRD_BEGIN;
	while(1)
    {
		time++;
		if(time>=100)time=0;
		OLED_Clear(0);//���OLED����
		current_operation_index=table[func_index].current_operation;	//ִ�е�ǰ����������Ӧ�Ĺ��ܺ���
		current_operation_index();//ִ�е�ǰ��������
		OLED_Display();//������ʾ
		THRD_DELAY(LCD_DELAY);
	}
	THRD_END;
}

struct pt pt1,pt2,pt3,pt4,pt5;//ProtoThread�����б���

int main(void)
{
    SysInit();
	Key_Init();
	MySpiGpioInit();
	OLED_Init();
	Shtc3Wakeup();
	BSP_AdcConfig();
    cd_sleep = SLEEP_DELAY;//ˢ��˯�ߵ���ʱ
//	Menu_AppInit();
    LL_mDelay(50);//�ȴ�SW6306�ϵ��ȶ�
    
    uprintf("\n\n5S1P 21700 Power Bank");
    uprintf("\nPowered by SW6306 & PY32F002A");
    uprintf("\nTKWTL 2024/08/10\n");
    
    PT_INIT(&pt1);
    PT_INIT(&pt2);
    PT_INIT(&pt3);
    PT_INIT(&pt4);
	PT_INIT(&pt5);
    
    while(1)//��ѭ��
    {
        thread_echo(&pt2);//�������յ�������ֱ�ӷ���
        thread_key(&pt3);//������������ϵͳ״̬
		thread_lcdprintf(&pt5);//LCD��ʾ
        thread_trig(&pt4);//EXTI��Ӧ
        thread_app(&pt1);//SW6306��ز���
        
        //�������˯�߱��������������
        //�����ɿ���˯�ߵ���ʱ���㡢I2Cû�����ڶ�д��������δ���д��ڴ�ӡ
        //��������ʱ�ر��������裺
        //��SW6306�͹���ģʽ���ر�Systick��ʱ���жϣ�����DeepSleep
        if((USART_IsBusy()==0)&&(cd_sleep==0)&&((LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_1))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_2))))
        {
            while(SW6306_LPSet()==0);//while����THRD_DELAY
			LL_TIM_OC_SetCompareCH1(TIM16,0);//LCD��������
			timeusart=0;
//			LL_TIM_OC_SetCompareCH4(TIM1,0);//LED����
			LL_TIM_OC_SetCompareCH2(TIM1,0);//����ת��
			LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12Vʧ��
			LcdGpioDisable();
			Shtc3Hibernate();
            LL_mDelay(1);//��ʱ�ȴ��������
            LL_SYSTICK_DisableIT();
            LL_LPM_EnableDeepSleep();

            __WFI();
            LL_SYSTICK_EnableIT();
            LL_LPM_EnableSleep();
			MySpiGpioInit();
			OLED_Init();
			LL_TIM_OC_SetCompareCH1(TIM16,100);//LCD��������
			keytrig1=1;
			Shtc3Wakeup();
//			LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_12);//12Vʹ��
//			LL_TIM_OC_SetCompareCH4(TIM1,50);//LED����
//			LL_TIM_OC_SetCompareCH2(TIM1,900);//����ת��
            while(SW6306_Unlock()==0);
        }
        else __WFI();//����ǳ��˯��
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
    /* Infinite loop */
    while (1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line)  */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */
void Menu_AppInit(void)
{
	//����DS3231����������
	Setds3231Time.year=2025;
	Setds3231Time.month=8;
	Setds3231Time.dayofmonth=28;
	Setds3231Time.dayOfWeek=4;
	DS3231_setDate(Setds3231Time.year%100,Setds3231Time.month,Setds3231Time.dayofmonth,Setds3231Time.dayOfWeek);
	//����DS3231ʱ����
	Setds3231Time.hour=10;
	Setds3231Time.minute=18;
	Setds3231Time.second=0;
	DS3231_setTime(Setds3231Time.hour,Setds3231Time.minute,Setds3231Time.second);
}

//������1,0��ң��������
void Home1(void)
{
	Setds3231Time=Getds3231Time;
	OLED_Clear(0);//OLED12832����
	sprintf((char*)ch,"%.02fV %.03fA",(float)SW6306_ReadVBAT()/1000.0f,(float)SW6306_ReadIBAT()/1000.0f);
	OLED_Print(0,0,16,ch,1);
	sprintf((char*)ch,"%.02fV %.03fA",(float)SW6306_ReadVBUS()/1000.0f,(float)SW6306_ReadIBUS()/1000.0f);
	OLED_Print(0,16,16,ch,1);
	if(SW6306_IsQCStatUFCS()) sprintf((char*)ch,"%.02fW %d%% UFCS",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatAFC()) sprintf((char*)ch,"%.02fW %d%% AFC",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatSFCP()) sprintf((char*)ch,"%.02fW %d%% SFCP",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatSVOOC()) sprintf((char*)ch,"%.02fW %d%% SVOOC",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatVOOC4()) sprintf((char*)ch,"%.02fW %d%% VOOC4",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatVOOC1()) sprintf((char*)ch,"%.02fW %d%% VOOC1",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatPE20()) sprintf((char*)ch,"%.02fW %d%% PE2.0",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatPE11()) sprintf((char*)ch,"%.02fW %d%% PE1.1",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatPDPPS()) sprintf((char*)ch,"%.02fW %d%% PDPPS",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatPDFIX()) sprintf((char*)ch,"%.02fW %d%% PDFIX",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatSCP()) sprintf((char*)ch,"%.02fW %d%% SCP",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatFCP()) sprintf((char*)ch,"%.02fW %d%% FCP",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatQC3P()) sprintf((char*)ch,"%.02fW %d%% QC3+",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatQC3()) sprintf((char*)ch,"%.02fW %d%% QC3",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatQC2()) sprintf((char*)ch,"%.02fW %d%% QC2",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	if(SW6306_IsQCStatNONE()) sprintf((char*)ch,"%.02fW %d%%",(float)SW6306_ReadVBUS()*SW6306_ReadIBUS()/1000000.0f,SW6306_ReadCapacity());
	OLED_Print(0,32,16,ch,1);
	sprintf((char*)ch,"%.02f�� %d�� %d",SW6306_ReadTCHIP(),SW6306_ReadTNTC(),(time*LCD_DELAY*10)/1000);
	OLED_Print(0,48,16,ch,1);
}
//������2��1��̽�յ�����
void Home2(void)
{
	Setds3231Time=Getds3231Time;
	sprintf((char*)ch,"%04d-%02d-%02d_%02d:%02d:%02d",Getds3231Time.year,Getds3231Time.month,Getds3231Time.dayofmonth,
		Getds3231Time.hour,Getds3231Time.minute,Getds3231Time.second);
	OLED_Print(0,0,16,ch,1);
	sprintf((char*)ch,"%.02f�� %.02f��",Temp1_pid.Current,Temp2);
	OLED_Print(0,16,16,ch,1);
	sprintf((char*)ch,"%.02f�� %.02f%%",GetShtc3Data(0),GetShtc3Data(1));
	OLED_Print(0,32,16,ch,1);
	//�˿�״̬��ʾ
	if(SW6306_IsPortC1ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d C1",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortC2ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d C2",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortA1ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d A1",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortA2ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d A2",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else sprintf((char*)ch,"FAN:%d%% %02d.%d",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	OLED_Print(0,48,16,ch,1);
//	sprintf((char*)ch,"FAN:%d%% %02d.%d",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
//	OLED_Print(0,48,16,ch,1);
}
//�����˵�1��ʾ����
void Home1_2(void)
{
	uint8_t j,k,l;
	int8_t i,n;
	Menu2_Display DisplayBuf[20]={0};
//	Mcu1SaveSign=0;//�����־λ
//	Mcu1ResetSign=0;//�������ñ�־λ
	TimeSaveTrig=0;
	TimeSaveTrig2=0;
	Setds3231Time=Getds3231Time;
	DisplayBuf[0].current=2;//������
	sprintf((char*)DisplayBuf[0].displaysrt,"LED���� %d%%",TIM1->CCR4/10);//������
	
	DisplayBuf[1].current=3;//������
	sprintf((char*)DisplayBuf[1].displaysrt,"ʱ������");//������
	
	DisplayBuf[2].current=4;//������
	if(LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_12))
	{
		sprintf((char*)DisplayBuf[2].displaysrt,"12V���� ��");//������
	}else
	{
		sprintf((char*)DisplayBuf[2].displaysrt,"12V���� ��");//������
	}
	
	DisplayBuf[3].current=5;//������
	sprintf((char*)DisplayBuf[3].displaysrt,"PA8����PWM");//������
	
	DisplayBuf[4].current=6;//������
	sprintf((char*)DisplayBuf[4].displaysrt,"�¿��趨ֵ%.0f",Temp1_pid.Target);//������
	
	DisplayBuf[5].current=16;//������
	sprintf((char*)DisplayBuf[5].displaysrt,"������...");//������
	
	k=20;//���鳤��
	for(j=0;j<k;j++)//����˵��б����
	{
		if(DisplayBuf[j].current==0)
		{
			break;
		}
	}
	
	for(l=0;l<j;l++)//ȷ������λ��
	{
		if(func_index==DisplayBuf[l].current)
		{
			break;
		}
	}
	n=l-3;
	if(n<0)n=0;
	for(i=0;i<4;i++)//��ʾ
	{
		if(func_index==DisplayBuf[i+n].current)
		{
			OLED_Print(0,i*16,16,(uint8_t*)DisplayBuf[i+n].displaysrt,0);//LCD��ʾ�ַ���������
		}else
		{
			OLED_Print(0,i*16,16,(uint8_t*)DisplayBuf[i+n].displaysrt,1);//LCD��ʾ�ַ�������������
		}
	}
}

//�����˵���7��LED��������
void MCU1LEDSet(void)
{
	OLED_Print(0,0,16,(uint8_t*)"�̰�.����������",1);//LCD��ʾ�ַ���
	OLED_Print(0,16,16,(uint8_t*)"ֵ,�������ֵ��",1);//LCD��ʾ�ַ���
	OLED_Print(0,32,16,(uint8_t*)"��0",1);//LCD��ʾ�ַ���s
	sprintf((char*)ch,"����ֵ:%d",Tim1Ch4Pwm);//ת��Ϊ�ַ���
	OLED_Print(0,48,16,ch,1);//LCD��ʾ�ַ���
}
//�����˵���15��ʱ�����ã��������˳�λ
void Ds3231SetHome(void)
{
	if(func_index==8){
		sprintf((char*)ch,"%04d",Setds3231Time.year);//��
		OLED_Print(0,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%04d",Setds3231Time.year);//��
		OLED_Print(0,0,16,ch,1);
	}
	OLED_Print(32,0,16,(uint8_t*)"-",1);
	if(func_index==9){
		sprintf((char*)ch,"%02d",Setds3231Time.month);//��
		OLED_Print(40,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.month);//��
		OLED_Print(40,0,16,ch,1);
	}
	OLED_Print(56,0,16,(uint8_t*)"-",1);
	if(func_index==10){
		sprintf((char*)ch,"%02d",Setds3231Time.dayofmonth);//��
		OLED_Print(64,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.dayofmonth);//��
		OLED_Print(64,0,16,ch,1);
	}
	OLED_Print(80,0,16,(uint8_t*)"_",1);
	if(func_index==11){
		sprintf((char*)ch,"%02d",Setds3231Time.hour);//ʱ
		OLED_Print(88,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.hour);//ʱ
		OLED_Print(88,0,16,ch,1);
	}
	OLED_Print(104,0,16,(uint8_t*)":",1);
	if(func_index==12){
		sprintf((char*)ch,"%02d",Setds3231Time.minute);//��
		OLED_Print(112,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.minute);//��
		OLED_Print(112,0,16,ch,1);
	}
	OLED_Print(0,16,16,(uint8_t*)":",1);
	if(func_index==13){
		sprintf((char*)ch,"%02d",Setds3231Time.second);//��
		OLED_Print(8,16,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.second);//��
		OLED_Print(8,16,16,ch,1);
	}
	OLED_Print(24,16,16,(uint8_t*)".",1);
	if(func_index==14){
		sprintf((char*)ch,"%d",Setds3231Time.dayOfWeek);//��
		OLED_Print(32,16,16,ch,0);
	}else{
		sprintf((char*)ch,"%d",Setds3231Time.dayOfWeek);//��
		OLED_Print(32,16,16,ch,1);
	}
	OLED_Print(48,16,16,(uint8_t*)"����ȷ�ϼ�",1);
	OLED_Print(0,32,16,(uint8_t*)"1�������ɿ���",1);
	OLED_Print(0,48,16,(uint8_t*)"��",1);
	if(TimeSaveTrig2)
	{
		OLED_Print(80,48,16,(uint8_t*)"�Ա���",1);
	}
}
void ElectricArc(void)//PA8PWMռ�ձ�
{
	sprintf((char*)ch,"PA8PWMռ�ձ�%d",TIM1->CCR1);
	OLED_Print(0,0,16,ch,1);
}
void SetTempControl(void)//�¿��趨ֵ
{
	sprintf((char*)ch,"�趨ֵ%.02f",Temp1_pid.Target);
	OLED_Print(0,0,16,ch,1);
}
