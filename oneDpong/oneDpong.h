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

enum {
  EV_PRESS1 = 0,
  EV_PRESS2,
  EV_RELEASE1,
  EV_RELEASE2,


  EV_PRESSA,
  EV_PRESSB,
  EV_RELEASEA,
  EV_RELEASEB,

  EV_TIMER1,
  EV_TIMER2,
  EV_TIMER3,
  EV_TIMER4,
  EV_TIMER5


};
class CGameHandler
{
public:
  virtual void init() = 0;
  virtual void handle() = 0;
  virtual void done() = 0;
};
