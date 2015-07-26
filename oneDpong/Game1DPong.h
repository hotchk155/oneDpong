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
    ST_WAIT_START,
    ST_PLAYER1_SERVE,
    ST_PLAYER2_SERVE,
    ST_IN_PLAY
  };
  CBat m_Player1;  
  CBat m_Player2;  
  CBall m_Ball;
  CTrail m_Trail;
  int m_stripLen;
  byte m_state;
  byte score1;
  byte score2;
  int rallyCount;
public:  
  CGame1DPong(int len) :
    m_stripLen(len),
    m_Player1(1, RGB_LED(0x00, 0xFF, 0x00), P_PLAYER1),
    m_Player2(-1, RGB_LED(0x00, 0x00, 0xFF), P_PLAYER2),
    m_Trail(len)  {
      score1 = 0;
      score2 = 0;
      m_state = ST_PLAYER1_SERVE;
      serve();
    }
    void sound_goal() {
      for(int i=0; i<5; ++i) {
        tone(P_SOUND, 240);
        delay(100);
        tone(P_SOUND, 480);
        delay(100);
      }
      noTone(P_SOUND);
    }
    void serve() 
    {
      rallyCount = 0;
      if(m_state == ST_PLAYER1_SERVE) {
        m_Ball.m_pos = 5;
        m_Ball.m_vel = 0.0;
      }
      else {
        m_Ball.m_pos = m_stripLen - 5;
        m_Ball.m_vel = 0.0;
      }
    }
    void render(APA102& strip) {
      m_Trail.render(strip);
      m_Player1.render(strip, m_stripLen);
      m_Player2.render(strip, m_stripLen);
      m_Ball.render(strip, m_stripLen);
    }
    void run(APA102& strip, unsigned long ms) {
      m_Player1.run(ms);
      m_Player2.run(ms);
      m_Ball.run(m_stripLen, ms);
      m_Trail.run(m_Ball, ms);      

      if(!(ms & 0xFF)) {
        switch(m_state) {
          case ST_PLAYER1_SERVE:
            Display.showScores((ms & 0x100)? score1 : -1, score2);
            break;
          case ST_PLAYER2_SERVE:
            Display.showScores(score1, (ms & 0x100)? score2 : -1);
            break;
          case ST_IN_PLAY:
            Display.showCount(rallyCount);
            break;
        }
      }
  
      if(m_Ball.m_pos < 0) {
        sound_goal();
        score2++;
        m_state = ST_PLAYER2_SERVE;
        serve();
      }
      else if(m_Ball.m_pos >= m_stripLen) {
        sound_goal();
        score1++;
        m_state = ST_PLAYER1_SERVE;
        serve();
      } 
      else if(m_Player1.hitTest((int)m_Ball.m_pos,m_stripLen)) {
        if(m_state == ST_PLAYER1_SERVE) {
          m_state = ST_IN_PLAY;
          m_Ball.m_vel = SERVE_VELOCITY;
        }
        else if(m_state == ST_IN_PLAY && m_Ball.m_vel < 0) {
          // player 1 return
          m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
          rallyCount++;
        }
      }
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),m_stripLen)) {
        if(m_state == ST_PLAYER2_SERVE) {
          // player 2 serve
          m_state = ST_IN_PLAY;
          m_Ball.m_vel = -SERVE_VELOCITY;
        }
        else if(m_state == ST_IN_PLAY  && m_Ball.m_vel > 0) {
          // player 2 return
          m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
          rallyCount++;
        }
      }
    }      
};

