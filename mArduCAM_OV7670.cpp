#include "mArduCAM_OV7670.hpp"

/*
 * Almost all the logic is from src blow
//
// Source code for application to transmit image from ov7670 to PC via USB
// By Siarhei Charkes in 2015
// http://privateblog.info 
//

I just put it into my framework
*/
#include "commonTools.h"
//#define DBUG_PRINT

#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask


int mArduCAM_OV7670::Init()
{

//Init 
  I2C_WRegSet_PROGMEM_8b(i2c_addr, ov7670_default_regs, sizeof(ov7670_default_regs));
  I2C_W8bAd8bVa(i2c_addr,REG_COM10, 0x32);//PCLK does not toggle on HBLANK.

//setRes
  I2C_W8bAd8bVa(i2c_addr, REG_COM3, 0x4); // REG_COM3 enable scaling
  I2C_WRegSet_PROGMEM_8b(i2c_addr, qvga_ov7670, sizeof(qvga_ov7670));
//setColor
  I2C_WRegSet_PROGMEM_8b(i2c_addr, yuv422_ov7670, sizeof(yuv422_ov7670));

  I2C_W8bAd8bVa(i2c_addr,0x11, 11); //Earlier it had the value: wrReg(0x11, 12); New version works better for me :) !!!!

  return 0;
}
int mArduCAM_OV7670::Init(int Data_1st_Pin,int VS_pin,int HS_pin,int PCLK_pin,int XCLK_pin)
{
  IO_Setup(Data_1st_Pin,VS_pin,HS_pin,PCLK_pin,XCLK_pin);
  Init();
}


//For non fifo version camera, the image cap is a timming sensitive do not spend too much time with pixel operation
int mArduCAM_OV7670::ImageCap_Session_BEGIN(uint32_t *pixelCount)
{
  XCLK_Speed(0);
  cli();//disable interrupts
  if(pixelCount)
    *pixelCount=(uint32_t)153600;//vga X yuv422
  while (((*VS_P_in)&VS_PIN_Mask));//wait for low
  while (!((*VS_P_in)&VS_PIN_Mask));//wait for high
  XCLK_Speed(offHook_XCLK_speed_factor);
  return 0;
}
int mArduCAM_OV7670::ImageCap_Session_RECV(byte* buff, int recvL)
{
  XCLK_Speed(0);
  /*
  
  */
  for(;recvL;recvL--)
  {
    
    while (!((*PCLK_P_in)&PCLK_PIN_Mask));//wait for pclk high
    *buff=*Data_P_in;
    buff++;
    //*TEST_P_ou^=TEST_PIN_Mask;
    //*TEST_P_ou^=TEST_PIN_Mask;
    while (((*PCLK_P_in)&PCLK_PIN_Mask));//wait for pclk low
  }
  XCLK_Speed(offHook_XCLK_speed_factor);
  return 0;
}
int mArduCAM_OV7670::ImageCap_Session_END()
{
  sei();
  return 0;
}
void mArduCAM_OV7670::XCLK_Speed(int XCLK_SpeedF)
{
  OCR2A=XCLK_SpeedF;
 
}
void mArduCAM_OV7670::IO_Setup(int Data_1st_Pin,int VS_pin,int HS_pin,int PCLK_pin,int XCLK_pin)
{
  //in fact, Data_1st_Pin don't have to be the first pin of the port but... whatever
  volatile uint8_t *DDR_Data=portModeRegister(digitalPinToPort(Data_1st_Pin));
  *DDR_Data=0;//Set Data port as input
  Data_P_in= portInputRegister(digitalPinToPort(Data_1st_Pin));

  pinMode(VS_pin, INPUT);
  //Set VS_pin as input
  VS_PIN_Mask  = digitalPinToBitMask(VS_pin);
  VS_P_in= portInputRegister(digitalPinToPort(VS_pin));

  pinMode(HS_pin, INPUT);
  //Set HS_pin as input
  HS_PIN_Mask  = digitalPinToBitMask(HS_pin);
  HS_P_in= portInputRegister(digitalPinToPort(HS_pin));
  
  pinMode(PCLK_pin, INPUT);
  //Set PCLK_pin as input
  PCLK_PIN_Mask  = digitalPinToBitMask(PCLK_pin);
  PCLK_P_in= portInputRegister(digitalPinToPort(PCLK_pin));

/*
  pinMode(46, OUTPUT);
  //Set PCLK_pin as input
  TEST_PIN_Mask  = digitalPinToBitMask(46);
  TEST_P_ou= portOutputRegister(digitalPinToPort(46));
*/

  
  pinMode(XCLK_pin, OUTPUT);//OC2A on Arduino Mega
  TCCR2A = (1 << COM2A0) | (1 << WGM21) | (1 << WGM20);
  TCCR2B = (1 << WGM22) | (1 << CS20);
  OCR2A = 0;//(F_CPU)/(2*(OCR2A+1))

}

mArduCAM_OV7670::mArduCAM_OV7670()
{
  i2c_addr = 0x21;
  offHook_XCLK_speed_factor=2;
}
