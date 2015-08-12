class CSounds {
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
  
  CSounds() {
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

class CBat
{
  enum {
    ST_IDLE,
    ST_RISING,
    ST_FALLING
  } m_state;
  
  enum {
    MIN_SIZE = 3,
    MAX_SIZE = 40,
    RISE_PERIOD = 2,
    FALL_PERIOD = 15
  };
  
  char m_dir;
  byte m_pin;
  byte m_size;
  unsigned long m_nextEvent;
  LED m_colour;
public:  
  CBat(byte dir, LED colour, byte pin) {
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
      
  void render(APA102& strip, int stripLen) 
  {
    float q = 255;
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


class CBall
{
public:  
  int m_bri;
  float m_pos;
  float m_vel;
  unsigned long m_ms;  
  CBall() {
    m_pos = 0;
    m_vel = 0.06;
    m_ms = 0;
    m_bri = 0;
  }
  void render(APA102& strip, int stripLen) {
    if(m_pos >= 0 && m_pos < stripLen-2) {
        byte b = m_bri >> 4;
        strip.m_data[(int)m_pos] = RGB_LED(b, b, b);
        strip.m_data[(int)m_pos+1] = RGB_LED(b, b, b);
        strip.m_data[(int)m_pos+2] = RGB_LED(b, b, b);
    }
  }
  void run(int stripLen, unsigned long ms) {
    if(m_ms != ms) {
      m_bri++;
      m_pos += m_vel;
    }
  }  
};

class CTrail 
{
  byte *m_data;
  int m_len;
public:  
  CTrail(int stripLen) {
    m_len = stripLen;
    m_data = new byte[m_len];
    memset(m_data, 0, m_len);    
  }
  void render(APA102& strip) {
      for(int i=0; i<m_len; ++i) {
        strip.m_data[i] = RGB_LED(m_data[i],0,0);
      }
  }
  void run(CBall& ball, unsigned long ms) {
    if(ball.m_pos >= 0 && ball.m_pos < m_len)
      m_data[(int)ball.m_pos] = 0x3F;
    if(!(ms & 0x3F)) {
      for(int i=0; i<m_len; ++i) {
        if(m_data[i])
          m_data[i]/=2;
      }
    }
  }  
  
};

#define SERVE_VELOCITY 0.05
#define RETURN_VELOCITY_MULTIPLIER 1.05

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
  CBat m_Player1;  
  CBat m_Player2;  
  CBall m_Ball;
  CTrail m_Trail;
  CSounds m_Sounds;
  int m_stripLen;
  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  byte m_score1;
  byte m_score2;
  int m_rallyCount;
  byte m_playerToServe;
  
public:  
  CGame1DPong(int len) :
    m_stripLen(len),
    m_Player1(1, RGB_LED(0x00, 0xFF, 0x00), P_PLAYER1),
    m_Player2(-1, RGB_LED(0x00, 0x00, 0xFF), P_PLAYER2),
    m_Trail(len)  {
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
      m_Player1.render(strip, m_stripLen);
      m_Player2.render(strip, m_stripLen);
      m_Ball.render(strip, m_stripLen);
    }
    void run(APA102& strip, unsigned long ms) {
      
      // Run the various game components
      m_Player1.run(ms);
      m_Player2.run(ms);
      m_Ball.run(m_stripLen, ms);
      m_Trail.run(m_Ball, ms);      
      m_Sounds.run(ms);

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
          m_Ball.m_pos = m_stripLen - 5;
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
          } else {
            Display.showScores(m_score1, (ms & 0x100)? m_score2 : -1);
          }
        }        
        if(m_Player1.hitTest((int)m_Ball.m_pos,m_stripLen)) {
            state(ST_IN_PLAY, ms);
            m_Ball.m_vel = SERVE_VELOCITY;
            m_Sounds.play(CSounds::S_SERVE);
        }
        else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),m_stripLen)) {
            state(ST_IN_PLAY, ms);
            m_Ball.m_vel = -SERVE_VELOCITY;          
            m_Sounds.play(CSounds::S_SERVE);
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
          m_Sounds.play(CSounds::S_GOAL);
          state(ST_GOAL, ms);
        }
        // Player 2 misses, player 1 scores        
        else if(m_Ball.m_pos >= m_stripLen) {
          m_playerToServe = PLAYER1;
          m_Sounds.play(CSounds::S_GOAL);
          state(ST_GOAL, ms);          
        } 
        // Player 1 returns
        else if(m_Player1.hitTest((int)m_Ball.m_pos,m_stripLen)  && m_Ball.m_vel < 0) {
          m_Sounds.play(CSounds::S_HIT, ++m_rallyCount);
          m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
        }
        // Player 2 returns
        else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),m_stripLen) && m_Ball.m_vel > 0) {
            // player 2 return
          m_Sounds.play(CSounds::S_HIT, ++m_rallyCount);
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
        m_Sounds.play(CSounds::S_COUNT);        
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

