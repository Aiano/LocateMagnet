/*****************************************************************************
*                                                                                                                                                         *
*                        File:    ���ģ��IIC, HAL��汾����׼��Ҳ                                                     *
*                                    ���ã�ֻ��Ҫ��һЩ���Ӧ�޸ļ���                                                     *
*                         ��ʱ����ʹ��TIM3ʵ��,
													SDA 1k OpenCollector PullUp
													SCL PUSH_PULL nopull;
													����1k�������裬��SDA�ȶ�ʱ��Ϊ1us������Ƶ��500khz
*																																	--wyb 1/28/2021
*****************************************************************************/

#include "I2C_SW.h"

/* ������� SDA SCL �ĺ� HAL���    */

#define I2C_SDA_UP        HAL_GPIO_WritePin(iic.sdaGroup,iic.sdaPin,GPIO_PIN_SET)              //SDA�ߵ�ƽ
#define I2C_SDA_LOW       HAL_GPIO_WritePin(iic.sdaGroup,iic.sdaPin,GPIO_PIN_RESET)            //SDA�͵�ƽ
#define I2C_SCL_UP        HAL_GPIO_WritePin(iic.sclGroup,iic.sclPin,GPIO_PIN_SET)              //SCL�ߵ�ƽ
#define I2C_SCL_LOW       HAL_GPIO_WritePin(iic.sclGroup,iic.sclPin,GPIO_PIN_RESET)            //SCL�͵�ƽ


/* ��ȡ SDA SCL ����״̬����׼���Ҳ������ͬ�߼�    */

#define I2C_SDA        HAL_GPIO_ReadPin(iic.sdaGroup,iic.sdaPin)        //��ȡSDA����״̬
#define I2C_SCL        HAL_GPIO_ReadPin(iic.sclGroup,iic.sclPin)        //��ȡSCL����״̬


/*******************************************************************************
* �� �� ��         : i2c_start
* ��������         : iic��ʼ�ź�,SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾ�����ź� 
* ��    ��         : swiic�ṹ��
* ��    ��         : ��
*******************************************************************************/
void i2c_start(SWI2C iic)
{
    I2C_SDA_UP;
    I2C_SCL_UP;
    delay_us(50);
    I2C_SDA_LOW;      //������ʼ�ź�
    delay_us(50);
    I2C_SCL_LOW;      //ǯסI2C���ߣ�׼�����ͻ��������
}


/*******************************************************************************
* �� �� ��         : i2c_stop
* ��������         : iicֹͣ�ź�,SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾֹͣ�ź�
* ��    ��         : swiic�ṹ��
* ��    ��         : ��
*******************************************************************************/
void i2c_stop(SWI2C iic)
{
    I2C_SDA_LOW;         //���ͽ��������������ź�
	delay_us(50);
    I2C_SCL_UP;
    delay_us(50);         //������������ʱ�����5��
    I2C_SDA_UP;         //����I2C���߽����ź�
    delay_us(50);
}


/*******************************************************************************
* �� �� ��         : i2c_send
* ��������         : iic��������
* ��    ��         : swiic�ṹ�壬uint8_t dat,Ҫ���͵�����
* ��    ��         : ��
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
* �� �� ��         : i2c_read
* ��������         : iic��������
* ��    ��         : swiic�ṹ��
* ��    ��         : ��
*******************************************************************************/
uint8_t i2c_read(SWI2C iic)
{
   uint8_t temp;
   static uint8_t dat;
   I2C_SDA_UP;                     //�ͷ�����׼������
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
* �� �� ��         : i2c_wit_ack
* ��������         : iic�ȴ�Ӧ��
* ��    ��         : swiic�ṹ��
* ��    ��         : 0/1������1��ʾ��Ӧ���źţ�����0��ʾӦ��
*******************************************************************************/
char i2c_wit_ack(SWI2C iic)
{
    
	uint8_t con=0;
	I2C_SDA_UP;       //�ͷ������ߣ�׼������Ӧ��
	delay_us(50);
	I2C_SCL_UP;       //CPU����SCL = 1, ��ʱ�����᷵��ACKӦ��
	delay_us(50);
  while( I2C_SDA )    //CPU��ȡSDA����״̬
  {
		con++;
		if(con==255)
		{
			i2c_stop(iic);
			return 1;     //��Ӧ���ź�
		}
  }
  I2C_SCL_LOW; 
  delay_us(50);
  return 0;             //��Ӧ��
}

/*******************************************************************************
* �� �� ��         : i2c_ack
* ��������         : iicӦ���ź�
* ��    ��         : swiic�ṹ��
* ��    ��         : ��
*******************************************************************************/
void i2c_ack(SWI2C iic)
{
    I2C_SDA_LOW;
    delay_us(50);
    I2C_SCL_UP;            //CPU����1��ʱ��
    delay_us(50);         //ʱ�ӵ͵�ƽ���ڴ���4��
    I2C_SCL_LOW;         //��ʱ���ߣ�ǯסI2C�����Ա��������
    delay_us(50);
    I2C_SDA_UP;            //CPU�ͷ�SDA����
}


/*******************************************************************************
* �� �� ��         : i2c_no_ack
* ��������         : iic��Ӧ���ź�
* ��    ��         : swiic�ṹ��
* ��    ��         : ��
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
* �� �� ��         : as5600read
* ��������         : ��������λ��(Angle�Ĵ���)
* ��    ��         : swiic ���Žṹ��
* ��    ��         : return 12bit ԭʼֵ������Ϊ5000
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
* �� �� ��         : delay_us
* ��������         : ��ʱ��������ʱus
* ��    ��         : i
* ��    ��         : ��
*******************************************************************************/
void delay_us(uint32_t i)
{
	TIM1->CNT=0;             //���������
    while(TIM2->CNT<i);     //�ȴ�ʱ�䵽��
}