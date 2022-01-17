#include "NRF24.h"
#include "main.h"

//------------------------------------------------

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
//extern UART_HandleTypeDef huart1;



//------------------------------------------------

#define TX_ADR_WIDTH 3

#define TX_PLOAD_WIDTH 5
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xb3,0xb4,0x01};
uint8_t RX_BUF[TX_PLOAD_WIDTH] = {0};
extern char str1[150];
extern uint8_t receive;
uint8_t ErrCnt_Fl = 0;
uint8_t data=0x01;

//------------------------------------------------

__STATIC_INLINE void DelayMicro(__IO uint32_t micros)

{
  micros *= (SystemCoreClock / 1000000) / 9;
  /* Wait till done */
  while (micros--) ;
}
/////////////////////////////////////////////////////////
//////////////////          /////////////////////////////
/////////////////    //     /////////////////////////////
////////////////    ///    //////////////////////////////
///////////////    //     ///////////////////////////////
//////////////          /////////////////////////////////
/////////////     /     /////////////////////////////////
////////////     //     /////////////////////////////////
///////////     ///     /////////////////////////////////
//--------------------------------------------------

uint8_t NRF24_ReadReg(uint8_t addr)
{
  uint8_t dt=0, cmd;
  CSN1_ON;
  HAL_SPI_TransmitReceive(&hspi1,&addr,&dt,1,1000);
  if (addr!=STATUS)//если адрес равен адрес регистра статус то и возварщаем его состояние
  {
    cmd=0xFF;
    HAL_SPI_TransmitReceive(&hspi1,&cmd,&dt,1,1000);
  }
  CSN1_OFF;
  return dt;
}

//------------------------------------------------

void NRF24_WriteReg(uint8_t addr, uint8_t dt)
{
  addr |= W_REGISTER;//включим бит записи в адрес
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);//отправим адрес в шину
  HAL_SPI_Transmit(&hspi1,&dt,1,1000);//отправим данные в шину
  CSN1_OFF;
}

//------------------------------------------------

void NRF24_ToggleFeatures(void)
{
  uint8_t dt[1] = {ACTIVATE};
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  dt[0] = 0x73;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  CSN1_OFF;
}

//------------------------------------------------

void NRF24_Read_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)

{
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);//отправим адрес в шину
  HAL_SPI_Receive(&hspi1,pBuf,bytes,1000);//отправим данные в буфер
  CSN1_OFF;
}

//------------------------------------------------

void NRF24_Write_Buf(uint8_t addr,uint8_t *pBuf,uint8_t bytes)

{
  addr |= W_REGISTER;//включим бит записи в адрес
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);//отправим адрес в шину
  DelayMicro(1);
  HAL_SPI_Transmit(&hspi1,pBuf,bytes,1000);//отправим данные в буфер
  CSN1_OFF;
}

//------------------------------------------------
void NRF24_FlushRX(void)
{
  uint8_t dt[1] = {FLUSH_RX};
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  CSN1_OFF;
}

//------------------------------------------------

void NRF24_FlushTX(void)

{
  uint8_t dt[1] = {FLUSH_TX};
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,dt,1,1000);
  DelayMicro(1);
  CSN1_OFF;
}

//------------------------------------------------

void NRF24L01_RX_Mode(void)
{
  uint8_t regval=0x00;
  regval = NRF24_ReadReg(CONFIG);
  //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  regval |= (1<<PWR_UP)|(1<<PRIM_RX);
  NRF24_WriteReg(CONFIG,regval);
  CE1_SET;
  DelayMicro(150); //Задержка минимум 130 мкс
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}
//------------------------------------------------

void NRF24L01_TX_Mode(uint8_t *pBuf)

{
  NRF24_Write_Buf(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
  CE1_RESET;
  // Flush buffers
  NRF24_FlushRX();
  NRF24_FlushTX();
}

//------------------------------------------------

void NRF24_Transmit(uint8_t addr,uint8_t *pBuf,uint8_t bytes)
{
  CE1_RESET;
  CSN1_ON;
  HAL_SPI_Transmit(&hspi1,&addr,1,1000);//отправим адрес в шину
  DelayMicro(1);
  HAL_SPI_Transmit(&hspi1,pBuf,bytes,1000);//отправим данные в буфер
  CSN1_OFF;
  CE1_SET;
}

//------------------------------------------------

uint8_t NRF24L01_Send(uint8_t *pBuf)
{

  uint8_t status=0x00, regval=0x00;
	NRF24L01_TX_Mode(pBuf);
	regval = NRF24_ReadReg(CONFIG);
	//если модуль ушел в спящий режим, то разбудим его, включив бит PWR_UP и выключив PRIM_RX
	regval |= (1<<PWR_UP);
	regval &= ~(1<<PRIM_RX);
	NRF24_WriteReg(CONFIG,regval);
	DelayMicro(150); //Задержка минимум 130 мкс
	//Отправим данные в воздух
	NRF24_Transmit(WR_TX_PLOAD, pBuf, TX_PLOAD_WIDTH);
	CE1_SET;
	DelayMicro(15); //minimum 10us high pulse (Page 21)
	CE1_RESET;
	//while((GPIO_PinState)IRQ1 == GPIO_PIN_SET) {}
	status = NRF24_ReadReg(STATUS);
	if(status&TX_DS) //tx_ds == 0x20
	{
			//LED_TGL;
	NRF24_WriteReg(STATUS, 0x20);
	}
	else if(status&MAX_RT)
	{
		NRF24_WriteReg(STATUS, 0x10);
		NRF24_FlushTX();
	}
	regval = NRF24_ReadReg(OBSERVE_TX);
	//Уходим в режим приёмника
  NRF24L01_RX_Mode();
  return regval;
}
//------------------------------------------------

uint8_t NRF24L01_Receive(void)
{
  uint8_t status=0x01;
  uint8_t gt=0x01;
	//while(IRQ1) {}
//	status = NRF24_ReadReg(STATUS);
//	sprintf(str1,"STATUS: 0x%02X\r\n",status);
	//HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),0x1000);
	//LED_TGL;
	status = NRF24_ReadReg(STATUS);
//	if(status & 0x40)
//	{
		do{
			DelayMicro(15);
			//NRF24_Read_Buf(RD_RX_PLOAD,RX_BUF,TX_PLOAD_WIDTH);
			DelayMicro(1500);
			//NRF24_WriteReg(STATUS, 0x40);
			status = NRF24_ReadReg(STATUS);

		}
		while (!(status & 0x40));
	//}
//  
//  {
		DelayMicro(15);
	NRF24_Read_Buf(RD_RX_PLOAD,RX_BUF,TX_PLOAD_WIDTH);
		DelayMicro(15);
    gt = *RX_BUF;
		NRF24_WriteReg(STATUS, 0x40);
    //gt = *(int16_t*)(RX_BUF+2);

//  }
	data=gt;
	//sprintf(str1,"DATA: 0x%02X\r\n",data);
	//HAL_UART_Transmit(&huart1,(uint8_t*)str1,strlen(str1),0x1000);
//	if (gt==0x006F){
//		LED7_OFF;
//	  RELAY_OFF;}
//	else{
//		LED7_ON;
//		RELAY_ON;}
	NRF24_FlushRX();
	return gt;

}
//------------------------------------------------
void NRF24_ini(void)
{
	CE1_RESET;
  DelayMicro(5000);
	NRF24_WriteReg(CONFIG, 0x3a); // Set PWR_UP bit, enable CRC(1 byte) &Prim_RX:0 (Transmitter)
	DelayMicro(5000);
	NRF24_WriteReg(EN_AA, 0x02); // Enable Pipe1
	NRF24_WriteReg(EN_RXADDR, 0x02); // Enable Pipe1
	NRF24_WriteReg(SETUP_AW, 0x01); // Setup address width=3 bytes
	NRF24_WriteReg(SETUP_RETR, 0x5F); // // 1500us, 15 retrans
	NRF24_ToggleFeatures();
	NRF24_WriteReg(FEATURE, 0);
	NRF24_WriteReg(DYNPD, 0);
	NRF24_WriteReg(STATUS, 0x70); //Reset flags for IRQ1
	NRF24_WriteReg(RF_CH, 76); // частота 2476 MHz
	NRF24_WriteReg(RF_SETUP, 0x2E); //TX_PWR:0dBm, Datarate:1Mbps
	NRF24_Write_Buf(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_Write_Buf(RX_ADDR_P1, TX_ADDRESS, TX_ADR_WIDTH);
	NRF24_WriteReg(RX_PW_P1, TX_PLOAD_WIDTH); //Number of bytes in RX payload in data pipe 0
	 //пока уходим в режим приёмника
  NRF24L01_RX_Mode();
  //LED_TGL;
	
}
void  TIM_ResetCounter ( TIM_TypeDef *   TIMx ) 
 { 
   /* Check the parameters */ assert_param ( IS_TIM_ALL_PERIPH ( TIMx )); 

   /* Reset the Counter Register value */ 
   TIMx -> CNT =   0 ; 
 } 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
//	uint8_t receive=0x01;
	
	TIM_ResetCounter ( TIM1 );
	if(GPIO_Pin== GPIO_PIN_8) {
		// этот цикл вроде помогает, когда происходит ложное прерывание но на spi ничего не весит или весит хуйня, тогда просто не общаемся по spi 
		for (int i=0; i<100; i++)
		{
			DelayMicro(100);
			if (IRQ1) return;
		}
    receive=NRF24L01_Receive();
		//LED2_TGL;
				if (receive==0x6F){
			//LED2_OFF;
			//RELAY_OFF;
				}
		
		else if (receive==0x85){
			//HAL_Delay(300);
			//LED2_ON;
			//RELAY_ON;
		}
  } else{

    __NOP();

  }
}

