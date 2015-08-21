typedef struct {
  int pos;
  byte brightness;
  byte velocity;  
} PARTICLE;

#define EXPLODE_PARTICLES 5
//////////////////////////////////////////////////////////////////
// PLAYER BAT
template<int MIN_SIZE, int MAX_SIZE>
class C1DPBat
{
  enum {
    ST_IDLE,
    ST_RISING,
    ST_FALLING,
    ST_EXPLODE,
    ST_DEAD
  } 
  m_state;

  enum {
//    MIN_SIZE = 3,
//    MAX_SIZE = 40,
    RISE_PERIOD = 2,
    FALL_PERIOD = 15,
    EXPLODE_PERIOD = 20
  };

  char m_dir;
  byte m_pin;
  byte m_size;
  unsigned long m_nextEvent;
  LED m_colour;
  PARTICLE explosion[EXPLODE_PARTICLES];

public:  
  C1DPBat(byte dir, LED colour, byte pin) {
    m_colour = colour;
    m_dir = dir;
    m_pin = pin;
    reset();
  }

  byte hitTest(int pos, int stripLen) {
    if(m_dir < 0 && pos < stripLen-m_size)
      return 0;
    if(m_dir > 0 && pos >= m_size)
      return 0;
    return (m_state == ST_RISING);
  }    

  void explode() 
  {
    for(int i=0; i<EXPLODE_PARTICLES; ++i)
    {
      explosion[i].pos = i<<6;
      explosion[i].brightness = 50+random(200);
      explosion[i].velocity =  random(50);
    }
    m_size = 0;
    m_nextEvent = 1;
    m_state = ST_EXPLODE;
  }

  void reset() 
  {
    m_state = ST_IDLE;
    m_nextEvent = 0;
    m_size = MIN_SIZE;
  }
  
  void render(APA102& strip) 
  {
    int i;
    int stripLen = strip.m_numLeds;
    switch(m_state) {
     case ST_DEAD:
       break;
     case ST_EXPLODE:
         for(int i=0; i<EXPLODE_PARTICLES; ++i)
         {
           int pos = explosion[i].pos >> 6;
           if(pos < stripLen) {
              if(m_dir < 0) {
                strip.m_data[stripLen-pos] = RGB_LED(0,explosion[i].brightness,0);
              }
              else {
                strip.m_data[pos] = RGB_LED(0,0,explosion[i].brightness);
              }
            }
         }
        break;
    default:        
      {
        float q = 255;
        if(m_dir < 0) {
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
        break;
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
      case ST_EXPLODE:
        if(++m_size > 60) {
          m_state = ST_DEAD;
          m_nextEvent = 0;
        }
        else {
         for(int i=0; i<EXPLODE_PARTICLES; ++i) {
           if(!(m_size%8))
           explosion[i].brightness/=2;
           explosion[i].pos += explosion[i].velocity;          
         }         
         m_nextEvent = ms + EXPLODE_PERIOD;            
        }
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

  C1DPWall(LED c) {
    m_minExt = 0;
    m_maxExt = 0;
    m_colour = c;
  }
  void init(int a, int b) {
    m_minExt = a;
    m_maxExt = b;    
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
};
