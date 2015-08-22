/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////


//#define SERVE_VELOCITY 0.05
#define RETURN_VELOCITY_MULTIPLIER 1.05
#define BAT_MIN_SIZE 3
#define BAT_MAX_SIZE 40

class CGamePong : public IGame
{
  enum {
    PLAYER1,
    PLAYER2
  };

  enum {    
    ST_INIT,
    ST_PREP_SERVE,
    ST_WAIT_SERVE,
    ST_IN_PLAY,
    ST_GOAL,
    ST_SHOW_SCORE,    
    ST_COUNT,    
  };
  
  enum {
    SIMP_AUTORETURN = 0x80,
    SIMP_REEXTEND   = 0x40,
    SIMP_HOLD       = 0x20
  };
  
  C1DPBat m_Player1;  
  C1DPBat m_Player2;  
  C1DPBall m_Ball;
  C1DPTrail m_Trail;
  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  byte m_score1;
  byte m_score2;
  int m_rallyCount;
  byte m_playerToServe;
  float m_velocity;

  byte m_initialSpeed;  
  byte m_acceleration;  
  byte m_maxSpeed;  
  byte m_p1simplify;
  byte m_p2simplify;
public:  
  CGamePong() {
    m_score1 = 0;
    m_score2 = 0;
    m_state = ST_INIT;
    m_stateChangeTime = 0;
    m_playerToServe = PLAYER1;
  }
  float getServeVelocity() {    
    switch(m_initialSpeed) {
      case 1: return 0.01;
      case 2: return 0.02;
      case 3: return 0.03;
      case 4: return 0.04;
      case 5: return 0.05;
      case 6: return 0.06;
      case 7: return 0.07;
      case 8: return 0.1;
      case 9: return 0.2;
      default: return 0.05;
    }
  }
  float getAcceleration() {
    switch(m_acceleration) {
      case 1: return 1.0;
      case 2: return 1.01;
      case 3: return 1.02;
      case 4: return 1.03;
      case 5: return 1.05;
      case 6: return 1.06;
      case 7: return 1.07;
      case 8: return 1.08;
      case 9: return 1.1;
      default: return 1.05;
    }
  }
  float getMaxVelocity() {    
    switch(m_maxSpeed) {
      case 1: return 0.02;
      case 2: return 0.04;
      case 3: return 0.06;
      case 4: return 0.08;
      case 5: return 0.1;
      case 6: return 0.2;
      case 7: return 0.3;
      case 8: return 0.5;
      case 9: return 999;
      default: return 0.05;
    }
  }
  float getAccelerated(float v)  {
    float m = getMaxVelocity();
    v *= -getAcceleration();
    if(v>m) return m;
    if(v<-m) return-m;
    return v;
  }
  float getVelocityForPlayer(float v, byte player) {
    if(player == PLAYER1) {
      if(m_p1simplify & 0xF) {
        return (v * (m_p1simplify & 0xF))/10.0;
      }
    }
    else {
      if(m_p2simplify & 0xF) {
        return (v * (m_p2simplify & 0xF))/10.0;
      }
    }
    return v;
  }
  byte getSimplify(byte level) {
    switch(level) {
      case 1: return 7; 
      case 2: return 6; 
      case 3: return 5; 
      case 4: return 4 | SIMP_REEXTEND;
      case 5: return 3 | SIMP_REEXTEND;
      case 6: return 2 | SIMP_REEXTEND;
      case 7: return 1 | SIMP_REEXTEND;
      case 8: return SIMP_HOLD|SIMP_REEXTEND;
      case 9: return SIMP_AUTORETURN|SIMP_REEXTEND;
    }
    return 0;
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
  void handleEvent(int event) {
    switch(event) {
      case EVENT_P1_PRESS:   m_Player1.onButtonDown(); break;
      case EVENT_P1_RELEASE: m_Player1.onButtonUp(); break;
      case EVENT_P2_PRESS:   m_Player2.onButtonDown(); break;
      case EVENT_P2_RELEASE: m_Player2.onButtonUp(); break;
    }    
  }
  void run(APA102& strip, unsigned long ms) {

    // Run the various game components
    m_Player1.run(ms);
    m_Player2.run(ms);
    m_Ball.run(strip, ms);
    m_Trail.run(m_Ball,strip,ms);

    // process game state
    switch(m_state) {
      ////////////////////////////////////////////////////////////////
      // NEW GAME
    case ST_INIT:
      m_playerToServe = PLAYER1;
      m_initialSpeed = Settings.get(CSettings::PONG_INITSPEED);
      m_acceleration = Settings.get(CSettings::PONG_ACCELERATION);
      m_maxSpeed = Settings.get(CSettings::PONG_MAXSPEED);
      m_p1simplify = getSimplify(Settings.get(CSettings::PONG_SIMPLIFY1));
      m_p2simplify = getSimplify(Settings.get(CSettings::PONG_SIMPLIFY2));        
      m_Player1.init(3, 40, 
        ((m_p1simplify&SIMP_REEXTEND)?C1DPBat::FLAG_ALLOW_REEXTEND:0) |
        ((m_p1simplify&SIMP_HOLD)?C1DPBat::FLAG_ALLOW_HOLD:0) 
      );
      m_Player2.init(3, 40, C1DPBat::FLAG_REVDIR |
        ((m_p2simplify&SIMP_REEXTEND)?C1DPBat::FLAG_ALLOW_REEXTEND:0) |
        ((m_p2simplify&SIMP_HOLD)?C1DPBat::FLAG_ALLOW_HOLD:0) 
      );
      state(ST_PREP_SERVE, ms);
      break;

      ////////////////////////////////////////////////////////////////
      // PREPARE FOR A PLAYER TO SERVE
    case ST_PREP_SERVE:
      m_Player1.reset();
      m_Player2.reset();
      m_rallyCount = 0;
      if(m_playerToServe == PLAYER1) {
        m_Ball.m_pos = 4;
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
        m_velocity = getServeVelocity();
        m_Ball.m_vel = getVelocityForPlayer(m_velocity, PLAYER2);
        Sounds.play(CSounds::S_SERVE);
      }
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds)) {
        state(ST_IN_PLAY, ms);
        m_velocity = -getServeVelocity();          
        m_Ball.m_vel = getVelocityForPlayer(m_velocity, PLAYER1);
        Sounds.play(CSounds::S_SERVE);
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

      if((m_p1simplify & SIMP_AUTORETURN) &&  m_Ball.m_vel < 0 && m_Ball.m_pos < 20) {
        m_Player1.onButtonDown(); 
      }
      if((m_p2simplify & SIMP_AUTORETURN) &&  m_Ball.m_vel > 0 && m_Ball.m_pos > strip.m_numLeds-20) {
        m_Player2.onButtonDown(); 
      }

      // Player 1 misses, player 2 scores        
      if(m_Ball.m_pos < 0) {
        m_playerToServe = PLAYER2;
        Sounds.play(CSounds::S_GOAL);
        m_Player1.hide();
        state(ST_GOAL, ms);
      }
      // Player 2 misses, player 1 scores        
      else if(m_Ball.m_pos >= strip.m_numLeds) {
        m_playerToServe = PLAYER1;
        Sounds.play(CSounds::S_GOAL);
        m_Player2.hide();
        state(ST_GOAL, ms);          
      } 
      // Player 1 returns
      else if(m_Player1.hitTest((int)m_Ball.m_pos,strip.m_numLeds)  && m_Ball.m_vel < 0) {
        Sounds.play(CSounds::S_HIT, ++m_rallyCount);
        m_velocity = getAccelerated(m_velocity);
        m_Ball.m_vel = getVelocityForPlayer(m_velocity, PLAYER2);        
      }
      // Player 2 returns
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds) && m_Ball.m_vel > 0) {
        // player 2 return
        Sounds.play(CSounds::S_HIT, ++m_rallyCount);
        m_velocity = getAccelerated(m_velocity);
        m_Ball.m_vel = getVelocityForPlayer(m_velocity, PLAYER1);
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
      Sounds.play(CSounds::S_COUNT);        
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


