/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//
// LED MATRIX DISPLAY MANAGER
//
/////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Display.h"
#include "Matrix.h"

CDisplay Display;

/////////////////////////////////////////////////////////////////////////
//
// LARGE SCOREBOARD DIGITS
//
/////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////
//
// FULL CHARACTER SET ASCII 33-126
//
// PROPORTIONAL SPACED CHARACTERS
//
/////////////////////////////////////////////////////////////////////////
const byte charSet[] PROGMEM = {
 
1,1,1,1,1,0,1,0, //!
5,5,0,0,0,0,0,0, //"
10,10,31,10,31,10,10,0, //#
2,7,4,7,1,7,2,0, //$
0,0,25,26,4,11,19,0, //%
0,4,10,4,11,10,5,0, //&
1,1,0,0,0,0,0,0, //'
1,2,2,2,2,2,2,1, //(
2,1,1,1,1,1,1,2, //)
0,5,2,7,2,5,0,0, //*
0,2,2,7,2,2,0,0, //+
0,0,0,0,0,1,1,2, //,
0,0,0,7,0,0,0,0, //-
0,0,0,0,0,0,1,0, //.
0,1,1,2,2,4,4,0, ///

6,9,11,13,9,9,6,0, //0
2,6,2,2,2,2,7,0, //1
6,9,1,6,8,8,15,0, //2
6,9,1,6,1,9,6,0, //3
1,3,5,9,15,1,1,0, //4
15,8,8,6,1,9,6,0, //5
6,9,8,14,9,9,6,0, //6
15,1,1,2,4,4,4,0, //7
6,9,9,6,9,9,6,0, //8
6,9,9,7,1,9,6,0, //9

0,0,0,1,0,1,0,0,//:
0,0,0,1,0,1,1,2, //;
0,1,2,4,2,1,0,0, //<
0,0,7,0,7,0,0,0, //=
0,4,2,1,2,4,0,0, //>
2,5,1,2,2,0,2,0, //?
14,17,17,23,21,23,16,15, //@

6,9,9,15,9,9,9,0, //A
14,9,9,14,9,9,14,0, //B
6,9,8,8,8,9,6,0, //C
14,9,9,9,9,9,14,0, //D
15,8,8,14,8,8,15,0, //E
15,8,8,14,8,8,8,0, //F
6,9,8,11,9,9,7,0, //G
9,9,9,15,9,9,9,0, //H
7,2,2,2,2,2,7,0, //I
15,1,1,1,1,9,6,0, //J
9,10,12,8,12,10,9,0, //K
8,8,8,8,8,8,15,0, //L
17,27,21,17,17,17,17,0, //M
9,9,13,11,9,9,9,0, //N
6,9,9,9,9,9,6,0, //O
14,9,9,14,8,8,8,0, //P
6,9,9,9,11,9,7,0, //Q
14,9,9,14,9,9,9,0, //R
6,9,8,6,1,9,6,0, //S
31,4,4,4,4,4,4,0, //T
9,9,9,9,9,9,6,0, //U
9,9,9,9,9,10,12,0, //V
17,17,17,17,21,27,17,0, //W
17,17,10,4,10,17,17,0, //X
17,17,10,4,4,4,4,0, //Y
31,1,2,4,8,16,31,0, //Z

3,2,2,2,2,2,2,3, //[
0,4,4,2,2,1,1,0, // backslash
3,1,1,1,1,1,1,3, //]
2,5,0,0,0,0,0,0, //^
0,0,0,0,0,0,0,7, //_

0b01111110,
0b10000001,
0b10100101,
0b10000001,
0b10100101,
0b10111101,
0b10000001,
0b01111110,

0,0,14,1,7,9,7,0, //a
8,8,14,9,9,9,14,0, //b
0,0,6,9,8,9,6,0, //c
1,1,7,9,9,9,7,0, //d
0,0,6,9,15,8,6,0, //e
3,4,4,6,4,4,4,0, //f
0,0,7,9,9,7,1,14, //g
8,8,14,9,9,9,9,0, //h
1,0,1,1,1,1,1,0, //i
1,0,1,1,1,1,1,2, //j
8,8,9,10,12,10,9,0, //k
2,2,2,2,2,2,1,0, //l
0,0,30,21,21,17,17,0, //m
0,0,14,9,9,9,9,0, //n
0,0,6,9,9,9,6,0, //o
0,0,14,9,9,14,8,8, //p
0,0,7,9,9,7,1,1, //q
0,0,14,9,8,8,8,0, //r
0,0,7,8,6,1,14,0, //s
2,2,3,2,2,2,1,0, //t
0,0,9,9,9,9,7,0, //u
0,0,9,9,9,10,12,0, //v
0,0,17,17,21,21,27,0, //w
0,0,17,10,4,10,17,0, //x
0,0,9,9,9,7,1,14, //y
0,0,31,2,4,8,31,0, //z


1,2,2,4,2,2,2,1, //{
1,1,1,1,1,1,1,1, //|
4,2,2,1,2,2,2,4, //}
5,10,0,0,0,0,0,0 //~

};

/////////////////////////////////////////////////////////////////////////
// Lookup the 8 raster rows for an ASCII character. Unknown characters
// are blanks. The function return the width of the character
int lookupChar(char ch, byte *buf)
{
  if(ch<33 || ch>126) {
    memset(buf, 0, 8);    
    return 3;
  }
  else {
    byte mask = 0;
    const byte *p = charSet + ((ch-33) * 8);
    for(int i=0; i<8; ++i) {
      buf[i] = pgm_read_byte_near(p + i);
      mask |= buf[i];
    }
    if(mask & 0x80) {  return 8; }
    if(mask & 0x40) {  return 7; }
    if(mask & 0x20) {  return 6; }
    if(mask & 0x10) {  return 5; }
    if(mask & 0x08) {  return 4; }
    if(mask & 0x04) {  return 3; }
    if(mask & 0x02) {  return 2; }
    if(mask & 0x01) {  return 1; }
    return 3; 
  }
}

/////////////////////////////////////////////////////////////////////////
// Measure a string and return the full width in pixels. A 1 pixel gap
// is added for each character
int  CDisplay::measureText(const char *sz) {
  int width = 0;
  while(*sz) {
    byte buf[8];        
    width += lookupChar(*sz, buf) + 1; 
    sz++;
  }
  return width;
}

/////////////////////////////////////////////////////////////////////////
// Display a string using the proportional spaced font. The ofs parameter
// is the index of the pixel column which will appear in first column of
// the display and can be used to implement scrolling
void CDisplay::showText(const char *sz, int ofs) 
{
  int i;
  int pos = 0;
  unsigned int data[8] = {0};
  while(*sz && pos < (ofs + 16)) {
    
    // get the definition and width of the next character
    byte buf[8];        
    char width = lookupChar(*sz, buf);
    
    // get the position of the rightmost column of this 
    // character in display coordinates (zero on left)
    int ext = (pos - ofs) + width;    
    
    // is the character visible on the screen?
    if(ext >= 0)
    {
      // calculate the bitwise left shift needed
      // to bring this character from right most
      // colum,n
      int shift = 16 - ext;
      if(shift < 0) {        
        for(i=0; i<8; ++i) {
          data[i] |= ((unsigned int)buf[i])>>(-shift);
        }
      }
      else {
        for(i=0; i<8; ++i) {
          data[i] |= ((unsigned int)buf[i])<<shift;
        }
      }
    }
    pos += (width + 1);
    ++sz;
  }
  for(i=0; i<8; ++i) {
    Matrix.m_DrawBuffer[i] = data[i]>>8;
    Matrix.m_DrawBuffer[i+8] = (byte)data[i];
  }
  Matrix.flip();  
}

/////////////////////////////////////////////////////////////////////////
// Show a pair of large digits. The shift parameter adds a 4-pixel gap
// between the characters 
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

/////////////////////////////////////////////////////////////////////////
// Show a 2-digit counter 
void CDisplay::showCount(int i)
{
  i %= 100;
  showDigits(i/10, i%10,0);
  Matrix.flip();
}
  
/////////////////////////////////////////////////////////////////////////
// Show two single digit scores with a "divider" line
void CDisplay::showScores(int a, int b)
{
  showDigits(a,b,1);
  for(int i=0; i<8; i+=2) {
    Matrix.m_DrawBuffer[i] |= 1;
    Matrix.m_DrawBuffer[9+i] |= 128;
  }
  Matrix.flip();
}

