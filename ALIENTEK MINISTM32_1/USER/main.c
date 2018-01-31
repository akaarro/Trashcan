#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "nrf.h"
#include "dianji.h"
#include "DHT11.h"
extern u8  tdata[4];		//温度湿度
extern u8  TIM2CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//输入捕获值
extern u8  TIM4CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u16	TIM4CH1_CAPTURE_VAL;	//输入捕获值
u8 flag;//是否垃圾溢满标志位
u8 quan=0;//圈数
u8 status;
u8 sbuf_me[4]={1};
u8 i; 
int main(void)
{	
	u32 temp=0,s; 
	YY=1;
	SPI_NRF_Init();//nrf初始化
	init_stepmotor_GPIO();//电机初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	delay_init();	    	 //延时函数初始化	
	uart_init(9600);		 //串口9600	  
	LED_Init();		  	//初始化IO口
	TIM2_Cap_Init(0XFFFF,72-1);		//以1Mhz的频率计数  
  TIM4_Cap_Init(0XFFFF,72-1);		//以1Mhz的频率计数    
  status = NRF_Check();//查看nrf连接状态 
  if(status == SUCCESS)	   
    printf("\r\n连接成功!!\r\n");  
  else	  
    printf("\r\n连接失败！！\r\n");
  while(1)
	{
		if(sbuf_me[2]!=0&&sbuf_me[3]!=0)
		{
			NRF_TX_Mode();//nrf进入发送模式
			status = NRF_Tx_Dat(sbuf_me);	//发送sbuf中的东西
			switch(status)
			{
				case MAX_RT:
				 printf("\r\n overtime! \r\n");
					break;
				case ERROR:
				 printf("\r\n error！ \r\n");
					break;
				case TX_DS:
				 printf("\r\n 发送成功 \r\n");	 		
					break;  								
			}	
		}			
		ReadDHT11();//读取湿度温度
		sbuf_me[3] = tdata[0];//湿度
		sbuf_me[2] = tdata[2];//温度
		GPIO_SetBits(GPIOB,GPIO_Pin_0);	// 启动超声波模块 trig  超声波模块5v供电
		delay_us(20);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
 		delay_ms(10);	 
		if(TIM2CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平  PA.0是tim2ch1  -->echo引脚
		{
			temp=TIM2CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//溢出时间总和
			temp+=TIM2CH1_CAPTURE_VAL;		//得到总的高电平时间
			s=((temp/2)*344)/10000;//s为距离（厘米）

			if(s<10)
			{ 
				printf("s<10!!!\n");
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);//用来查看是否真的距离收集到了 D2 置0
				flag=1;//满了就置1使得下面当有人过来之后不能打开 
        sbuf_me[1] = 1;//sbuf中表示自己已满
			}
			else
			{  		
				printf("s>10!!!\n");
				GPIO_SetBits(GPIOD,GPIO_Pin_2);	//用来查看是否真的距离收集到了  D2 置1
		    flag=0;//没有满置0使得有人的时候能打开
        sbuf_me[1] = 0;//sbuf中表示自己未满
			}		
  		TIM2CH1_CAPTURE_STA=0;		//状态清零	
		}
		GPIO_SetBits(GPIOB,GPIO_Pin_1);	// 启动超声波模块 trig  超声波模块5v供电
		delay_us(20);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_1);	
 		delay_ms(10);	
		if(TIM4CH1_CAPTURE_STA&0X80)//成功捕获到了一次高电平  Pb.6是tim4ch1  -->echo引脚
		{
			temp=TIM4CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//溢出时间总和
			temp+=TIM4CH1_CAPTURE_VAL;		//得到总的高电平时间
			s=((temp/2)*344)/10000;//s为距离（厘米）
					printf("%d!!!\n",s); 	
			if(s<50)//有人走过来
			{ 
				printf(" someone is coming！！\n");
				if(flag==0)//如果垃圾没有满
					motor_ccw(100);//电机正转
			//	flag=1;
				delay_ms(1000);
				YY=0;//语音开启
				delay_ms(100);
				YY=1;//语音关闭
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
			//flag=0;
				if(flag==0)//如果垃圾没有满
					motor_cw(200);//电机反转
			}
			TIM4CH1_CAPTURE_STA=0;		//状态清零	
		}
	
 }
}
