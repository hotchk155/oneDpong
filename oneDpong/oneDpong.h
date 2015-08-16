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


/*

Initial speed: 0-9
Acceleration: 0-9

P1/P2: Speed Limit
- None
- F/M/S

P1/P2: Bat operation 
- Normal bat
- Long bat
- Extra long bat
- Extra long bat can be extended again even if retracting
- Extra long bat can be held extended
- Normal bat, Auto hit

P1/P2: Switch operation (NO/NC)
 

Strip Length

*/

// Player simplify options
enum {
  PONG_OPT_SPEEDLIM_1   = 0x0001,
  PONG_OPT_SPEEDLIM_2   = 0x0002,
  PONG_OPT_SPEEDLIM_3   = 0x0004,
  PONG_OPT_BIGBAT_1    = 0x0008,
  PONG_OPT_BIGBAT_2    = 0x0010,
  PONG_OPT_BIGBAT_3    = 0x0020,
  PONG_OPT_BAT_NOREST   = 0x0040,
  PONG_OPT_BAT_HOLD     = 0x0080,
  PONG_OPT_BAT_AUTO     = 0x0100
};

typedef struct {
  int initialSpeed;
  int acceleration;
  int playerOptions1; 
  int playerOptions2; 
} PONG_SETTINGS;

typedef struct {
  byte magicCookie;
  int stripLen;
  PONG_SETTINGS pong;
} GAME_SETTINGS;



