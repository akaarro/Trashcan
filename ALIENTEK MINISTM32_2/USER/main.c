#include "stm32f10x.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "timer.h"
#include "usart.h"
#include "nrf.h"
#include "dianji.h"
#include "DHT11.h"
extern u8  tdata[4];		//�¶�ʪ��
extern u8  TIM2CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM2CH1_CAPTURE_VAL;	//���벶��ֵ
extern u8  TIM4CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u16	TIM4CH1_CAPTURE_VAL;	//���벶��ֵ
u8 flag;//�Ƿ�����������־λ
u8 quan=0;//Ȧ��
u8 status;
u8 sbuf_last[4];
u8 i; 
int main(void)
{	
	/*���͸���λ���ĸ�ʽ*/
	u8 s1[] = {14,'N','o','.','0','T','r','a','s','h','c','a','n',':','\n'};//��������Ͱ
	u8 s2[] = {16,'G','a','r','b','a','g','e',' ','i','s',' ','f','u','l','l','\n'};//�Ƿ�����
	u8 s3[] = {20,'G','a','r','b','a','g','e',' ','i','s',' ','n','o','t',' ','f','u','l','l','\n'};
	u8 s4[] = {17,'T','e','m','p','e','r','a','t','u','r','e',':',' ',' ',' ','C','\n'};//�¶�
	u8 s5[] = {17,'H','u','m','i','d','i','t','y',':',' ',' ',' ',' ',' ',' ','%','\n'};//ʪ��
	u32 temp=0,s; 
	u8 sbuf_me1[4];
	YY=1;
	sbuf_me1[0] = 2;
	SPI_NRF_Init();//nrf��ʼ��
	init_stepmotor_GPIO();//�����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	delay_init();	    	 //��ʱ������ʼ��	
	uart_init(9600);		 //����9600	  
	LED_Init();		  	//��ʼ��IO��
 	TIM2_Cap_Init(0XFFFF,72-1);		//��1Mhz��Ƶ�ʼ��� 
  TIM4_Cap_Init(0XFFFF,72-1);		//��1Mhz��Ƶ�ʼ��� 
  status = NRF_Check(); //�鿴nrf����״̬ 
  if(status == SUCCESS)	   
    printf("\r\n���ӳɹ�!!\r\n");  
  else	  
    printf("\r\n����ʧ�ܣ���\r\n");
  while(1)
	{
		NRF_RX_Mode();//nrf�������ģʽ

		status = NRF_Rx_Dat(sbuf_last);	//�������ݵ�sbuf
		if(status == RX_DR)//���յ���ǰһ��������
		{
		  i=0;
			/*��һ��������Ͱ*/
			s1[4] = sbuf_last[i]+'0';
			s4[14] = sbuf_last[i+2]/10+'0' ;
			s4[15] = sbuf_last[i+2]%10+'0' ;
			s5[14] = sbuf_last[i+3]/10+'0' ;
			s5[15] = sbuf_last[i+3]%10+'0' ;
			NRF_TX_Mode();//nrf���뷢��ģʽ
			status = NRF_Tx_Dat(s1);	//���͸���λ��
	//		delay_ms(100);
			if(sbuf_last[i+1]==1)
				status = NRF_Tx_Dat(s2);	//���͸���λ��
			else if(sbuf_last[i+1]==0)
				status = NRF_Tx_Dat(s3);	//���͸���λ��
		//	delay_ms(100);
			status = NRF_Tx_Dat(s4);	//���͸���λ��
			delay_ms(40);
			status = NRF_Tx_Dat(s5);	//���͸���λ��
		//	delay_ms(100);
		}
		else
			printf("\r\n δ�յ���һ������Ϣ \r\n");
			/*�Լ�������Ͱ*/
			i=0;
      NRF_TX_Mode();//nrf���뷢��ģʽ
		//	printf("%d\n",sbuf_me1[i]);
			s1[4] = sbuf_me1[i]+'0';
			s4[14] = sbuf_me1[i+2]/10+'0' ;
			s4[15] = sbuf_me1[i+2]%10+'0' ;
			s5[14] = sbuf_me1[i+3]/10+'0' ;
			s5[15] = sbuf_me1[i+3]%10+'0' ;
			status = NRF_Tx_Dat(s1);	//���͸���λ��
		//	delay_ms(50);
			if(sbuf_me1[i+1]==1)
				status = NRF_Tx_Dat(s2);	//���͸���λ��
			else if(sbuf_me1[i+1]==0)
				status = NRF_Tx_Dat(s3);	//���͸���λ��
		//	delay_ms(20);
			status = NRF_Tx_Dat(s4);	//���͸���λ��
			delay_ms(50);
			status = NRF_Tx_Dat(s5);	//���͸���λ��
			switch(status)//����Ƿ��ͳɹ�
			{
				case MAX_RT:
				 printf("\r\n overtime! \r\n");
					break;
				case ERROR:
				 printf("\r\n error�� \r\n");
					break;
				case TX_DS:
				 printf("\r\n ���ͳɹ� \r\n");	 		
					break;  								
			}	
		

		ReadDHT11();//��ȡʪ���¶�
		sbuf_me1[3] = tdata[0];//ʪ��
		sbuf_me1[2] = tdata[2];//�¶�

		GPIO_SetBits(GPIOB,GPIO_Pin_0);	// ����������ģ�� trig  ������ģ��5v����
		delay_us(20);
	  GPIO_ResetBits(GPIOB,GPIO_Pin_0);	
 		delay_ms(10);	 
		if(TIM2CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ  PA.0��tim2ch1  -->echo����
		{
			temp=TIM2CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//���ʱ���ܺ�
			temp+=TIM2CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			s=((temp/2)*344)/10000;//sΪ���루���ף�
			if(s<10)
			{ 
				printf("s<10!!!\n");
				GPIO_ResetBits(GPIOD,GPIO_Pin_2);//�����鿴�Ƿ���ľ����ռ����� D2 ��0
				flag=1;//���˾���1ʹ�����浱���˹���֮���ܴ�
        sbuf_me1[1] = 1;//sbuf�б�ʾ�Լ�����
			}
			else
			{  		
				printf("s>10!!!\n");
				GPIO_SetBits(GPIOD,GPIO_Pin_2);	//�����鿴�Ƿ���ľ����ռ�����  D2 ��1
		    flag=0;//û������0ʹ�����˵�ʱ���ܴ� 
        sbuf_me1[1] = 0;//sbuf�б�ʾ�Լ�δ��
			}		
  		TIM2CH1_CAPTURE_STA=0;		//״̬����	
		}
		
	GPIO_SetBits(GPIOB,GPIO_Pin_1);	// ����������ģ�� trig  ������ģ��5v����
	delay_us(20);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);	
 	delay_ms(10);				 
	if(TIM4CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ  Pb.6��tim4ch1  -->echo����
		{
			temp=TIM4CH1_CAPTURE_STA&0X3F;
			temp*=65536;					//���ʱ���ܺ�
			temp+=TIM4CH1_CAPTURE_VAL;		//�õ��ܵĸߵ�ƽʱ��
			s=((temp/2)*344)/10000;//sΪ���루���ף�
					printf("%d!!!\n",s); 	
			if(s<50)//�����߹���
			{ 
				printf(" someone is coming����\n");
				if(flag==0)//�������û����
					motor_ccw(55);//�����ת
			//	flag=1;
				delay_ms(1000);
				delay_ms(1000);
				YY=0;//��������
				delay_ms(100);
				YY=1;//�����ر�
				delay_ms(1000);
				delay_ms(1000);
				delay_ms(1000);
			//flag=0;
				if(flag==0)//�������û����
					motor_cw(40);//�����ת
			}
			TIM4CH1_CAPTURE_STA=0;		//״̬����	
		}
		
 }
}
