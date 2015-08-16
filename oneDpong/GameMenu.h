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
class CGameMenu
{
public:  
  void render(APA102& strip) {
    static byte q = 0;
    byte qq = 255;
      for(int i=0; i<strip.m_numLeds; ++i) {
        strip.m_data[i] = RGB_LED(qq,qq,qq);

      }
  }
  
  byte  run(APA102& strip, unsigned long ms) {
    return 0;
  }  
};

