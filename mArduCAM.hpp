#ifndef M_ARDU_CAM_HPP
#define M_ARDU_CAM_HPP

#include <arduino.h>


struct sensor_reg {
  uint16_t reg;
  uint16_t val;
};


class mArduCAM_proto
{
  public:
  /*I2S*/
  virtual int Init( void );

  virtual int ImageCap_Session_BEGIN(uint32_t *pixelCount);
  virtual int ImageCap_Session_RECV(byte* buff, int recvL);
  virtual int ImageCap_Session_END();
  
  /*
  virtual int flush_fifo(void);
  virtual int clear_fifo_flag(void);
  virtual int start_capture(void);

  virtual int set_fifo_burst_begin(uint32_t *fifoL);
  virtual int fifo_burst_recv(byte* buff, int recvL);
  virtual int set_fifo_burst_end();*/


  
  /*
  uint8_t read_fifo(void);
  
  uint8_t read_reg(uint8_t addr);
  void write_reg(uint8_t addr, uint8_t data); 
  
  uint32_t read_fifo_length(void);
  void set_fifo_burst(void);
  
  void set_mode(uint8_t mode);
 
  uint8_t bus_write(int address, int value);
  uint8_t bus_read(int address);  
 
  void set_JPEG_size(uint8_t size);
  void set_format(byte fmt);
  
  void transferBytes(uint8_t * out, uint8_t * in, uint32_t size);
  
  protected:
  regtype *P_CS;
  regsize B_CS;
  byte m_fmt;
  byte sensor_model;*/
};






#define ARDUCHIP_FIFO          0x04  //FIFO and I2C control
#define FIFO_CLEAR_MASK       0x01
#define FIFO_START_MASK       0x02
#define FIFO_RDPTR_RST_MASK     0x10
#define FIFO_WRPTR_RST_MASK     0x20



#define SPI_REG_FIFO_SIZE1        0x42  //Camera write FIFO size[7:0] for burst to read
#define SPI_REG_FIFO_SIZE2        0x43  //Camera write FIFO size[15:8]
#define SPI_REG_FIFO_SIZE3        0x44  //Camera write FIFO size[18:16]


#define SPI_REG_ARDUCHIP_TRIG          0x41  //Trigger source
#define SPI_REG_VSYNC_MASK            0x01
#define SPI_REG_SHUTTER_MASK          0x02
#define SPI_REG_CAP_DONE_MASK         0x08


#define SPI_REG_BURST_FIFO_READ      0x3C  //Burst FIFO read operation
#define SPI_REG_SINGLE_FIFO_READ    0x3D  //Single FIFO read operation

#endif
