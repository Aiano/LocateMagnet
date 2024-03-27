#include "stdio.h"

#include "stm32f1xx_hal.h"

typedef struct iic{
	GPIO_TypeDef* sclGroup;
	uint16_t sclPin;
	GPIO_TypeDef* sdaGroup;
	uint16_t sdaPin;
}SWI2C;

void         i2c_start(SWI2C);                    //*    ��ʼ�ź�
void         i2c_ack(SWI2C);                        //*    Ӧ���ź�
void         i2c_no_ack(SWI2C);                    //*    ��Ӧ���ź�
void         i2c_stop(SWI2C);                        //*    ֹͣ�ź�
char         i2c_wit_ack(SWI2C);                //*    �ȴ�Ӧ��
void         i2c_send(SWI2C,uint8_t dat);        //* ��������
void i2cWriteByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf);
void i2cReadByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf);
	
uint8_t i2c_read(SWI2C);                        //* ��������

void delay_us(uint32_t i);        //*��ʱ
