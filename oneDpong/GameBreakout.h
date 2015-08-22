

#define BREAKOUT_SERVE_VELOCITY   0.03
#define BREAKOUT_RETURN_VELOCITY  1.05
#define BREAKOUT_RECYCLE_TIME  500
class CGameBreakout : public IGame
{
  enum {    
    ST_INIT,
    ST_PREP_SERVE,
    ST_WAIT_SERVE,
    ST_IN_PLAY,
    ST_GOAL,
    ST_SHOW_SCORE,    
    ST_COUNT,    
  };
  C1DPBat m_Player1;  
  C1DPBat m_Player2;  
  C1DPBall m_Ball1;
  C1DPBall m_Ball2;
  C1DPWall m_Wall1;
  C1DPWall m_Wall2;


  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  unsigned int m_nextBallTime1;
  unsigned int m_nextBallTime2;
  int m_rallyCount1;
  int m_rallyCount2;
public:  
  CGameBreakout()
  {
    m_state = ST_INIT;
  }
  void handleEvent(int event) {
    switch(event) {
      case EVENT_P1_PRESS:   m_Player1.onButtonDown(); break;
      case EVENT_P1_RELEASE: m_Player1.onButtonUp(); break;
      case EVENT_P2_PRESS:   m_Player2.onButtonDown(); break;
      case EVENT_P2_RELEASE: m_Player2.onButtonUp(); break;
    }    
  }
  
  void state(int s, unsigned int ms) {
    m_state = s;
    m_stateChangeTime = ms;
  }
  void nextBall1(int stripLen) {
     m_Ball1.m_pos = 4;
     m_Ball1.m_vel = 0.0;
  }
  void nextBall2(int stripLen) {
     m_Ball2.m_pos = stripLen - 6;
     m_Ball2.m_vel = 0.0;
  }
  void render(APA102& strip) {
    strip.cls();
    m_Wall1.render(strip);
    m_Wall2.render(strip);
    m_Player1.render(strip);
    m_Player2.render(strip);
    m_Ball1.render(strip);
    m_Ball2.render(strip);
  }
    
  void run(APA102& strip, unsigned long ms) {

    // Run the various game components
    m_Player1.run(ms);
    m_Player2.run(ms);
    m_Ball1.run(strip, ms);
    m_Ball2.run(strip, ms);

      // process game state
     switch(m_state) {
     ////////////////////////////////////////////////////////////////
     // NEW GAME
     case ST_INIT:
       {
         int len = strip.m_numLeds/6;       
         int mid = strip.m_numLeds/2;       
         m_Wall1.init(mid-len, mid, RGB_LED(0x00, 0x3f, 0x00));
         m_Wall2.init(mid+1, mid+len+1, RGB_LED(0x00, 0x00, 0x3f));
         nextBall1(strip.m_numLeds);
         nextBall2(strip.m_numLeds);
         m_Player1.init(3, 25, 0);
         m_Player2.init(3, 25, C1DPBat::FLAG_REVDIR);
         m_rallyCount1 = 0;
         m_rallyCount2 = 0;
         state(ST_IN_PLAY, ms);
      }
     break;
     
     ////////////////////////////////////////////////////////////////
     // IN PLAY
     case ST_IN_PLAY:
     
     if(m_Ball1.m_pos < 0) {
       if(m_Ball1.m_vel < 0) {
         Sounds.play(CSounds::S_GOAL);
         m_Ball1.m_vel = 0;         
         m_nextBallTime1 = ms + BREAKOUT_RECYCLE_TIME;
       }
       else if(ms >= m_nextBallTime1) {
         nextBall1(strip.m_numLeds);
       }
     }
     else if(m_Player1.hitTest((int)m_Ball1.m_pos,strip.m_numLeds)) {
        if(m_Ball1.m_vel < 0) {
          // return
         m_Ball1.m_vel *= -BREAKOUT_RETURN_VELOCITY;
        }
        else if(abs(m_Ball1.m_vel)<0.001) {
          // serve
         m_Ball1.m_vel = BREAKOUT_SERVE_VELOCITY;
        }
     }
     else if(m_Wall1.hitTestMin(m_Ball1.m_pos) && m_Ball1.m_vel > 0) {      
       
       if(m_Wall1.removeBrick(true)) {
         Sounds.play(CSounds::S_HIT, 100-m_Wall1.numBricks());
         m_Ball1.m_vel *= -BREAKOUT_RETURN_VELOCITY;
       }
     }
     

     if(m_Ball2.m_pos >= strip.m_numLeds) {
       if(m_Ball2.m_vel > 0) {
         Sounds.play(CSounds::S_GOAL);
         m_Ball2.m_vel = 0;         
         m_nextBallTime2 = ms + BREAKOUT_RECYCLE_TIME;
       }
       else if(ms >= m_nextBallTime2) {
         nextBall2(strip.m_numLeds);
       }
     }
     else if(m_Player2.hitTest((int)m_Ball2.m_pos,strip.m_numLeds)) {
        if(m_Ball2.m_vel > 0) {
          // return
         m_Ball2.m_vel *= -BREAKOUT_RETURN_VELOCITY;
        }
        else if(abs(m_Ball2.m_vel)<0.001) {
          // serve
         m_Ball2.m_vel = BREAKOUT_SERVE_VELOCITY;
        }
     }
     else if(m_Wall2.hitTestMax(m_Ball2.m_pos) && m_Ball2.m_vel < 0) {      
       
       if(m_Wall2.removeBrick(false)) {
         Sounds.play(CSounds::S_HIT, 100-m_Wall2.numBricks());
         m_Ball2.m_vel *= -BREAKOUT_RETURN_VELOCITY;
       }
     }
     break;
     ////////////////////////////////////////////////////////////////         
     }     
  } 
};


