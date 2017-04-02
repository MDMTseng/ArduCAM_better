#include "mArduCAM_OV2640_mini.hpp"


#include "commonTools.h"
//#define DBUG_PRINT
int mArduCAM_OV2640_mini::Init()
{
  uint8_t vid, pid;
  get_vid_pid(&vid, &pid);
  if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
    //Serial.println(F("ACK CMD Can't find OV2640 module!"));
    return -1;
  }


  int ret;
  ret = I2C_W8bAd8bVa(i2c_addr, 0xff, 0x01);
  if (ret != 0)return ret;
  ret = I2C_W8bAd8bVa(i2c_addr, 0x12, 0x80);
  if (ret != 0)return ret;
  delay(100);
  if (0)
  {
    /*I2C_WRegSet_PROGMEM_8b(i2c_addr,OV2640_JPEG_INIT,sizeof(OV2640_JPEG_INIT));
      I2C_WRegSet_PROGMEM_8b(i2c_addr,OV2640_YUV422,sizeof(OV2640_YUV422));
      I2C_WRegSet_PROGMEM_8b(i2c_addr,OV2640_JPEG,sizeof(OV2640_JPEG));
      I2C_W8bAd8bVa(i2c_addr,0xff, 0x01);
      I2C_W8bAd8bVa(i2c_addr,0x15, 0x00);
      I2C_WRegSet_PROGMEM_8b(i2c_addr,OV2640_320x240_JPEG,sizeof(OV2640_320x240_JPEG));
      //wrSensorReg8_8(0xff, 0x00);
      //wrSensorReg8_8(0x44, 0x32);*/
  }
  else
  {
    ret = I2C_WRegSet_PROGMEM_8b(i2c_addr, OV2640_QVGA, sizeof(OV2640_QVGA));
    if (ret != 0)return ret;
    I2C_W16bAd8bVa(i2c_addr,0x3818, 0x81);
    I2C_W16bAd8bVa(i2c_addr,0x3621, 0xA7);
  }


  byte data[4];
  while(1){
    data[0]=0|0x80;
    data[1]=0x55;
    data[2]=0x0;
    data[3]=0x0;
    SPI_CS_EN(1);
    SPI_Transfer(data, 2);
    SPI_CS_EN(0);

    SPI_CS_EN(1);
    SPI_Transfer(data+2, 2);
    SPI_CS_EN(0);

    
    if (data[3] == 0x55){
      Serial.println(F("ACK CMD SPI interface OK."));break;
    }else{
      Serial.print(F("ACK CMD SPI interface Error!::"));
      Serial.println(data[2],HEX);
      delay(1000);continue;
    }
  }
  return 0;
}



int mArduCAM_OV2640_mini::ImageCap_Session_BEGIN(uint32_t *pixelCount)
{
  
  flush_fifo();
  clear_fifo_flag();
  start_capture();

  start_capture();

  while (1)
  {
    byte trig= SPI_Get8b(SPI_REG_ARDUCHIP_TRIG);
    if(trig&SPI_REG_CAP_DONE_MASK)break;
    delay(1);
  }
  
  int ret = set_fifo_burst_begin(pixelCount);//Set fifo burst mode
  if(ret != 0 )return ret;

  return 0;
}
int mArduCAM_OV2640_mini::ImageCap_Session_RECV(byte* buff, int recvL)
{
  fifo_burst_recv(buff, recvL);
  return 0;
}
int mArduCAM_OV2640_mini::ImageCap_Session_END()
{
  set_fifo_burst_end();
  
  clear_fifo_flag();
  return 0;
}

int mArduCAM_OV2640_mini::start_capture(void)
{
  SPI_CS_EN(1);
  byte data[]={ARDUCHIP_FIFO|0x80, FIFO_CLEAR_MASK};
  SPI_Transfer(data, sizeof(data));
  SPI_CS_EN(0);
  return 0;
}
int mArduCAM_OV2640_mini::flush_fifo(void)
{
  SPI_CS_EN(1);
  byte data[]={ARDUCHIP_FIFO|0x80, FIFO_START_MASK};
  SPI_Transfer(data, sizeof(data));
  SPI_CS_EN(0);
  return 0;
  return I2C_W8bAd8bVa(i2c_addr, ARDUCHIP_FIFO, FIFO_START_MASK);
}
int mArduCAM_OV2640_mini::clear_fifo_flag(void )
{
  SPI_CS_EN(1);
  byte data[]={ARDUCHIP_FIFO|0x80, FIFO_CLEAR_MASK};
  SPI_Transfer(data, sizeof(data));
  SPI_CS_EN(0);
  return 0;
}
uint32_t mArduCAM_OV2640_mini::read_fifo_length(void )
{
  SPI_CS_EN(1);
  uint32_t len1, len2, len3, length = 0;
  len1 = SPI_Get8b(SPI_REG_FIFO_SIZE1);
  len2 = SPI_Get8b(SPI_REG_FIFO_SIZE2);
  len3 = SPI_Get8b(SPI_REG_FIFO_SIZE3) & 0x7f;
  length = ((len3 << 16) | (len2 << 8) | len1) & 0x07fffff;
  SPI_CS_EN(0);
  return length;
}

int mArduCAM_OV2640_mini::set_fifo_burst_begin(uint32_t *fifoL)
{
  int ret=0;
  do{
    uint32_t len = read_fifo_length();
  
    SPI_CS_EN(1);
    printfx("length:%u",len);
    
    byte dat = SPI_REG_BURST_FIFO_READ;
    SPI_Transfer(&dat);
  
    if( len == 0 )
    {
      ret=-1;
      break;
    }
    delay(50);
    //Find fifo header and depelete residue
    uint8_t temp = 0, temp_last = 0;
    temp=0;
    SPI_Transfer(&temp);
    len --;
    
    if( len == 0 )
    {
      ret=-1;
      break;
    }
  
  
    if(fifoL) 
      *fifoL=len;
    ret = 0;
  }while(0);
  
  return ret;
}

int mArduCAM_OV2640_mini::fifo_burst_recv(byte* buff, int recvL)
{
  
  int ret= SPI_Transfer(buff,recvL);
  return ret;
}

int mArduCAM_OV2640_mini::set_fifo_burst_end()
{
  
  SPI_CS_EN(0);
  return 0;
}
int mArduCAM_OV2640_mini::get_vid_pid( uint8_t *vid, uint8_t *pid)
{
  int ret = 0;
  if ((ret = I2C_W8bAd8bVa(i2c_addr, 0xff, 0x01)) != 0)return ret;
  if ((ret = I2C_R8bAd8bVa(i2c_addr, OV2640_CHIPID_HIGH, vid)) != 0)return ret;
  if ((ret = I2C_R8bAd8bVa(i2c_addr, OV2640_CHIPID_LOW, pid)) != 0)return ret;
  return 0;
}

mArduCAM_OV2640_mini::mArduCAM_OV2640_mini(int CS_PIN)
{
  i2c_addr = 0x30;
  SPI_INIT(CS_PIN);
  SPI_CS_EN(0);

}
