#include "stdio.h"

#include "stm32f1xx_hal.h"

typedef struct iic{
	GPIO_TypeDef* sclGroup;
	uint16_t sclPin;
	GPIO_TypeDef* sdaGroup;
	uint16_t sdaPin;
}SWI2C;

void         i2c_start(SWI2C);                    //*    开始信号
void         i2c_ack(SWI2C);                        //*    应答信号
void         i2c_no_ack(SWI2C);                    //*    非应答信号
void         i2c_stop(SWI2C);                        //*    停止信号
char         i2c_wit_ack(SWI2C);                //*    等待应答
void         i2c_send(SWI2C,uint8_t dat);        //* 发送数据
void i2cWriteByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf);
void i2cReadByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf);
	
uint8_t i2c_read(SWI2C);                        //* 接收数据

void delay_us(uint32_t i);        //*延时
