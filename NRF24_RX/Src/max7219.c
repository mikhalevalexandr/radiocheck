#include "max7219.h"

uint8_t aTxBuf[1]={0};
extern SPI_HandleTypeDef hspi3;
char dg=8;

#define cs_set() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define cs_reset() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)

void Send_7219 (uint8_t rg, uint8_t dt)
{
	cs_set();
	aTxBuf[0]=rg;
	HAL_SPI_Transmit (&hspi3, (uint8_t*)aTxBuf, 1, 5000);
	aTxBuf[0]=dt;
	HAL_SPI_Transmit (&hspi3, (uint8_t*)aTxBuf, 1, 5000);
	cs_reset();
}
//------------------------------------------------------
void Clear_7219 (void)
{
	uint8_t i=dg;
	do
	{
		Send_7219(i,0xF);//символ пустоты
	} while (--i);
}
//------------------------------------------------------
void Number_7219 (volatile long n)
{
	uint8_t ng=0;//переменная для минуса
	if(n<0)
	{
		ng=1;
		n*=-1;
	}
	uint8_t i=0;
	do
	{
		Send_7219(++i,n%10);//символ цифры
		n/=10;
	} while(n);
	if(ng)
	{
		Send_7219(i+1,0x0A);//символ -
	}
}
//-------------------------------------------------------
void NumberL_7219 (volatile int n) //пишем в левую тетраду
{
  uint8_t ng=0;//переменная для минуса
  if(n<0)
  {
    ng=1;
    n*=-1;
  }
  uint8_t i=4;
  do
  {
    Send_7219(++i,n%10);//символ цифры
    n/=10;
  } while(n);
  if(ng)
  {
    Send_7219(i+1,0x0A);//символ -
  }
}
//-------------------------------------------------------
void Init_7219 (void)
{
		Send_7219(0x09,0xFF);//включим режим декодирования
		Send_7219(0x0B,dg-1);//кол-во используемых разрядов
		Send_7219(0x0A,0x06);//интенсивность свечения
		Send_7219(0x0C,0x01);//включим индикатор
		Clear_7219();
}
