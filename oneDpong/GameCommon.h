//////////////////////////////////////////////////////////////////
// PLAYER BAT
class C1DPBat
{
  enum {
    ST_IDLE,
    ST_RISING,
    ST_HOLD,
    ST_FALLING
  } m_state;

  enum {
//    MIN_SIZE = 3,
//    MAX_SIZE = 40,
    RISE_PERIOD = 2,
    FALL_PERIOD = 15
  };

  
  /*
  - Normal bat
- Long bat
- Extra long bat
- Extra long bat can be extended again even if retracting
- Extra long bat can be held extended
- Normal bat, Auto hit
*/
  byte m_minSize;
  byte m_maxSize;
  byte m_flags;
  byte m_size;
  unsigned long m_nextEvent;

public:  
  enum {
    FLAG_REVDIR =        0x01,
    FLAG_HIDDEN =        0x02,  
    FLAG_ALLOW_REEXTEND =  0x04,
    FLAG_ALLOW_HOLD = 0x08
  };
  C1DPBat() {
    init(0,0,0);
  }

  void init(byte minSize, byte maxSize, byte flags) {
    m_minSize = minSize;
    m_maxSize = maxSize;
    m_flags = flags;
    m_state = ST_IDLE;
    m_size = minSize;
    m_nextEvent = 0;
  }
  
  byte hitTest(int pos, int stripLen) {
    if((m_flags&FLAG_REVDIR) && pos < stripLen-m_size)
      return 0;
    if(!(m_flags&FLAG_REVDIR) && pos >= m_size)
      return 0;
    return (m_state == ST_RISING || m_state == ST_HOLD);
  }    
  
  byte reset() {
    m_flags&=~FLAG_HIDDEN;
    m_state = ST_IDLE;
    m_size = m_minSize;
    m_nextEvent = 0;
  }
  
  byte hide() {
    m_flags|=FLAG_HIDDEN;    
  }
  
  void render(APA102& strip) 
  {
    if(!(m_flags&FLAG_HIDDEN)) {    
      int i;
      int stripLen = strip.m_numLeds;
      float q = 255;
      if(m_flags&FLAG_REVDIR) {
        for(i=stripLen-m_size; i<stripLen; ++i) {
          strip.m_data[i] = RGB_LED(0,q,0);
          q *= 0.8;
        }
      }
      else {
        for(i=m_size; i>0; --i) {
          strip.m_data[i] = RGB_LED(0,0,q);
          q *= 0.8;
        }
      }
    }
  }

  void run(unsigned long ms)
  {
    if(m_nextEvent && ms >= m_nextEvent)
    {
      switch(m_state) {
      case ST_RISING:
        if(m_size < m_maxSize) {
          ++m_size;
          m_nextEvent = ms + RISE_PERIOD;
        }
        else if(m_flags & FLAG_ALLOW_HOLD) {
          m_state = ST_HOLD; 
        }
        else {
          m_state = ST_FALLING; 
        }
        break;
      case ST_FALLING:
        if(m_size > m_minSize) {
          --m_size;
          m_nextEvent = ms + FALL_PERIOD;
        }
        else {
          m_state = ST_IDLE;
          m_nextEvent = 0;
        }
        break;          
      }
    }
  }
  
  void onButtonDown() {
    if(m_state == ST_IDLE || 
      ((m_flags & FLAG_ALLOW_REEXTEND) && m_state == ST_FALLING))
    {
        m_state = ST_RISING;
        m_nextEvent = 1;
    }
  }
  
  void onButtonUp() {
    if(m_state == ST_HOLD)
    {
        m_state = ST_FALLING;
        m_nextEvent = 1;
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
  float m_limit;
  float m_divider;
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
  byte m_data[APA102_MAX_LEDS];
public:  
  void render(APA102& strip) {
     for(int i=0; i<strip.m_numLeds; ++i) {
        strip.m_data[i] = RGB_LED(m_data[i],0,0);
     }
  }
  void run(C1DPBall& ball, APA102& strip, unsigned long ms) {
    if(ball.m_pos >= 0 && ball.m_pos < strip.m_numLeds)
      m_data[(int)ball.m_pos] = 0x3F;
    if(!(ms & 0x3F)) {
      for(int i=0; i<strip.m_numLeds; ++i) {
        if(m_data[i])
          m_data[i]/=2;
      }
    }
  }  
};

class C1DPWall {
  byte m_minExt;
  byte m_maxExt;
  LED m_colour;
public:  

  C1DPWall() {
    m_minExt = 0;
    m_maxExt = 0;
  }
  void init(int a, int b, LED c) {
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

class IGame {
  public:
    virtual void run(APA102& strip, unsigned long ms) = 0;
    virtual void render(APA102& strip) = 0;
    virtual void handleEvent(int event) = 0;
};
