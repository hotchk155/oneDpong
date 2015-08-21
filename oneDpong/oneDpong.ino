/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "EEPROM.h"
#include "oneDpong.h"
#include "Matrix.h"
#include "APA102.h"
#include "Sounds.h"
#include "Display.h"
#include "Settings.h"
#include "GameCommon.h"
#include "GamePong.h"
#include "GameBreakout.h"
#include "Menu.h"



APA102 Strip;
CSounds Sounds;
CSettings Settings;
IGame* Game = NULL;
CMenu Menu;

void setup() 
{
  pinMode(P_DS, OUTPUT);  
  pinMode(P_SHCP, OUTPUT);  
  pinMode(P_STCP, OUTPUT);  
  pinMode(P_D0, OUTPUT);  
  pinMode(P_D1, OUTPUT);  
  pinMode(P_D2, OUTPUT);  
  pinMode(P_D3, OUTPUT);  
  pinMode(P_D4, OUTPUT);  
  pinMode(P_D5, OUTPUT);  
  pinMode(P_D6, OUTPUT);  
  pinMode(P_D7, OUTPUT);  
  pinMode(P_SOUND, OUTPUT);  
  
  pinMode(P_PLAYER1, INPUT_PULLUP);  
  pinMode(P_PLAYER2, INPUT_PULLUP);  
  
  pinMode(P_SELECT, INPUT_PULLUP);  
  pinMode(P_INPUT, INPUT_PULLUP);  
  
 
  Matrix.m_DrawBuffer[0] = 224;
  Matrix.m_DrawBuffer[1] = 145;
  Matrix.m_DrawBuffer[2] = 145;
  Matrix.m_DrawBuffer[3] = 237;
  Matrix.m_DrawBuffer[4] = 146;
  Matrix.m_DrawBuffer[5] = 146;
  Matrix.m_DrawBuffer[6] = 146;
  Matrix.m_DrawBuffer[7] = 12;

  Matrix.m_DrawBuffer[8] = 224;
  Matrix.m_DrawBuffer[9] = 39;
  Matrix.m_DrawBuffer[10] = 41;
  Matrix.m_DrawBuffer[11] = 41;
  Matrix.m_DrawBuffer[12] = 9;
  Matrix.m_DrawBuffer[13] = 7;
  Matrix.m_DrawBuffer[14] = 1;
  Matrix.m_DrawBuffer[15] = 14;

  Matrix.m_DispBuffer[0] = 0;
  Matrix.m_DispBuffer[1] = 236;
  Matrix.m_DispBuffer[2] = 146;
  Matrix.m_DispBuffer[3] = 146;
  Matrix.m_DispBuffer[4] = 243;
  Matrix.m_DispBuffer[5] = 141;
  Matrix.m_DispBuffer[6] = 129;
  Matrix.m_DispBuffer[7] = 128;

  Matrix.m_DispBuffer[8] = 7;
  Matrix.m_DispBuffer[9] = 9;
  Matrix.m_DispBuffer[10] = 9;
  Matrix.m_DispBuffer[11] = 233;
  Matrix.m_DispBuffer[12] = 39;
  Matrix.m_DispBuffer[13] = 33;
  Matrix.m_DispBuffer[14] = 46;
  Matrix.m_DispBuffer[15] = 0;

//  memcpy(Matrix.m_DispBuffer, Matrix.m_DrawBuffer, 16);
  //Matrix.m_DrawBuffer[8] = 0b10000000;
  Matrix.m_Switch = 1;
 
 // start the interrupt to service the UI   
  TCCR1A = 0;
  TCCR1B = 1<<CS11;// | 1<<CS10;
  TIMSK1 = 1<<TOIE1;
  TCNT1H = 0;  
  TCNT1L = 0;  
  Settings.init();  
  Strip.begin();
  Strip.resize(Settings.get(CSettings::GEN_STRIP_LEN));
  switch(Settings.get(CSettings::GEN_GAME)) {
    case GAME_BREAKOUT:
      Game = new CGameBreakout;
      break;
    default:
      Game = new CGamePong;
      break;
  }
}

/*
void refreshMatrix() {
  int i;  

   // clear shift reg
  digitalWrite(P_DS, LOW);
  for(i=0; i<16; ++i) {
    digitalWrite(P_SHCP, LOW);
    digitalWrite(P_SHCP, HIGH);
  }
    
  // clock in single bit
  digitalWrite(P_DS, HIGH);
  digitalWrite(P_SHCP, LOW);
  digitalWrite(P_SHCP, HIGH);
  digitalWrite(P_DS, LOW);
  
  for(i=0; i<16; ++i) {
    // clock to next position
    digitalWrite(P_STCP, LOW);    
    digitalWrite(P_STCP, HIGH);
    
    digitalWrite(P_D0, !!(disp[i]&0x01));
    digitalWrite(P_D1, !!(disp[i]&0x02));
    digitalWrite(P_D2, !!(disp[i]&0x04));
    digitalWrite(P_D3, !!(disp[i]&0x08));
    digitalWrite(P_D4, !!(disp[i]&0x10));
    digitalWrite(P_D5, !!(disp[i]&0x20));
    digitalWrite(P_D6, !!(disp[i]&0x40));
    digitalWrite(P_D7, !!(disp[i]&0x80));
    
    delay(1);
    
    digitalWrite(P_D0,0);
    digitalWrite(P_D1,0);
    digitalWrite(P_D2,0);
    digitalWrite(P_D3,0);
    digitalWrite(P_D4,0);
    digitalWrite(P_D5,0);
    digitalWrite(P_D6,0);
    digitalWrite(P_D7,0);
    
    digitalWrite(P_SHCP, LOW);
    digitalWrite(P_SHCP, HIGH);
    
  }    
}
*/
void loop() 
{
  unsigned long ms = millis();
  Sounds.run(ms);
  if(Menu.run(Strip,ms)) 
  {
    if(Strip.is_transfer_complete()) {
      Menu.render(Strip);
      Strip.begin_transfer();
    }
  }
  else
  {   
    Game->run(Strip,ms);
    if(Strip.is_transfer_complete()) {
      Game->render(Strip);
      Strip.begin_transfer();
    }
  }
}

