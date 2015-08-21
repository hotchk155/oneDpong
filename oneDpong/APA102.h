/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////

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

#define APA102_MAX_LEDS   300

#define RGB_LED(r,g,b) (LED){b,g,r}

class APA102
{
public:
  volatile int m_ledIndex;
  volatile int m_sendState;
  LED m_data[APA102_MAX_LEDS];
  int m_numLeds;
  APA102();
  void begin();
  void begin_transfer();
  int is_transfer_complete();
  void refresh();
  void cls();
  void fill(LED colour);
  void resize(int s);
  void run_spi();
};

#endif

