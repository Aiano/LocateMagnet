/*****************************************************************************
*                                                                                                                                                         *
*                        File:    软件模拟IIC, HAL库版本，标准库也                                                     *
*                                    可用，只需要做一些相对应修改即可                                                     *
*                         延时函数使用TIM3实现,
													SDA 1k OpenCollector PullUp
													SCL PUSH_PULL nopull;
													对于1k上拉电阻，其SDA稳定时间为1us，操作频率500khz
*																																	--wyb 1/28/2021
*****************************************************************************/

#include "I2C_SW.h"

/* 定义控制 SDA SCL 的宏 HAL库版    */

#define I2C_SDA_UP        HAL_GPIO_WritePin(iic.sdaGroup,iic.sdaPin,GPIO_PIN_SET)              //SDA高电平
#define I2C_SDA_LOW       HAL_GPIO_WritePin(iic.sdaGroup,iic.sdaPin,GPIO_PIN_RESET)            //SDA低电平
#define I2C_SCL_UP        HAL_GPIO_WritePin(iic.sclGroup,iic.sclPin,GPIO_PIN_SET)              //SCL高电平
#define I2C_SCL_LOW       HAL_GPIO_WritePin(iic.sclGroup,iic.sclPin,GPIO_PIN_RESET)            //SCL低电平


/* 获取 SDA SCL 引脚状态，标准库版也是用相同逻辑    */

#define I2C_SDA        HAL_GPIO_ReadPin(iic.sdaGroup,iic.sdaPin)        //获取SDA引脚状态
#define I2C_SCL        HAL_GPIO_ReadPin(iic.sclGroup,iic.sclPin)        //获取SCL引脚状态


/*******************************************************************************
* 函 数 名         : i2c_start
* 函数功能         : iic开始信号,SCL高电平时，SDA出现一个下跳沿表示启动信号 
* 输    入         : swiic结构体
* 输    出         : 无
*******************************************************************************/
void i2c_start(SWI2C iic)
{
    I2C_SDA_UP;
    I2C_SCL_UP;
    delay_us(50);
    I2C_SDA_LOW;      //启动开始信号
    delay_us(50);
    I2C_SCL_LOW;      //钳住I2C总线，准备发送或接收数据
}


/*******************************************************************************
* 函 数 名         : i2c_stop
* 函数功能         : iic停止信号,SCL高电平时，SDA出现一个上跳沿表示停止信号
* 输    入         : swiic结构体
* 输    出         : 无
*******************************************************************************/
void i2c_stop(SWI2C iic)
{
    I2C_SDA_LOW;         //发送结束条件的数据信号
	delay_us(50);
    I2C_SCL_UP;
    delay_us(50);         //结束条件建立时间大于5μ
    I2C_SDA_UP;         //发送I2C总线结束信号
    delay_us(50);
}


/*******************************************************************************
* 函 数 名         : i2c_send
* 函数功能         : iic发送数据
* 输    入         : swiic结构体，uint8_t dat,要发送的数据
* 输    出         : 无
*******************************************************************************/
void i2c_send(SWI2C iic,uint8_t dat)
{
   unsigned char temp;
   for(temp=0x80;temp!=0;temp>>=1)
   {
       if((temp & dat)== 0){
          I2C_SDA_LOW;
       }else{
          I2C_SDA_UP;
       }
       delay_us(50);
       I2C_SCL_UP;
       delay_us(50);
       I2C_SCL_LOW;
   }
	 I2C_SDA_UP;
}


/*******************************************************************************
* 函 数 名         : i2c_read
* 函数功能         : iic接收数据
* 输    入         : swiic结构体
* 输    出         : 无
*******************************************************************************/
uint8_t i2c_read(SWI2C iic)
{
   uint8_t temp;
   static uint8_t dat;
   I2C_SDA_UP;                     //释放总线准备接收
   for(temp=0x80;temp!=0;temp>>=1)
   {
      delay_us(50);
      I2C_SCL_UP;
	  delay_us(50);
	  I2C_SDA_UP;
      if(I2C_SDA==1)
         dat|=temp;
      else
         dat&=~temp;
      I2C_SCL_LOW;
   }
   return dat;
}


/*******************************************************************************
* 函 数 名         : i2c_wit_ack
* 函数功能         : iic等待应答
* 输    入         : swiic结构体
* 输    出         : 0/1，返回1表示无应答信号，返回0表示应答
*******************************************************************************/
char i2c_wit_ack(SWI2C iic)
{
    
	uint8_t con=0;
	I2C_SDA_UP;       //释放数据线，准备接收应答
	delay_us(50);
	I2C_SCL_UP;       //CPU驱动SCL = 1, 此时器件会返回ACK应答
	delay_us(50);
  while( I2C_SDA )    //CPU读取SDA口线状态
  {
		con++;
		if(con==255)
		{
			i2c_stop(iic);
			return 1;     //无应答信号
		}
  }
  I2C_SCL_LOW; 
  delay_us(50);
  return 0;             //有应答
}

/*******************************************************************************
* 函 数 名         : i2c_ack
* 函数功能         : iic应答信号
* 输    入         : swiic结构体
* 输    出         : 无
*******************************************************************************/
void i2c_ack(SWI2C iic)
{
    I2C_SDA_LOW;
    delay_us(50);
    I2C_SCL_UP;            //CPU产生1个时钟
    delay_us(50);         //时钟低电平周期大于4μ
    I2C_SCL_LOW;         //清时钟线，钳住I2C总线以便继续接收
    delay_us(50);
    I2C_SDA_UP;            //CPU释放SDA总线
}


/*******************************************************************************
* 函 数 名         : i2c_no_ack
* 函数功能         : iic非应答信号
* 输    入         : swiic结构体
* 输    出         : 无
*******************************************************************************/
void i2c_no_ack(SWI2C iic)
{
   I2C_SDA_UP;
   delay_us(50);
   I2C_SCL_UP;
   delay_us(50);
   I2C_SCL_LOW;
   delay_us(50);
}


/*******************************************************************************
* 函 数 名         : as5600read
* 函数功能         : 读编码器位置(Angle寄存器)
* 输    入         : swiic 引脚结构体
* 输    出         : return 12bit 原始值，出错为5000
*******************************************************************************/
uint16_t as5600read(SWI2C iic)  
{
		uint16_t angle;
    i2c_start(iic);
    i2c_send(iic,0x36<<1);
		if(i2c_wit_ack(iic)) return 5000;
		i2c_send(iic,0x0C);
		if(i2c_wit_ack(iic)) return 5000;
		i2c_start(iic);
		i2c_send(iic,(0x36<<1)|1);
		if(i2c_wit_ack(iic)) return 5000;
		angle=i2c_read(iic);
		i2c_no_ack(iic);
	  angle=angle<<8;
		i2c_stop(iic);
		delay_us(50);
	
    i2c_start(iic);
    i2c_send(iic,0x36<<1);
		if(i2c_wit_ack(iic)) return 5000;
		i2c_send(iic,0x0D);
		if(i2c_wit_ack(iic)) return 5000;
		i2c_start(iic);
		i2c_send(iic,(0x36<<1)|1);
		if(i2c_wit_ack(iic)) return 5000;
		angle+=i2c_read(iic);
		i2c_no_ack(iic);
		i2c_stop(iic);
		I2C_SCL_UP;
		return angle;
} 

void i2cWriteByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf){
	i2c_start(iicInterface);
	i2c_send(iicInterface,deviceAddr); // Write
	if(i2c_wit_ack(iicInterface)) printf("Device No ACK\n");
	i2c_send(iicInterface,regAddr);
	if(i2c_wit_ack(iicInterface)) printf("No ACK\n");
	
	for(uint8_t i =0 ;i<(len-1);i++){
		i2c_send(iicInterface,dataBuf[i]);
		if(i2c_wit_ack(iicInterface)) printf("No ACK\n");
	}
	i2c_send(iicInterface,dataBuf[len-1]);
	if(i2c_wit_ack(iicInterface)) printf("No ACK\n");
	
	i2c_no_ack(iicInterface);
	i2c_stop(iicInterface);
}
	

void i2cReadByte(SWI2C iicInterface,uint8_t deviceAddr,uint8_t regAddr,uint8_t len,uint8_t* dataBuf){
	
	i2c_start(iicInterface);
	i2c_send(iicInterface,deviceAddr); // Dummy Write
	if(i2c_wit_ack(iicInterface)) printf("Device No ACK\n");
	i2c_send(iicInterface,regAddr);
	if(i2c_wit_ack(iicInterface)) printf("No ACK\n");
	
	delay_us(50);
	i2c_start(iicInterface);
	i2c_send(iicInterface,deviceAddr | 0x01); 
	if(i2c_wit_ack(iicInterface)) printf("No ACK\n");
	for(uint8_t i =0 ;i<(len-1);i++){
		dataBuf[i] = i2c_read(iicInterface);
		i2c_ack(iicInterface);
	}
	dataBuf[len-1] = i2c_read(iicInterface);
	i2c_no_ack(iicInterface);
	i2c_stop(iicInterface);
}

/*******************************************************************************
* 函 数 名         : delay_us
* 函数功能         : 延时函数，延时us
* 输    入         : i
* 输    出         : 无
*******************************************************************************/
void delay_us(uint32_t i)
{
	TIM1->CNT=0;             //清零计数器
    while(TIM2->CNT<i);     //等待时间到达
}