#include "Arduino.h"
#include "oneDpong.h"
#include "APA102.h"
#include "PortIO.h"
#include "Matrix.h"

CMatrix Matrix;


byte g_xref[16] = {
  7, //0
  6, //1
  0, //2
  5, //3
  3, //4
  4, //5
  2, //6
  1, //7
  
  15, //8
  14, //9
  8,//10
  13,//11
  11,//12
  12,//13
  10,//14
  9 //15
};


/*
2  10
7  15
6  14
4  12
5  13
3  11
1  9
0  8

bit 

0-1-2-3-4-5-6-7
*/

byte g_row = 0;
ISR(TIMER1_OVF_vect) 
{
  if(!g_row) {
    // clock in single bit
    _DIGITAL_WRITE(P_DS, HIGH);
    _DIGITAL_WRITE(P_SHCP, LOW);
    _DIGITAL_WRITE(P_SHCP, HIGH);
    _DIGITAL_WRITE(P_DS, LOW);    
  }

  // turn off all anode lines
  _DIGITAL_WRITE(P_D0, LOW);    
  _DIGITAL_WRITE(P_D1, LOW);    
  _DIGITAL_WRITE(P_D2, LOW);    
  _DIGITAL_WRITE(P_D3, LOW);    
  _DIGITAL_WRITE(P_D4, LOW);    
  _DIGITAL_WRITE(P_D5, LOW);    
  _DIGITAL_WRITE(P_D6, LOW);    
  _DIGITAL_WRITE(P_D7, LOW);    

  // enable the output
  _DIGITAL_WRITE(P_STCP, LOW);
  _DIGITAL_WRITE(P_STCP, HIGH);

  byte row = g_xref[g_row];
  
  _DIGITAL_WRITE(P_D0, !!(Matrix.m_DispBuffer[row]&0x80));
  _DIGITAL_WRITE(P_D1, !!(Matrix.m_DispBuffer[row]&0x40));
  _DIGITAL_WRITE(P_D2, !!(Matrix.m_DispBuffer[row]&0x20));
  _DIGITAL_WRITE(P_D3, !!(Matrix.m_DispBuffer[row]&0x10));
  _DIGITAL_WRITE(P_D4, !!(Matrix.m_DispBuffer[row]&0x08));
  _DIGITAL_WRITE(P_D5, !!(Matrix.m_DispBuffer[row]&0x04));
  _DIGITAL_WRITE(P_D6, !!(Matrix.m_DispBuffer[row]&0x02));
  _DIGITAL_WRITE(P_D7, !!(Matrix.m_DispBuffer[row]&0x01));
  
  // enable the output
  _DIGITAL_WRITE(P_SHCP, LOW);
  _DIGITAL_WRITE(P_SHCP, HIGH);
  if(++g_row >= 16) {
    g_row = 0;
    if(Matrix.m_Switch) {
      byte *t = Matrix.m_DispBuffer;
      Matrix.m_DispBuffer = Matrix.m_DrawBuffer;
      Matrix.m_DrawBuffer = t;
      Matrix.m_Switch = 0;
    }
  }  
  
  TCNT1H = 250;  
  TCNT1L = 0;  
}





