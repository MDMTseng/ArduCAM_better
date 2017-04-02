
#include "commonTools.h"
#include <stdio.h>
#include "Arduino.h"
#include <stdarg.h>
void printfx(const char *format, ...)
{
  char buf[200];
  va_list ap;
  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  //Serial.print(buf);

  char *bufiter=buf;
  while(*bufiter)
  {
    UDR0=*bufiter;
    while (!(UCSR0A & (1 << UDRE0)));//wait for byte to transmit
    bufiter++;
  }
  va_end(ap);
}
