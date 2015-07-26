///////////////////////////////////////////////////////
// LIBRARY FOR APA102
// hotchk155/2015

#ifndef _APA102_H_
#define _APA102_H_



// Macro to assemble an RGB colour in APA102 format
//#define RGB(r,g,b) (\
//(((unsigned long)(b))<<16)|\
//(((unsigned long)(g))<<8)|\
//(((unsigned long)(r)))\
//)


typedef struct  {
  byte b;  
  byte g;  
  byte r;  
} LED;

#define RGB_LED(r,g,b) (LED){b,g,r}

class APA102
{
public:
  volatile int m_ledIndex;
  volatile int m_sendState;
//  volatile int m_dataLen;
  LED *m_data;
  int m_numLeds;
  APA102(int numLeds);
  void begin();
  void begin_transfer();
  int is_transfer_complete();
  void refresh();
  void cls();
  void run_spi();
};

#endif

