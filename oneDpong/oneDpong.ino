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
#include "PortIO.h"
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
  randomSeed(analogRead(A6));
  
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

byte playerState = 0;
byte player1debounce = 0;
byte player2debounce = 0;
byte lastMillis;
#define PLAYER_SWITCH_DEBOUNCE 20
void loop() 
{
  
  unsigned long ms = millis();
  if((byte)ms != lastMillis) {
    lastMillis = (byte)ms;
    if(player1debounce) --player1debounce;
    if(player2debounce) --player2debounce;
  }
  
  
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
    if(!player1debounce) {
      if(!digitalRead(P_PLAYER1)) {
        if(!(playerState & 1)) {
          Game->handleEvent(EVENT_P1_PRESS);
          playerState |= 1;
          player1debounce = PLAYER_SWITCH_DEBOUNCE;
        }
      }
      else {
        if(playerState & 1) {
          Game->handleEvent(EVENT_P1_RELEASE);
          playerState &= ~1;
          player1debounce = PLAYER_SWITCH_DEBOUNCE;
        }
      }
    }
    
    if(!player2debounce) {
      if(!digitalRead(P_PLAYER2)) {
        if(!(playerState & 2)) {
          Game->handleEvent(EVENT_P2_PRESS);
          playerState |= 2;
          player2debounce = PLAYER_SWITCH_DEBOUNCE;
        }
      }
      else {
        if(playerState & 2) {
          Game->handleEvent(EVENT_P2_RELEASE);
          playerState &= ~2;
          player2debounce = PLAYER_SWITCH_DEBOUNCE;
        }
      }
    }
        
    Game->run(Strip,ms);
    if(Strip.is_transfer_complete()) {
      Game->render(Strip);
      Strip.begin_transfer();
    }
  }
}

