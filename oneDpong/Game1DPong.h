//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//
//
// ONE DIMENSIONAL PONG GAME
//
//
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
// SOUND EFFECTS
class C1DPSounds {
  unsigned long m_nextEvent;
  byte m_sound;
  char m_count;
  int m_param;
public:
  enum {
    S_NONE,
    S_COUNT,
    S_HIT,
    S_SERVE,
    S_GOAL
  };

  C1DPSounds() {
    m_nextEvent = 0;
    m_sound = S_NONE;
    m_count = 0;
    m_param = 0;
  }
  void play(int sound, int param = 0) {
    m_sound = sound;
    m_count = 0;
    m_param = param;
    m_nextEvent = 1;    
  }
  void run(unsigned long ms) {
    if(m_nextEvent && ms >= m_nextEvent)  {
      switch(m_sound) {
      case S_NONE:
        noTone(P_SOUND);
        m_nextEvent = 0;         
        break;
      case S_COUNT:
        if(m_count == 0) {
          tone(P_SOUND, 500);
          m_nextEvent = ms + 100;
          m_count = 1;
        }
        else if(m_count == 1) {
          tone(P_SOUND, 1000);
          m_nextEvent = ms + 100;
          m_count = 2;
        }
        else {
          m_sound = S_NONE;
        }
        break;
      case S_HIT:
        tone(P_SOUND, 1000 + 20 * m_param);
        m_nextEvent = ms + 10;
        m_sound = S_NONE;
        break;
      case S_SERVE:
        tone(P_SOUND, 1000);
        m_nextEvent = ms + 10;
        m_sound = S_NONE;
        break;
      case S_GOAL:         
        if(m_count & 1){
          tone(P_SOUND, 240 + 50 * m_count);
          m_nextEvent = ms + 10;
        }
        else {
          tone(P_SOUND, 2*(240 + 50 * m_count));
          m_nextEvent = ms + 10;
        }
        if(++m_count > 10) {
          m_sound = S_NONE;
        }
        break;

      } 
    }

  }

};

//////////////////////////////////////////////////////////////////
// PLAYER BAT
template<int MIN_SIZE, int MAX_SIZE>
class C1DPBat
{
  enum {
    ST_IDLE,
    ST_RISING,
    ST_FALLING
  } 
  m_state;

  enum {
//    MIN_SIZE = 3,
//    MAX_SIZE = 40,
    RISE_PERIOD = 2,
    FALL_PERIOD = 15
  };

  char m_dir;
  byte m_pin;
  byte m_size;
  unsigned long m_nextEvent;
  LED m_colour;
public:  
  C1DPBat(byte dir, LED colour, byte pin) {
    m_colour = colour;
    m_state = ST_IDLE;
    m_nextEvent = 0;
    m_size = MIN_SIZE;
    m_dir = dir;
    m_pin = pin;
  }

  byte hitTest(int pos, int stripLen) {
    if(m_dir < 0 && pos < stripLen-m_size)
      return 0;
    if(m_dir > 0 && pos >= m_size)
      return 0;
    return (m_state == ST_RISING);
  }    

  void render(APA102& strip) 
  {
    float q = 255;
    int stripLen = strip.m_numLeds;
    if(m_dir < 0) {
      for(int i=stripLen-m_size; i<stripLen; ++i) {
        strip.m_data[i] = RGB_LED(0,q,0);
        q *= 0.8;
      }
    }
    else {
      for(int i=m_size-1; i>0; --i) {
        strip.m_data[i] = RGB_LED(0,0,q);
        q *= 0.8;
      }
    }
  }

  void run(unsigned long ms)
  {
    if(m_nextEvent && ms >= m_nextEvent)
    {
      switch(m_state) {
      case ST_RISING:
        if(m_size < MAX_SIZE) {
          ++m_size;
          m_nextEvent = ms + RISE_PERIOD;
          break;
        }
        m_state = ST_FALLING; // drop thru
      case ST_FALLING:
        if(m_size > MIN_SIZE) {
          --m_size;
          m_nextEvent = ms + FALL_PERIOD;
          break;
        }
        m_state = ST_IDLE;
        m_nextEvent = 0;
        break;          
      }
    }
    else if(m_state == ST_IDLE)
    {
      if(!digitalRead(m_pin)) {
        m_state = ST_RISING;
        m_nextEvent = ms + RISE_PERIOD;
      }
    }    
  }
};

//////////////////////////////////////////////////////////////////
// BALL
class C1DPBall
{
public:  
  int m_bri;
  float m_pos;
  float m_vel;
  unsigned long m_ms;  
  C1DPBall() {
    m_pos = 0;
    m_vel = 0.06;
    m_ms = 0;
    m_bri = 0;
  }
  void render(APA102& strip) {
    if(m_pos >= 0 && m_pos < strip.m_numLeds-2) {
      byte b = m_bri >> 4;
      strip.m_data[(int)m_pos] = RGB_LED(b, b, b);
      strip.m_data[(int)m_pos+1] = RGB_LED(b, b, b);
      strip.m_data[(int)m_pos+2] = RGB_LED(b, b, b);
    }
  }
  void run(APA102& strip, unsigned long ms) {
    if(m_ms != ms) {
      m_bri++;
      m_pos += m_vel;
    }
  }  
};

//////////////////////////////////////////////////////////////////
// TRAIL
class C1DPTrail 
{
  int m_len;
  byte *m_data;
public:  
  C1DPTrail() {
    m_len = 0;
    m_data = 0;
  }
  ~C1DPTrail() {
    if(m_data) {
      delete[] m_data;
    }    
  }
  void render(APA102& strip) {
    if(m_data) {
      for(int i=0; i<strip.m_numLeds; ++i) {
        strip.m_data[i] = RGB_LED(m_data[i],0,0);
      }
    }
  }
  void run(C1DPBall& ball, APA102& strip, unsigned long ms) {
    if(!m_data || strip.m_numLeds != m_len) {
      // need to be able to dynamically resize since user
      // can change the length of the strip
      m_len = strip.m_numLeds;
      m_data = new byte[m_len];
      memset(m_data, 0, m_len);    
    }    
    if(m_data) {
      if(ball.m_pos >= 0 && ball.m_pos < m_len)
        m_data[(int)ball.m_pos] = 0x3F;
      if(!(ms & 0x3F)) {
        for(int i=0; i<m_len; ++i) {
          if(m_data[i])
            m_data[i]/=2;
        }
      }
    }
  }  
};

#define SERVE_VELOCITY 0.05
#define RETURN_VELOCITY_MULTIPLIER 1.05
#define BAT_MIN_SIZE 3
#define BAT_MAX_SIZE 40

class CGame1DPong 
{
  enum {
    PLAYER1,
    PLAYER2
  };

  /*
        PLAYERxSERVE
   
   */
  enum {    
    ST_INIT,
    ST_PREP_SERVE,
    ST_WAIT_SERVE,
    ST_IN_PLAY,
    ST_GOAL,
    ST_SHOW_SCORE,    
    ST_COUNT,    
  };
  C1DPBat<BAT_MIN_SIZE,  BAT_MIN_SIZE> m_Player1;  
  C1DPBat<BAT_MIN_SIZE,  BAT_MIN_SIZE> m_Player2;  
  C1DPBall m_Ball;
  C1DPTrail m_Trail;
  C1DPSounds m_Sounds;
  //  int m_stripLen;
  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  byte m_score1;
  byte m_score2;
  int m_rallyCount;
  byte m_playerToServe;

public:  
  CGame1DPong() :
  m_Player1(1, RGB_LED(0x00, 0xFF, 0x00), P_PLAYER1),
  m_Player2(-1, RGB_LED(0x00, 0x00, 0xFF), P_PLAYER2),
  m_Trail()  {
    m_score1 = 0;
    m_score2 = 0;
    m_state = ST_INIT;
    m_stateChangeTime = 0;
    m_playerToServe = PLAYER1;
  }
  void state(int s, unsigned int ms) {
    m_state = s;
    m_stateChangeTime = ms;
  }
  void render(APA102& strip) {
    m_Trail.render(strip);
    m_Player1.render(strip);
    m_Player2.render(strip);
    m_Ball.render(strip);
  }
  void run(APA102& strip, unsigned long ms) {

    // Run the various game components
    m_Player1.run(ms);
    m_Player2.run(ms);
    m_Ball.run(strip, ms);
    m_Sounds.run(ms);
    m_Trail.run(m_Ball,strip,ms);

    // process game state
    switch(m_state) {
      ////////////////////////////////////////////////////////////////
      // NEW GAME
    case ST_INIT:
      m_playerToServe = PLAYER1;
      state(ST_PREP_SERVE, ms);
      break;

      ////////////////////////////////////////////////////////////////
      // PREPARE FOR A PLAYER TO SERVE
    case ST_PREP_SERVE:
      m_rallyCount = 0;
      if(m_playerToServe == PLAYER1) {
        m_Ball.m_pos = 5;
        m_Ball.m_vel = 0.0;
      }
      else {
        m_Ball.m_pos = strip.m_numLeds - 5;
        m_Ball.m_vel = 0.0;
      }
      state(ST_WAIT_SERVE, ms);
      break;

      ////////////////////////////////////////////////////////////////
      // WAITING FOR SERVE
    case ST_WAIT_SERVE:
      // Display shows score with serving player score blinking        
      if(!(ms & 0xFF)) {
        if(m_playerToServe == PLAYER1) {
          Display.showScores((ms & 0x100)? m_score1 : -1, m_score2);
        } 
        else {
          Display.showScores(m_score1, (ms & 0x100)? m_score2 : -1);
        }
      }        
      if(m_Player1.hitTest((int)m_Ball.m_pos,strip.m_numLeds)) {
        state(ST_IN_PLAY, ms);
        m_Ball.m_vel = SERVE_VELOCITY;
        m_Sounds.play(C1DPSounds::S_SERVE);
      }
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds)) {
        state(ST_IN_PLAY, ms);
        m_Ball.m_vel = -SERVE_VELOCITY;          
        m_Sounds.play(C1DPSounds::S_SERVE);
      }
      break;

      ////////////////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////////////
      // IN PLAY
    case ST_IN_PLAY:
      // display shows rally count
      if(!(ms & 0xFF)) {
        Display.showCount(m_rallyCount);
      }

      // Player 1 misses, player 2 scores        
      if(m_Ball.m_pos < 0) {
        m_playerToServe = PLAYER2;
        m_Sounds.play(C1DPSounds::S_GOAL);
        state(ST_GOAL, ms);
      }
      // Player 2 misses, player 1 scores        
      else if(m_Ball.m_pos >= strip.m_numLeds) {
        m_playerToServe = PLAYER1;
        m_Sounds.play(C1DPSounds::S_GOAL);
        state(ST_GOAL, ms);          
      } 
      // Player 1 returns
      else if(m_Player1.hitTest((int)m_Ball.m_pos,strip.m_numLeds)  && m_Ball.m_vel < 0) {
        m_Sounds.play(C1DPSounds::S_HIT, ++m_rallyCount);
        m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
      }
      // Player 2 returns
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds) && m_Ball.m_vel > 0) {
        // player 2 return
        m_Sounds.play(C1DPSounds::S_HIT, ++m_rallyCount);
        m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
      }
      break;
      ////////////////////////////////////////////////////////////////

      ////////////////////////////////////////////////////////////////
      // GOAL SCORED
    case ST_GOAL:
      // display shows rally count
      if(!(ms & 0xF)) {
        Display.showCount(m_rallyCount);
      }
      if(ms >= m_stateChangeTime + 2000) {
        Display.showScores(m_score1, m_score2);
        state(ST_SHOW_SCORE, ms);
      }
      break;

      ////////////////////////////////////////////////////////////////
      // SHOWING PREVIOUS SCORE BEFORE UPDATING
    case ST_SHOW_SCORE:
      if(ms >= m_stateChangeTime + 500) {
        state(ST_COUNT, ms);
      }
      break;

      ////////////////////////////////////////////////////////////////
      // SHOWING PREVIOUS SCORE BEFORE UPDATING
    case ST_COUNT:
      m_Sounds.play(C1DPSounds::S_COUNT);        
      if(m_playerToServe == PLAYER1) {
        ++m_score1;        
      }
      else {
        ++m_score2;        
      }
      Display.showScores(m_score1, m_score2);
      state(ST_PREP_SERVE, ms);
      break;

    }     
  } 
};


class C1DPWall {
  byte m_minExt;
  byte m_maxExt;
  LED m_colour;
public:  

  C1DPWall(byte a, byte b, LED c) {
    m_minExt = a;
    m_maxExt = b;
    m_colour = c;
  }
  void render(APA102& strip) {
    for(int i=m_minExt; i<=m_maxExt; ++i) {
      strip.m_data[i] = m_colour;      
    }
  }
  byte hitTestMin(int i) {
    return (m_minExt<=m_maxExt ) && (i>=m_minExt);
  }
  byte hitTestMax(int i) {
    return (m_minExt<=m_maxExt ) && (i<=m_maxExt);
  }
  int numBricks() {
    return m_maxExt - m_minExt;
  }
  byte  removeBrick(byte fromMin) {
    if(m_minExt <= m_maxExt) {
      if(fromMin) {
        m_minExt++;
      }
      else {
        m_maxExt--;
      }
      return 1;
    }
    return 0;
  }
};


#define BREAKOUT_SERVE_VELOCITY   0.03
#define BREAKOUT_RETURN_VELOCITY  1.05
#define BREAKOUT_RECYCLE_TIME  500
class CGame1DBreakout
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
  C1DPBat<3,25> m_Player1;  
  C1DPBat<3,25> m_Player2;  
  C1DPBall m_Ball1;
  C1DPBall m_Ball2;
  C1DPWall m_Wall1;
  C1DPWall m_Wall2;
  C1DPSounds m_Sounds;


  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  unsigned int m_nextBallTime1;
  unsigned int m_nextBallTime2;
  int m_rallyCount1;
  int m_rallyCount2;
public:  
  CGame1DBreakout() :
  m_Player1(1, RGB_LED(0x00, 0xFF, 0x00), P_PLAYER1),
  m_Player2(-1, RGB_LED(0x00, 0x00, 0xFF), P_PLAYER2),
  m_Wall1(80, 120, RGB_LED(0x00, 0x3f, 0x00)),
  m_Wall2(121, 161, RGB_LED(0x00, 0x00, 0x3f))
  {
    m_state = ST_INIT;
  }
  void state(int s, unsigned int ms) {
    m_state = s;
    m_stateChangeTime = ms;
  }
  void nextBall1(int stripLen) {
     m_Ball1.m_pos = 5;
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
    m_Sounds.run(ms);

      // process game state
     switch(m_state) {
     ////////////////////////////////////////////////////////////////
     // NEW GAME
     case ST_INIT:
       nextBall1(strip.m_numLeds);
       nextBall2(strip.m_numLeds);
      m_rallyCount1 = 0;
      m_rallyCount2 = 0;
     state(ST_IN_PLAY, ms);
     break;
     
     ////////////////////////////////////////////////////////////////
     // IN PLAY
     case ST_IN_PLAY:
     
     if(m_Ball1.m_pos < 0) {
       if(m_Ball1.m_vel < 0) {
         m_Sounds.play(C1DPSounds::S_GOAL);
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
         m_Sounds.play(C1DPSounds::S_HIT, 100-m_Wall1.numBricks());
         m_Ball1.m_vel *= -BREAKOUT_RETURN_VELOCITY;
       }
     }
     

     if(m_Ball2.m_pos >= strip.m_numLeds) {
       if(m_Ball2.m_vel > 0) {
         m_Sounds.play(C1DPSounds::S_GOAL);
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
         m_Sounds.play(C1DPSounds::S_HIT, 100-m_Wall2.numBricks());
         m_Ball2.m_vel *= -BREAKOUT_RETURN_VELOCITY;
       }
     }


     break;
     ////////////////////////////////////////////////////////////////
     
     
     }     
  } 
};


