#ifndef mARDUCAM_PARALLEL_BUS_H
#define mARDUCAM_PARALLEL_BUS_H
#include "mArduCAM.hpp"
#include <pins_arduino.h>
//#include <avr/pgmspace.h>




#define regtype volatile uint8_t
#define regsize uint8_t

class mArduCAM_parallelBus : public mArduCAM_proto
{
  public:



  protected:

  regtype *P_CS;
  regsize B_CS;
};

#endif






