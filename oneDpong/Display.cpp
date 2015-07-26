#include "Arduino.h"
#include "Display.h"
#include "Matrix.h"

CDisplay Display;

const byte digits[] PROGMEM = 
{
  0b00111100, 0b01000110, 0b01001010, 0b01010010, 0b01100010, 0b00111100, 
  0b00001000, 0b00011000, 0b00001000, 0b00001000, 0b00001000, 0b00111110, 
  0b00111100, 0b01000010, 0b00000010, 0b00111100, 0b01000000, 0b01111110, 
  0b00111100, 0b01000010, 0b00011100, 0b00000010, 0b01000010, 0b00111100, 
  0b00001100, 0b00010100, 0b00100100, 0b01000100, 0b01111110, 0b00000100, 
  0b01111110, 0b01000000, 0b01111100, 0b00000010, 0b00000010, 0b01111100, 
  0b00111100, 0b01000000, 0b01111100, 0b01000010, 0b01000010, 0b00111100, 
  0b01111110, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 
  0b00111100, 0b01000010, 0b00111100, 0b01000010, 0b01000010, 0b00111100, 
  0b00111100, 0b01000010, 0b01000010, 0b00111110, 0b00000010, 0b00111100
};

void CDisplay::showDigits(int a, int b, byte shift)
{  
  

  const byte *pa = (a<0)? NULL : &digits[6 * (a%10)];
  const byte *pb = (b<0)? NULL : &digits[6 * (b%10)];
  Matrix.m_DrawBuffer[0] = 0;
  Matrix.m_DrawBuffer[7] = 0;
  Matrix.m_DrawBuffer[8] = 0;
  Matrix.m_DrawBuffer[15] = 0;
  for(int i = 0; i<6; i++) {
    Matrix.m_DrawBuffer[1+i] = pa? pgm_read_byte_near(pa + i)<<shift : 0;
    Matrix.m_DrawBuffer[9+i] = pb? pgm_read_byte_near(pb + i)>>shift : 0;
  }
}

void CDisplay::showCount(int i)
{
  i %= 100;
  showDigits(i/10, i%10,0);
  Matrix.flip();
}
  
void CDisplay::showScores(int a, int b)
{
  showDigits(a,b,1);
  for(int i=0; i<8; i+=2) {
    Matrix.m_DrawBuffer[i] |= 1;
    Matrix.m_DrawBuffer[9+i] |= 128;
  }
  Matrix.flip();
}

