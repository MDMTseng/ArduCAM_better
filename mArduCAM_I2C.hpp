#ifndef mARDUCAM_I2C_H
#define mARDUCAM_I2C_H
#include "mArduCAM.hpp"
#include <Wire.h>
#include <pins_arduino.h>
//#include <avr/pgmspace.h>

class mArduCAM_I2C
{
  public:
    
    static int I2C_WArr(byte devAddr,const byte *dat,int datL)
    {
      int ret=0;
      Wire.beginTransmission(devAddr);
      byte i=0;
      ret=Wire.write(dat,datL);
      if(ret!=datL)ret=-1;//error,but endTransmission would return error as well
      else ret=0;
      if ((ret=Wire.endTransmission())!=0)
      {
        return ret;
      }
      return 0;
    };
    static int I2C_RArr(byte devAddr,byte *read_buff,size_t readL)
    {
      Wire.requestFrom(devAddr, readL);
      int i=0;
      for(i=0;i<readL;)
      {
        if (Wire.available())
        {
          read_buff[i]= Wire.read();
          i++;
        }
        else
        {
        }
      }
      
      return 0;
    };
    static int I2C_R8bAd8bVa(byte devAddr,byte addr, byte *rd)
    {
      byte buff[1]={addr};
      I2C_WArr(devAddr,buff,1);
      return I2C_RArr( devAddr,rd,1);
    };
    
    static int I2C_W8bAd8bVa(byte devAddr,byte addr,byte value)
    {
      byte buf[]={addr,value};
      return I2C_WArr(devAddr,buf,sizeof(buf));
    };
    static int I2C_W16bAd8bVa(byte devAddr,uint16_t addr,byte value)
    {
      byte buf[]={(byte)(addr>> 8),(byte)(addr& 0x00FF),value};
      return I2C_WArr(devAddr,buf,sizeof(buf));
    };
    
      // Write 8 bit values to 8 bit register address
    static int I2C_WRegSet_PROGMEM_8b(byte devAddr,const struct sensor_reg *reg_set,int reg_setL )
    {
      
      
        int err = 0;
        uint8_t reg_addr = 0;
        uint8_t reg_val = 0;
        const struct sensor_reg *next = reg_set;
        while ((reg_addr != 0xff) | (reg_val != 0xff))
        {
          reg_addr = (uint8_t)pgm_read_word(&next->reg);
          reg_val = (uint8_t)pgm_read_word(&next->val);
          err = I2C_W8bAd8bVa( devAddr,reg_addr,reg_val);
          if(err!=0)return err;
          next++;
          delayMicroseconds(1000);
        }
        return 0;
    };
    protected:
    unsigned char i2c_addr;
};
#endif



