/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////


#define SERVE_VELOCITY 0.05
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
  C1DPBat<BAT_MIN_SIZE,  BAT_MAX_SIZE> m_Player1;  
  C1DPBat<BAT_MIN_SIZE,  BAT_MAX_SIZE> m_Player2;  
  C1DPBall m_Ball;
  C1DPTrail m_Trail;
  byte m_state;
  byte m_nextState;
  unsigned int m_stateChangeTime;
  byte m_score1;
  byte m_score2;
  int m_rallyCount;
  byte m_playerToServe;

public:  
  CGamePong() :
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
    Sounds.run(ms);
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
        m_Ball.m_vel = SERVE_VELOCITY;
        Sounds.play(CSounds::S_SERVE);
      }
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds)) {
        state(ST_IN_PLAY, ms);
        m_Ball.m_vel = -SERVE_VELOCITY;          
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

      // Player 1 misses, player 2 scores        
      if(m_Ball.m_pos < 0) {
        m_playerToServe = PLAYER2;
        Sounds.play(CSounds::S_GOAL);
        m_Player1.explode();
        state(ST_GOAL, ms);
      }
      // Player 2 misses, player 1 scores        
      else if(m_Ball.m_pos >= strip.m_numLeds) {
        m_playerToServe = PLAYER1;
        Sounds.play(CSounds::S_GOAL);
        m_Player2.explode();
        state(ST_GOAL, ms);          
      } 
      // Player 1 returns
      else if(m_Player1.hitTest((int)m_Ball.m_pos,strip.m_numLeds)  && m_Ball.m_vel < 0) {
        Sounds.play(CSounds::S_HIT, ++m_rallyCount);
        m_Ball.m_vel *= -RETURN_VELOCITY_MULTIPLIER;
      }
      // Player 2 returns
      else if(m_Player2.hitTest((int)(0.5+m_Ball.m_pos),strip.m_numLeds) && m_Ball.m_vel > 0) {
        // player 2 return
        Sounds.play(CSounds::S_HIT, ++m_rallyCount);
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


