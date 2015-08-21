//////////////////////////////////////////////////////////////////
// SOUND EFFECTS
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
    S_GOAL,
    S_TICK
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
      case S_TICK:
        tone(P_SOUND, 2000);
        m_nextEvent = ms + 10;
        m_sound = S_NONE;
        break;
      } 
    }
  }
};
extern CSounds Sounds;
