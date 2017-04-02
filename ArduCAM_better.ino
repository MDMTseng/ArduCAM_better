
#include <Wire.h>
#include "mArduCAM_OV2640_mini.hpp"
#include "mArduCAM_OV7670.hpp"
#include "commonTools.h"

int loopx(mArduCAM_proto * cam);
void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(250000);  // start serial for output

  
  int ret=0;
  
  mArduCAM_OV2640_mini cam_2MPf(53);
  mArduCAM_OV7670 cam_0_3MPf;
  mArduCAM_proto *cam_v0;

  if(1)
  {
    ret=cam_2MPf.Init();
    cam_v0=&cam_2MPf;
  }
  else
  {
    ret=cam_0_3MPf.Init(22,34,35,36,10);// Data_1st_Pin, VS_pin, HS_pin, PCLK_pin, XCLK_pin) 
    cam_v0=&cam_0_3MPf;
  }
  //Caution:: XCLK_pin cannot pick arbitrarily, it has to be OC2A pin, it's to generate upto 8MHz clk
  if(ret==0)
  {
    printfx("mArduCAM_OV7670 init success\n");
    while(1)
    {
      loopx(cam_v0);
    }
  }
  else
  {
    printfx("mArduCAM_OV7670 init failed: err: %d\n",ret);
  }
}

void loop()
{
  
}
struct RGB565
{
  unsigned int R : 5;
  unsigned int G : 6;
  unsigned int B : 5;
};
//char grayAscii[]="$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";//70 level
char grayAscii[] =  "@%#x+=:-.  ";

int loopx(mArduCAM_proto * cam)
{
  uint32_t length = 0;
  
  int ret = cam->ImageCap_Session_BEGIN(&length);//Set fifo burst mode
  printfx(">>set_fifo_burst_begin ret:%d<<\n",ret);
  if(ret != 0 )return ret;
  byte buffer[640];
  uint32_t rest_len=length;
  uint16_t ccc=0;
  
  while ( rest_len )
  {
    int recvL = (rest_len < sizeof(buffer))?rest_len:sizeof(buffer);
    memset(buffer, 0, recvL);
    cam->ImageCap_Session_RECV(buffer, recvL);
    //TODO: deal with pixel data


    //printfx(">>%03d<<\n",tranBuf[0].R+tranBuf[0].G+tranBuf[0].B);
    rest_len-=recvL;
    //printfx("rest_len:%lu\n",rest_len);
    if((ccc&3)==0)
    {
      for(int i=0;i<recvL;i++)
      {
        if(i&0x3)continue;
        //printfx(grayAscii[buffer[i]>>4]);
        UDR0=grayAscii[buffer[i]/20];
        while (!(UCSR0A & (1 << UDRE0)));//wait for byte to transmit
        buffer[i]=0;
      }
      printfx("\n");
    }
    ccc++;
  }

  cam->ImageCap_Session_END();
  return 1;
}



