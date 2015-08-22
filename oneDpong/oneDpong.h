#define P_DS    14 // PC0
#define P_SHCP  15 // PC1
#define P_STCP  16 // PC2
#define P_D0 8 // PB0
#define P_D1 9 // PB1
#define P_D2 2 // PD2
#define P_D3 3 // PD3
#define P_D4 4 //PD4
#define P_D5 5 //PD5
#define P_D6 6 // PD6
#define P_D7 7 // PD7
#define P_SOUND 17
#define P_PLAYER1 0
#define P_PLAYER2 1

#define P_SELECT 18
#define P_INPUT  19

enum {
  EVENT_NONE,
  EVENT_P1_PRESS,
  EVENT_P2_PRESS,
  EVENT_P1_RELEASE,
  EVENT_P2_RELEASE
};

// enumeration of games
enum {
  GAME_PONG,
  GAME_PONG_1P,
  GAME_BREAKOUT,
  GAME_MAX
};




