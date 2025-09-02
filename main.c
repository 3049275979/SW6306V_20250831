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

static uint8_t ledsta=0;//LED状态全局变量
static uint8_t timeusart=1;//
static uint8_t forceoff;//关闭输出用全局变量
static uint8_t keytrig1=0;
static uint8_t key_data=0;
static uint8_t TimeSaveTrig=0;//时间设置保存状态标志位
static uint8_t TimeSaveTrig2=0;//时间设置保存状态标志位
uint16_t cd_sleep;//睡眠倒计时
static float V1,V2,R2,Temp2;
int16_t Tim1Ch4Pwm=0;
static pid Temp1_pid = 
{
	
	35,	//目标值
	0,		//实际值(反馈值)
	0,		//偏差值(当前偏差)
	0,		//上次偏差值
	0,		//积分值
	2.0,	//比例项
	0.1,	//积分项
	1.0,	//微分项
	0,		//输出值
};
static uint8_t ch[20];
static uint8_t time=0;
static uint8_t  func_index = 0;	//主程序此时所在程序的索引值
static void (*current_operation_index)(void);
static DateTime Getds3231Time,Setds3231Time;
static Menu_table  table[100]=
{
    {0,1,1,2,0,(&Home1)},//一级菜单，开始菜单（主页面） 索引，向上一个，向下一个，确定，退出
	{1,0,0,2,0,(&Home2)},//一级菜单，开始菜单（主页面） 索引，向上一个，向下一个，确定，退出
	
	{2,3,6,7,0,(&Home1_2)},		//二级菜单，2， LED亮度
	{3,4,2,8,0,(&Home1_2)},		//二级菜单，3，时间设置
	{4,5,3,4,0,(&Home1_2)},		//二级菜单，4，NDP2450供电开关
	{5,6,4,15,0,(&Home1_2)},		//二级菜单，5，PA8引脚PWM
	{6,2,5,16,0,(&Home1_2)},		//二级菜单，6，温控设定值
//	
	{7,7,7,7,2,(&MCU1LEDSet)},					//三级菜单，7，LED亮度设置
	{8,8,8,9,3,(&Ds3231SetHome)},				//三级菜单，8，时间设置，年
	{9,9,9,10,3,(&Ds3231SetHome)},				//三级菜单，9，时间设置，月
	{10,10,10,11,3,(&Ds3231SetHome)},	//三级菜单，10，时间设置，日
	{11,11,11,12,3,(&Ds3231SetHome)},			//三级菜单，11，时间设置，时
	{12,12,12,13,3,(&Ds3231SetHome)},		//三级菜单，12，时间设置，分
	{13,13,13,14,3,(&Ds3231SetHome)},		//三级菜单，13，时间设置，秒
	{14,14,14,8,3,(&Ds3231SetHome)},		//三级菜单，14，时间设置，周
	{15,15,15,15,5,(&ElectricArc)},				//三级菜单，15，PA8引脚PWM
	{16,16,16,16,6,(&SetTempControl)},				//三级菜单，16，温控设定值
};

extern uint8_t inttrig,keytrig;//bsp_exti.c文件定义的指示变量
static uint16_t deattach_delay;

THRD_DECLARE(thread_app)
{
    THRD_BEGIN;
    THRD_UNTIL(SW6306_Init());
    THRD_UNTIL(SW6306_IextDirSet(1));//外部系统(LED)一定放电
    while(1)
    {
		if(timeusart>=10)
		{
			timeusart=10;
		V1 = (float)3.3/4096*BSP_AdcConvert(LL_ADC_CHANNEL_4);//ADC通道4，SW6306V电路板温度
		V2 = 3.32-V1;
		R2 = 100000.0f*(V1/V2);
		Temp1_pid.Current = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//热敏电阻温度值
		
		V1 = (float)3.3/4096*BSP_AdcConvert(LL_ADC_CHANNEL_5);//ADC通道5，箱温
		V2 = 3.32-V1;
		R2 = 100000.0f*(V1/V2);
		Temp2 = 1.0f/(1.0f/(273.15f+25.0f)+log(R2/100000.0f)/3950.0f)-273.15f;//热敏电阻温度值
		
		Temp1_pid.Error =  SW6306_ReadTCHIP()-Temp1_pid.Target ;//实际值- 目标值 
		Temp1_pid.Error=Temp1_pid.Error>50?50:(Temp1_pid.Error<0.001f?0.001f:Temp1_pid.Error);//限幅输出实际值
		Temp1_pid.Out=Temp1_pid.Error*10+800;//误差值乘10加基础值800
		Temp1_pid.Out=Temp1_pid.Out>1000?1000:(Temp1_pid.Out<100?100:Temp1_pid.Out);//限幅输出实际值
		TIM1->CCR2=(uint16_t)Temp1_pid.Out;//定时器1通道2，散热风扇
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
		
        uprintf("\n休眠倒计时:%d0ms",cd_sleep);                
        
        THRD_UNTIL(SW6306_ADCLoad());
        THRD_UNTIL(SW6306_PortStatusLoad());
        THRD_UNTIL(SW6306_StatusLoad());
        THRD_UNTIL(SW6306_PowerLoad());
        THRD_UNTIL(SW6306_CapacityLoad());
        
        if(SW6306_IsInitialized()==0)//检测SW6306是否已初始化过
        {
            uprintf("\n重新初始化 SW6306V..."); 
            THRD_UNTIL(SW6306_Init());
        }
        
        //A口非空状态且充电中或BUS电流过小满足其一时延时发送A口拔出事件，以保证单口输入输出时的快充
        if(SW6306_ReadIBUS()<IBUS_NOLOAD&&(SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) deattach_delay++;
        else if((SW6306_IsCharging()||SW6306_IsPortC1ON()||SW6306_IsPortC2ON())&&(SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) deattach_delay = A_DEATTACH_DELAY;
        else deattach_delay = 0;
        //充电状态、充满状态、BUS与BAT电流足够大时刷新睡眠倒计时
        if((SW6306_ReadIBAT()>IBAT_NOLOAD)||(SW6306_ReadIBUS()>IBUS_NOLOAD)||SW6306_IsPortC1ON()||SW6306_IsPortC2ON()||SW6306_IsPortA1ON()||SW6306_IsPortA2ON()) cd_sleep = SLEEP_DELAY;

        //充放电状态显示
        if(SW6306_IsErrorinCharging()) uprintf("\n充电发生错误");
        if(SW6306_IsErrorinDischarging()) uprintf("\n放电发生错误");
        if(SW6306_IsCharging()) uprintf("\n在充电");
        if(SW6306_IsDischarging()) uprintf("\n正在放电");
        if(SW6306_IsFullCharged()) uprintf("\n正在充电");
        //端口状态显示
        if(SW6306_IsPortC1ON()) uprintf("\nC1 端口以启用");
        if(SW6306_IsPortC2ON()) uprintf("\nC2 端口以启用");
        if(SW6306_IsPortA1ON()) uprintf("\nA1 端口以启用");
        if(SW6306_IsPortA2ON()) uprintf("\nA2 端口以启用");
		uprintf("\n库仑计最大容量:%f",SW6306_ReadMaxGuageCap());
		uprintf("\n库仑计当前容量%f",SW6306_ReadPresentGuageCap());
		if(SW6306_IsQCStatVQC()) uprintf("\n处于快充电压");
		if(SW6306_IsQCStatPQC()) uprintf("，处于快充协议");
		if(SW6306_IsQCStatUFCS()) uprintf("，UFCS协议");
		if(SW6306_IsQCStatAFC()) uprintf("，AFC协议");
		if(SW6306_IsQCStatSFCP()) uprintf("，SFCP协议");
		if(SW6306_IsQCStatSVOOC()) uprintf("，Super VOOC协议");
		if(SW6306_IsQCStatVOOC4()) uprintf("，VOOC 4.0协议");
		if(SW6306_IsQCStatVOOC1()) uprintf("，VOOC 1.0协议");
		if(SW6306_IsQCStatPE20()) uprintf("，MTK PE2.0协议");
		if(SW6306_IsQCStatPE11()) uprintf("，MTK PE1.1协议");
		if(SW6306_IsQCStatPDPPS()) uprintf("，PD PPS电压");
		if(SW6306_IsQCStatPDFIX()) uprintf("，PD FIX电压");
		if(SW6306_IsQCStatSCP()) uprintf("，SCP协议");
		if(SW6306_IsQCStatFCP()) uprintf("，FCP协议");
		if(SW6306_IsQCStatQC3P()) uprintf("，QC3+协议");
		if(SW6306_IsQCStatQC3()) uprintf("，QC3协议");
		if(SW6306_IsQCStatQC2()) uprintf("，QC2协议");
		if(SW6306_IsQCStatNONE()) uprintf("\n无协议");
        //电气数据显示
        uprintf("\n电量:%d%%,LED亮度:%d",SW6306_ReadCapacity(),TIM1->CCR4);//定时器1通道4，LED亮度
        uprintf("\n接口电压:%dmV,风扇转速%d",SW6306_ReadVBUS(),TIM1->CCR2);//定时器1通道2，散热风扇 
        uprintf("\n接口电流:%dmA,%d",SW6306_ReadIBUS(),LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_11));
        uprintf("\n电池电压:%dmV",SW6306_ReadVBAT());
        uprintf("\n电池电流:%dmA,SHTC3温度:%.02f°C,SHTC3湿度:%.02f%%",SW6306_ReadIBAT(),GetShtc3Data(0),GetShtc3Data(1));
        uprintf("\nSW6306V板载NTC温度:%d°C,板温1:%.02f°C",SW6306_ReadTNTC(),Temp1_pid.Current);//ADC通道4，SW6306V电路板温度
        uprintf("\nSW6306V内核温度:%.2f°C,板温2:%.02f°C",SW6306_ReadTCHIP(),Temp2);//ADC通道5，箱温
		uprintf("\n%04d_%02d_%02d-%02d:%02d:%02d.%d\n",Getds3231Time.year,Getds3231Time.month,Getds3231Time.dayofmonth,
					Getds3231Time.hour,Getds3231Time.minute,Getds3231Time.second,Getds3231Time.dayOfWeek);
		if(SW6306_ReadTCHIP()>180.0f||SW6306_ReadTNTC()>180.0f||
			SW6306_ReadTCHIP()<(-75.0f)||SW6306_ReadTNTC()<(-75.0f))
		{
			uprintf("\n重新初始化 SW6306V..."); 
			THRD_UNTIL(SW6306_Init());
		}else{
			if(SW6306_ReadTCHIP()<=(Temp1_pid.Target-2)){
				LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V失能
			}else if(SW6306_ReadTCHIP()>=(Temp1_pid.Target+2)){
				LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V使能
			}
		}
        //A口断开操作的执行
        if(deattach_delay >= A_DEATTACH_DELAY)
        {
            THRD_UNTIL(SW6306_PortA1Remove());
            THRD_UNTIL(SW6306_PortA2Remove());
            deattach_delay = 0;
        }
        //LED耗电量计算
        if(ledsta)
        {
//            cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时            
            if(!(SW6306_IsPortC1ON()||SW6306_IsPortC2ON()||SW6306_IsPortA1ON()||SW6306_IsPortA2ON())) THRD_UNTIL(SW6306_Click());//没有口打开时触发按键以持续显示电量
            THRD_UNTIL(SW6306_IextEnSet(1));//使能电量计算
            THRD_UNTIL(SW6306_IextSet(SW6306_ReadIBAT() - (uint32_t)SW6306_ReadIBUS()*SW6306_ReadVBUS()/SW6306_ReadVBAT()));//输入电流
        }
        else THRD_UNTIL(SW6306_IextEnSet(0));//禁止电量计算
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
    static uint8_t step,cnt;//双击计时用变量
    static uint8_t holding,leddir,ledind;//按键按住、LED是否打开、调光方向与LED亮度
    static uint16_t cd_reset;
    THRD_BEGIN;
    while(1)
    {
		timeusart++; 
        
        Key_Scan();// 按键扫描（每10ms调用一次）
		key_data=Check_Key_Events();
		switch(key_data)
		{
			case 1://按键1短按//确定
				func_index=table[func_index].enter;	//确定
				uprintf("键值:%d\n",key_data);			
			break;
			case 2://按键2短按//加//向上一个
				func_index=table[func_index].last;	//向上一个
				if(func_index==7)
				{
					Tim1Ch4Pwm++;
					if(Tim1Ch4Pwm>150)
					{
						Tim1Ch4Pwm=0;
					}
				}else if(func_index==8){//年
					Setds3231Time.year++;
					if(Setds3231Time.year>2099){
						Setds3231Time.year=2025;
					}
				}else if(func_index==9){//月
					Setds3231Time.month++;
					if(Setds3231Time.month>12){
						Setds3231Time.month=1;
					}
				}else if(func_index==10){//日
					Setds3231Time.dayofmonth++;
					if(Setds3231Time.dayofmonth>31){
						Setds3231Time.dayofmonth=1;
					}
				}else if(func_index==11){//时
					Setds3231Time.hour++;
					if(Setds3231Time.hour>23){
						Setds3231Time.hour=0;
					}
				}else if(func_index==12){//分
					Setds3231Time.minute++;
					if(Setds3231Time.minute>59){
						Setds3231Time.minute=0;
					}
				}else if(func_index==13){//秒
					Setds3231Time.second++;
					if(Setds3231Time.second>59){
						Setds3231Time.second=0;
					}
				}else if(func_index==14){//周
					Setds3231Time.dayOfWeek++;
					if(Setds3231Time.dayOfWeek>7){
						Setds3231Time.dayOfWeek=1;
					}
				}else if(func_index==15){//PA8PWM
					TIM1->CCR1++;
					if(TIM1->CCR1>=1000){
						TIM1->CCR1=0;
					}
				}else if(func_index==16){//温控设定值
					Temp1_pid.Target++;
					if(Temp1_pid.Target>=70){
						Temp1_pid.Target=0;
					}
				}
				uprintf("键值:%d\n",key_data);
			break;
			case 3://按键3短按//减//向下一个
				func_index=table[func_index].next;	//向下一个
				if(func_index==7)
				{
					Tim1Ch4Pwm--;
					if(Tim1Ch4Pwm<0)
					{
						Tim1Ch4Pwm=150;
					}
				}else if(func_index==8){//年
					Setds3231Time.year--;
					if(Setds3231Time.year<2025){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//月
					Setds3231Time.month--;
					if(Setds3231Time.month==0){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//日
					Setds3231Time.dayofmonth--;
					if(Setds3231Time.dayofmonth==0){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//时
					if(Setds3231Time.hour==0){
						Setds3231Time.hour=24;
					}
					Setds3231Time.hour--;
				}else if(func_index==12){//分
					if(Setds3231Time.minute==0){
						Setds3231Time.minute=60;
					}
					Setds3231Time.minute--;
				}else if(func_index==13){//秒
					if(Setds3231Time.second==0){
						Setds3231Time.second=60;
					}
					Setds3231Time.second--;
				}else if(func_index==14){//周
					Setds3231Time.dayOfWeek--;
					if(Setds3231Time.dayOfWeek==0){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					if(TIM1->CCR1==0){
						TIM1->CCR1=1000;
					}
					TIM1->CCR1--;
				}else if(func_index==16){//温控设定值
					Temp1_pid.Target--;
					if(Temp1_pid.Target<=-70){
						Temp1_pid.Target=70;
					}
				}
				uprintf("键值:%d\n",key_data);
			break;
			case 4://按键4短按//退出
				func_index=table[func_index].back;	//退出
				uprintf("键值:%d\n",key_data);
			break;
			case 5://按键1长按
				uprintf("键值:%d\n",key_data);			
			break;
			case 6://按键2长按//一直加
				if(func_index==7)
				{
					Tim1Ch4Pwm++;
					if(Tim1Ch4Pwm>150)
					{
						Tim1Ch4Pwm=150;
					}
				}else if(func_index==8){//年
					Setds3231Time.year++;
					if(Setds3231Time.year>2099){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//月
					Setds3231Time.month++;
					if(Setds3231Time.month>12){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//日
					Setds3231Time.dayofmonth++;
					if(Setds3231Time.dayofmonth>31){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//时
					Setds3231Time.hour++;
					if(Setds3231Time.hour>23){
						Setds3231Time.hour=23;
					}
				}else if(func_index==12){//分
					Setds3231Time.minute++;
					if(Setds3231Time.minute>59){
						Setds3231Time.minute=59;
					}
				}else if(func_index==13){//秒
					Setds3231Time.second++;
					if(Setds3231Time.second>59){
						Setds3231Time.second=59;
					}
				}else if(func_index==14){//周
					Setds3231Time.dayOfWeek++;
					if(Setds3231Time.dayOfWeek>7){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					TIM1->CCR1++;
					if(TIM1->CCR1>=1000){
						TIM1->CCR1=0;
					}
				}else if(func_index==16){//温控设定值
					Temp1_pid.Target++;
					if(Temp1_pid.Target>=70){
						Temp1_pid.Target=0;
					}
				}
				uprintf("键值:%d\n",key_data);
			break;
			case 7://按键3长按//一直减
				if(func_index==7)
				{
					Tim1Ch4Pwm--;
					if(Tim1Ch4Pwm<=0)
					{
						Tim1Ch4Pwm=0;
					}
				}else if(func_index==8){//年
					Setds3231Time.year--;
					if(Setds3231Time.year<2025){
						Setds3231Time.year=2099;
					}
				}else if(func_index==9){//月
					Setds3231Time.month--;
					if(Setds3231Time.month==0){
						Setds3231Time.month=12;
					}
				}else if(func_index==10){//日
					Setds3231Time.dayofmonth--;
					if(Setds3231Time.dayofmonth==0){
						Setds3231Time.dayofmonth=31;
					}
				}else if(func_index==11){//时
					if(Setds3231Time.hour==0){
						Setds3231Time.hour=24;
					}
					Setds3231Time.hour--;
				}else if(func_index==12){//分
					if(Setds3231Time.minute==0){
						Setds3231Time.minute=60;
					}
					Setds3231Time.minute--;
				}else if(func_index==13){//秒
					if(Setds3231Time.second==0){
						Setds3231Time.second=60;
					}
					Setds3231Time.second--;
				}else if(func_index==14){//周
					Setds3231Time.dayOfWeek--;
					if(Setds3231Time.dayOfWeek==0){
						Setds3231Time.dayOfWeek=7;
					}
				}else if(func_index==15){//PA8PWM
					if(TIM1->CCR1==0){
						TIM1->CCR1=1000;
					}
					TIM1->CCR1--;
				}else if(func_index==16){//温控设定值
					Temp1_pid.Target--;
					if(Temp1_pid.Target<=-70){
						Temp1_pid.Target=70;
					}
				}
				uprintf("键值:%d\n",key_data);
			break;
			case 8://按键4长按
				uprintf("键值:%d\n",key_data);
			break;
			case 9://按键1长按松开
				if((func_index==8)||(func_index==9)||(func_index==10)||(func_index==11)
					||(func_index==12)||(func_index==13)||(func_index==14))
				{
					TimeSaveTrig=1;
				}
				TIM1->CCR1=0;
				uprintf("键值:%d\n",key_data);			
			break;
			case 10://按键2长按松开
				uprintf("键值:%d\n",key_data);
			break;
			case 11://按键3长按松开
				uprintf("键值:%d\n",key_data);
			break;
			case 12://按键4长按松开
				uprintf("键值:%d\n",key_data);
			break;
			default: 
			break;
		}
		
        if((SW6306_IsBatteryDepleted()||SW6306_IsOverHeated()))//低电压
        {
            uprintf("低电压!!!\n");
            ledsta = 0;
        }
		if(SW6306_IsOverHeated())//过温
		{
            uprintf("过温!!!\n");
            ledsta = 0;
		}
		if((func_index!=0)&&(func_index!=1))
		{
			cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时
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
            uprintf("\nSW6306V中断事件发生!\n");
            THRD_UNTIL(SW6306_StatusLoad());
            inttrig = 0;
        }
        if(keytrig)
        {
            uprintf("\n按键中断事件发生!\n");
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
		OLED_Clear(0);//清除OLED缓存
		current_operation_index=table[func_index].current_operation;	//执行当前索引号所对应的功能函数
		current_operation_index();//执行当前操作函数
		OLED_Display();//更新显示
		THRD_DELAY(LCD_DELAY);
	}
	THRD_END;
}

struct pt pt1,pt2,pt3,pt4,pt5;//ProtoThread库运行变量

int main(void)
{
    SysInit();
	Key_Init();
	MySpiGpioInit();
	OLED_Init();
	Shtc3Wakeup();
	BSP_AdcConfig();
    cd_sleep = SLEEP_DELAY;//刷新睡眠倒计时
//	Menu_AppInit();
    LL_mDelay(50);//等待SW6306上电稳定
    
    uprintf("\n\n5S1P 21700 Power Bank");
    uprintf("\nPowered by SW6306 & PY32F002A");
    uprintf("\nTKWTL 2024/08/10\n");
    
    PT_INIT(&pt1);
    PT_INIT(&pt2);
    PT_INIT(&pt3);
    PT_INIT(&pt4);
	PT_INIT(&pt5);
    
    while(1)//主循环
    {
        thread_echo(&pt2);//将串口收到的数据直接发回
        thread_key(&pt3);//按键、调光与系统状态
		thread_lcdprintf(&pt5);//LCD显示
        thread_trig(&pt4);//EXTI响应
        thread_app(&pt1);//SW6306相关操作
        
        //进入深度睡眠必须满足的条件：
        //按键松开、睡眠倒计时归零、I2C没有正在读写的任务且未进行串口打印
        //满足条件时关闭所有外设：
        //打开SW6306低功耗模式，关闭Systick定时器中断，允许DeepSleep
        if((USART_IsBusy()==0)&&(cd_sleep==0)&&((LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_3))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_0))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_1))||
			(LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_2))))
        {
            while(SW6306_LPSet()==0);//while代替THRD_DELAY
			LL_TIM_OC_SetCompareCH1(TIM16,0);//LCD背光亮度
			timeusart=0;
//			LL_TIM_OC_SetCompareCH4(TIM1,0);//LED亮度
			LL_TIM_OC_SetCompareCH2(TIM1,0);//风扇转速
			LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V失能
			LcdGpioDisable();
			Shtc3Hibernate();
            LL_mDelay(1);//延时等待操作完成
            LL_SYSTICK_DisableIT();
            LL_LPM_EnableDeepSleep();

            __WFI();
            LL_SYSTICK_EnableIT();
            LL_LPM_EnableSleep();
			MySpiGpioInit();
			OLED_Init();
			LL_TIM_OC_SetCompareCH1(TIM16,100);//LCD背光亮度
			keytrig1=1;
			Shtc3Wakeup();
//			LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_12);//12V使能
//			LL_TIM_OC_SetCompareCH4(TIM1,50);//LED亮度
//			LL_TIM_OC_SetCompareCH2(TIM1,900);//风扇转速
            while(SW6306_Unlock()==0);
        }
        else __WFI();//否则浅度睡眠
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
	//设置DS3231年月日星期
	Setds3231Time.year=2025;
	Setds3231Time.month=8;
	Setds3231Time.dayofmonth=28;
	Setds3231Time.dayOfWeek=4;
	DS3231_setDate(Setds3231Time.year%100,Setds3231Time.month,Setds3231Time.dayofmonth,Setds3231Time.dayOfWeek);
	//设置DS3231时分秒
	Setds3231Time.hour=10;
	Setds3231Time.minute=18;
	Setds3231Time.second=0;
	DS3231_setTime(Setds3231Time.hour,Setds3231Time.minute,Setds3231Time.second);
}

//主界面1,0，遥控器数据
void Home1(void)
{
	Setds3231Time=Getds3231Time;
	OLED_Clear(0);//OLED12832清零
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
	sprintf((char*)ch,"%.02f℃ %d℃ %d",SW6306_ReadTCHIP(),SW6306_ReadTNTC(),(time*LCD_DELAY*10)/1000);
	OLED_Print(0,48,16,ch,1);
}
//主界面2，1，探照灯数据
void Home2(void)
{
	Setds3231Time=Getds3231Time;
	sprintf((char*)ch,"%04d-%02d-%02d_%02d:%02d:%02d",Getds3231Time.year,Getds3231Time.month,Getds3231Time.dayofmonth,
		Getds3231Time.hour,Getds3231Time.minute,Getds3231Time.second);
	OLED_Print(0,0,16,ch,1);
	sprintf((char*)ch,"%.02f℃ %.02f℃",Temp1_pid.Current,Temp2);
	OLED_Print(0,16,16,ch,1);
	sprintf((char*)ch,"%.02f℃ %.02f%%",GetShtc3Data(0),GetShtc3Data(1));
	OLED_Print(0,32,16,ch,1);
	//端口状态显示
	if(SW6306_IsPortC1ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d C1",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortC2ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d C2",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortA1ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d A1",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else if(SW6306_IsPortA2ON()) sprintf((char*)ch,"FAN:%d%% %02d.%d A2",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	else sprintf((char*)ch,"FAN:%d%% %02d.%d",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
	OLED_Print(0,48,16,ch,1);
//	sprintf((char*)ch,"FAN:%d%% %02d.%d",TIM1->CCR2/10,Getds3231Time.second,Getds3231Time.dayOfWeek);
//	OLED_Print(0,48,16,ch,1);
}
//二级菜单1显示内容
void Home1_2(void)
{
	uint8_t j,k,l;
	int8_t i,n;
	Menu2_Display DisplayBuf[20]={0};
//	Mcu1SaveSign=0;//保存标志位
//	Mcu1ResetSign=0;//参数重置标志位
	TimeSaveTrig=0;
	TimeSaveTrig2=0;
	Setds3231Time=Getds3231Time;
	DisplayBuf[0].current=2;//索引号
	sprintf((char*)DisplayBuf[0].displaysrt,"LED亮度 %d%%",TIM1->CCR4/10);//功能名
	
	DisplayBuf[1].current=3;//索引号
	sprintf((char*)DisplayBuf[1].displaysrt,"时间设置");//功能名
	
	DisplayBuf[2].current=4;//索引号
	if(LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_12))
	{
		sprintf((char*)DisplayBuf[2].displaysrt,"12V开关 开");//功能名
	}else
	{
		sprintf((char*)DisplayBuf[2].displaysrt,"12V开关 关");//功能名
	}
	
	DisplayBuf[3].current=5;//索引号
	sprintf((char*)DisplayBuf[3].displaysrt,"PA8引脚PWM");//功能名
	
	DisplayBuf[4].current=6;//索引号
	sprintf((char*)DisplayBuf[4].displaysrt,"温控设定值%.0f",Temp1_pid.Target);//功能名
	
	DisplayBuf[5].current=16;//索引号
	sprintf((char*)DisplayBuf[5].displaysrt,"待设置...");//功能名
	
	k=20;//数组长度
	for(j=0;j<k;j++)//计算菜单列表个数
	{
		if(DisplayBuf[j].current==0)
		{
			break;
		}
	}
	
	for(l=0;l<j;l++)//确定索引位置
	{
		if(func_index==DisplayBuf[l].current)
		{
			break;
		}
	}
	n=l-3;
	if(n<0)n=0;
	for(i=0;i<4;i++)//显示
	{
		if(func_index==DisplayBuf[i+n].current)
		{
			OLED_Print(0,i*16,16,(uint8_t*)DisplayBuf[i+n].displaysrt,0);//LCD显示字符串、反显
		}else
		{
			OLED_Print(0,i*16,16,(uint8_t*)DisplayBuf[i+n].displaysrt,1);//LCD显示字符串、正常显视
		}
	}
}

//三级菜单，7，LED亮度设置
void MCU1LEDSet(void)
{
	OLED_Print(0,0,16,(uint8_t*)"短按.长按加亮度",1);//LCD显示字符串
	OLED_Print(0,16,16,(uint8_t*)"值,到达最大值后",1);//LCD显示字符串
	OLED_Print(0,32,16,(uint8_t*)"置0",1);//LCD显示字符串s
	sprintf((char*)ch,"亮度值:%d",Tim1Ch4Pwm);//转换为字符串
	OLED_Print(0,48,16,ch,1);//LCD显示字符串
}
//三级菜单，15，时间设置，保存与退出位
void Ds3231SetHome(void)
{
	if(func_index==8){
		sprintf((char*)ch,"%04d",Setds3231Time.year);//年
		OLED_Print(0,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%04d",Setds3231Time.year);//年
		OLED_Print(0,0,16,ch,1);
	}
	OLED_Print(32,0,16,(uint8_t*)"-",1);
	if(func_index==9){
		sprintf((char*)ch,"%02d",Setds3231Time.month);//月
		OLED_Print(40,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.month);//月
		OLED_Print(40,0,16,ch,1);
	}
	OLED_Print(56,0,16,(uint8_t*)"-",1);
	if(func_index==10){
		sprintf((char*)ch,"%02d",Setds3231Time.dayofmonth);//日
		OLED_Print(64,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.dayofmonth);//日
		OLED_Print(64,0,16,ch,1);
	}
	OLED_Print(80,0,16,(uint8_t*)"_",1);
	if(func_index==11){
		sprintf((char*)ch,"%02d",Setds3231Time.hour);//时
		OLED_Print(88,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.hour);//时
		OLED_Print(88,0,16,ch,1);
	}
	OLED_Print(104,0,16,(uint8_t*)":",1);
	if(func_index==12){
		sprintf((char*)ch,"%02d",Setds3231Time.minute);//分
		OLED_Print(112,0,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.minute);//分
		OLED_Print(112,0,16,ch,1);
	}
	OLED_Print(0,16,16,(uint8_t*)":",1);
	if(func_index==13){
		sprintf((char*)ch,"%02d",Setds3231Time.second);//秒
		OLED_Print(8,16,16,ch,0);
	}else{
		sprintf((char*)ch,"%02d",Setds3231Time.second);//秒
		OLED_Print(8,16,16,ch,1);
	}
	OLED_Print(24,16,16,(uint8_t*)".",1);
	if(func_index==14){
		sprintf((char*)ch,"%d",Setds3231Time.dayOfWeek);//周
		OLED_Print(32,16,16,ch,0);
	}else{
		sprintf((char*)ch,"%d",Setds3231Time.dayOfWeek);//周
		OLED_Print(32,16,16,ch,1);
	}
	OLED_Print(48,16,16,(uint8_t*)"长按确认键",1);
	OLED_Print(0,32,16,(uint8_t*)"1秒以上松开后保",1);
	OLED_Print(0,48,16,(uint8_t*)"存",1);
	if(TimeSaveTrig2)
	{
		OLED_Print(80,48,16,(uint8_t*)"以保存",1);
	}
}
void ElectricArc(void)//PA8PWM占空比
{
	sprintf((char*)ch,"PA8PWM占空比%d",TIM1->CCR1);
	OLED_Print(0,0,16,ch,1);
}
void SetTempControl(void)//温控设定值
{
	sprintf((char*)ch,"设定值%.02f",Temp1_pid.Target);
	OLED_Print(0,0,16,ch,1);
}
