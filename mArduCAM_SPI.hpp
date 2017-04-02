#ifndef mARDUCAM_SPI_H
#define mARDUCAM_SPI_H
#include "mArduCAM.hpp"
#include <SPI.h>
#include <pins_arduino.h>
//#include <avr/pgmspace.h>
#define SPI_MISO 50 // PB3 SPI MISO 
#define SPI_MOSI 51 // PB2 SPI MOSI
#define SPI_CLK 52  // PB1 SPI CLK

#define cbi(reg, bitmask) *reg &= ~bitmask
#define sbi(reg, bitmask) *reg |= bitmask

class mArduCAM_SPI
{
  public:


  
  static int SPI_Transfer(void *buf_io_swap, size_t bufL)
  {
    byte *buff=(byte *)buf_io_swap;
    /*while ( bufL-- )
    {
      *buff =  SPI.transfer(*buff);
      buff++;
    }*/
    SPI.transfer(buff,bufL);
    return 0;
  };
  
  int SPI_Transfer(uint8_t *dat_io_swap)
  {
    return SPI_Transfer(dat_io_swap,sizeof(*dat_io_swap));
  };
  int SPI_Transfer(uint16_t *dat_io_swap)
  {
    return SPI_Transfer(dat_io_swap,sizeof(*dat_io_swap));
  };
  uint8_t SPI_Get8b(uint8_t addr)
  {
    SPI_CS_EN(1);
    uint8_t dat[]={addr,0};
    SPI_Transfer(dat,sizeof(dat));
    
    SPI_CS_EN(0);
    return dat[1];
  };

  protected:
  void SPI_CS_EN(uint8_t en)
  { //active low
    if (en)
      cbi(P_CS, B_CS);
    else
      sbi(P_CS, B_CS);
  }
  void SPI_INIT(int CS_PIN)
  {
    pinMode(SPI_MISO,INPUT);
    pinMode(SPI_MOSI,OUTPUT);
    pinMode(SPI_CLK,OUTPUT);
   
    pinMode(CS_PIN, OUTPUT);
    P_CS  = portOutputRegister(digitalPinToPort(CS_PIN));
    B_CS  = digitalPinToBitMask(CS_PIN);
    SPI.begin();
  }

  volatile uint8_t *P_CS;
  uint8_t B_CS;
};
#endif



