/**
  ******************************************************************************
  * @file    bsp_xxx.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g????/nrf24l01+/master ??bsp
  ******************************************************************************
  * @attention
  *
  * ????:?? iSO STM32 ??? 
  * ??    :http://www.chuxue123.com
  * ??    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
#include "nrf.h"
#include "stm32f10x_spi.h"
 u8 RX_BUF[RX_PLOAD_WIDTH];		//??????
 u8 TX_BUF[TX_PLOAD_WIDTH];		//??????
 u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x02};  // ??????????
 u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x03};

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 


void SPI_NRF_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*????IO?????*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                         |RCC_APB2Periph_GPIOC
                         |RCC_APB2Periph_GPIOG,ENABLE);

 /*??SPI1??*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

   /*?? SPI_NRF_SPI? SCK,MISO,MOSI??,GPIOA^5,GPIOA^6,GPIOA^7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //????
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  /*??SPI_NRF_SPI?CE??,?SPI_NRF_SPI? CSN ??*/
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

   /*??SPI_NRF_SPI?IRQ??*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //????
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
		  
  /* ???????,????csn??,NRF?????? */
  NRF_CSN_HIGH(); 
 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //?????
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 					//???
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 				//????8?
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 				//????,?????
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//?1?????,????????
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		   					//NSS???????
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //8??,9MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  				//????
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}

/**
  * @brief   ???NRF?/??????
  * @param   ?????
  *		@arg dat 
  * @retval  ??????
  */
u8 SPI_NRF_RW(u8 dat)
{  	
   /* ? SPI?????????? */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  
   /* ?? SPI2??????? */
  SPI_I2S_SendData(SPI1, dat);		
 
   /* ?SPI?????????? */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

/**
  * @brief   ???NRF??????????
  * @param   
  *		@arg reg:NRF???+?????
  *		@arg dat:???????????
  * @retval  NRF?status??????
  */
u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
   u8 status;
	 NRF_CE_LOW();

   NRF_CSN_LOW();
		
   status = SPI_NRF_RW(reg);
		 
   SPI_NRF_RW(dat); 
	             
   NRF_CSN_HIGH();	
		
   return(status);
}
u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();

 	NRF_CSN_LOW();
				
	SPI_NRF_RW(reg); 

	reg_val = SPI_NRF_RW(NOP);
	            
	NRF_CSN_HIGH();		
   	
	return reg_val;
}	
u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	NRF_CE_LOW();

	NRF_CSN_LOW();
			
	status = SPI_NRF_RW(reg); 

	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	   pBuf[byte_cnt] = SPI_NRF_RW(NOP); //?NRF24L01????  

	NRF_CSN_HIGH();	
		
 	return status;	
}
u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	 u8 status,byte_cnt;
	 NRF_CE_LOW();
   
	 NRF_CSN_LOW();			

   status = SPI_NRF_RW(reg); 
 	
	 for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++);	//??????? 	 
	  	   
	NRF_CSN_HIGH();			
  
  	return (status);	//??NRF24L01??? 		
}

/**
  * @brief  ?????????
  * @param  ?
  * @retval ?
  */
void NRF_RX_Mode(void)

{
	NRF_CE_LOW();	

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//?RX????

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);    //????0?????    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);//????0?????    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);      //??RF????    

   SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//????0???????      

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f); //??TX????,0db??,2Mbps,???????   

   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);  //???????????;PWR_UP,EN_CRC,16BIT_CRC,???? 

/*CE??,??????*/	
  NRF_CE_HIGH();

}    

/**
  * @brief  ??????
  * @param  ?
  * @retval ?
  */
void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

   SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);    //?TX???? 

   SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH); //??TX????,??????ACK   

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);     //????0?????    

   SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); //????0?????  

   SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);//??????????:500us + 86us;????????:10?

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);       //??RF???CHANAL

   SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);  //??TX????,0db??,2Mbps,???????   
	
   SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);    //???????????;PWR_UP,EN_CRC,16BIT_CRC,????,??????

/*CE??,??????*/	
  NRF_CE_HIGH();
    Delay(0xffff); //CE??????????????
}

/**
  * @brief  ????NRF?MCU??????
  * @param  ?
  * @retval SUCCESS/ERROR ????/????
  */
u8 NRF_Check(void)
{
	u8 buf[5]={0xC2,0xC2,0xC2,0xC2,0xC2};
	u8 buf1[5];
	u8 i; 
	 
	/*??5??????.  */  
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	/*??????? */
	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	 
	/*??*/               
	for(i=0;i<5;i++)
	{
		if(buf1[i]!=0xC2)
		break;
	} 
	       
	if(i==5)
		return SUCCESS ;        //MCU?NRF???? 
	else
		return ERROR ;        //MCU?NRF?????
}

/**
  * @brief   ???NRF???????????
  * @param   
  *		@arg txBuf:?????????????,????	
  * @retval  ????,????TXDS,????MAXRT?ERROR
  */
u8 NRF_Tx_Dat(u8 *txbuf)
{
	u8 state;  

	 /*ce??,??????1*/
	NRF_CE_LOW();

	/*????TX BUF ?? 32???*/						
   SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);

      /*CE??,txbuf??,????? */   
 	 NRF_CE_HIGH();
	  	
	  /*???????? */                            
	while(NRF_Read_IRQ()!=0); 	
	
	/*????????? */                              
	state = SPI_NRF_ReadReg(STATUS);

	 /*??TX_DS?MAX_RT????*/                  
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

	SPI_NRF_WriteReg(FLUSH_TX,NOP);    //??TX FIFO??? 

	 /*??????*/    
	if(state&MAX_RT)                     //????????
	  	 return MAX_RT; 

	else if(state&TX_DS)                  //????
		 	return TX_DS;
	 else						  
			return ERROR;                 //????????
} 

/**
  * @brief   ???NRF???????????
  * @param   
  *		@arg rxBuf :??????????,????	
  * @retval 
  *		@arg ????
  */
u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state; 
	NRF_CE_HIGH();	

	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();  	 
	              
	state=SPI_NRF_ReadReg(STATUS);
	     
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);


	if(state&RX_DR)                                
	{
	  SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);
	  SPI_NRF_WriteReg(FLUSH_RX,NOP);          
	  return RX_DR; 
	}
	else    
		return ERROR;                    //???????
}
/*********************************************END OF FILE**********************/
