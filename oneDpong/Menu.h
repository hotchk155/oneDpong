/////////////////////////////////////////////////////////////////////////
//
// ONE DIMENSIONAL PONG
// 2015/hotchk155
// Sixty Four Pixels Ltd  six4pix.com/pong1d
//
/////////////////////////////////////////////////////////////////////////

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
class CMenu
{
  byte m_debounce;
  byte m_scrollDelay;
  int m_keyRepeat;
  int m_maxScroll;
  int m_scroll;
  
  char m_settingIndex;  // menu option setting index
  int m_settingValue;   // value of the setting

  byte m_dataEntry; // are we in data entry mode?
  
  byte m_menuOn;
  byte m_buttons;
  char *m_szScrollText;

  byte m_ms;  
  GAME_TYPE m_game;
  GAME_SETTING m_setting;
  
  enum {
    INITIAL_SCROLL = -10
  };
public:  

  ///////////////////////////////////////////////////////////////
  CMenu() {
    m_maxScroll = 0;
    m_scroll = 0;
    m_menuOn = 0;
    m_buttons = 0;
    m_debounce = 0;
    m_scrollDelay = 0;
    m_dataEntry = 0;
    m_keyRepeat = 0;
  }
  
  ///////////////////////////////////////////////////////////////
  void render(APA102& strip) {
    switch(m_settingIndex) {
      case CSettings::GEN_STRIP_LEN:
        strip.resize(m_settingValue);
        strip.fill(RGB_LED(0x40,0,0));
        strip.m_data[m_settingValue] = RGB_LED(0xFF,0xFF,0xFF);
        break;
      default:
        strip.cls();
        break;
    }
  }
  
  ///////////////////////////////////////////////////////////////
  void displayValue()
  {
    m_szScrollText = NULL;
    char s[5];
    byte i = 0;
    s[i++] = ':';
    int n = m_settingValue; 
    if(n > 100) {
      n%=1000;
      s[i++] = '0' + n/100;
    }
    if(n > 10) {
      n%=100;
      s[i++] = '0' + n/10;
    }
    n%=10;
    s[i++] = '0' + n;
    s[i++] = 0;
    Display.showText(s,0);   
  }
  
  ///////////////////////////////////////////////////////////////
  // Handler for the SELECT button getting pressed
  void onSelectButton() {
    
    // are we exiting from data entry mode?
    if(m_dataEntry) {
      // store the current setting
      Settings.set(m_settingIndex, m_settingValue);
      m_dataEntry = 0;
    }
    else {
      // move to the next setting
      switch(m_settingIndex) {
        case CSettings::GEN_GAME:
          if(m_game.settingMin < 0) {
            m_settingIndex = CSettings::GEN_STRIP_LEN;            
          } else {
            m_settingIndex = m_game.settingMin;
          }
          break;
        case CSettings::GEN_STRIP_LEN:
          m_settingIndex = CSettings::GEN_GAME;
          break;
        default:
          if(++m_settingIndex > m_game.settingMax) {
            m_settingIndex = CSettings::GEN_STRIP_LEN;            
          }
          break;
      }
    }            
    m_setting = Settings.getSettingInfo(m_settingIndex);
    m_settingValue = Settings.get(m_settingIndex);
    setScrollText(m_setting.name);
    Sounds.play(CSounds::S_TICK);
  }
  
  ///////////////////////////////////////////////////////////////
  // Handler for the DATA button getting pressed
  void onDataButton() {
    if(m_dataEntry) {
      Sounds.play(CSounds::S_TICK);
      if(++m_settingValue > m_setting.maxValue) {
        m_settingValue = m_setting.minValue;
      }
    }
    m_dataEntry = 1;
    switch(m_settingIndex) {
      case CSettings::GEN_GAME:
        m_game = Settings.getGameInfo(m_settingValue);
        setScrollText(m_game.name);
        break;
      default:
        displayValue();
        break;
    }
  }
  
  ///////////////////////////////////////////////////////////////  
  void setScrollText(char *sz) {
    m_szScrollText = sz;
    m_maxScroll = Display.measureText(sz);
    m_scroll = INITIAL_SCROLL;
  }
  
  
  ///////////////////////////////////////////////////////////////  
  byte  run(APA102& strip, unsigned long ms) {    

   if((byte)ms != m_ms) {
     m_ms = (byte)ms;
     if(m_debounce) --m_debounce;
     if(m_scrollDelay) --m_scrollDelay;
     if(m_keyRepeat) --m_keyRepeat;
   }
   // read the menu switches    
   if(!m_debounce) {

     byte buttons = 0;
      if(!digitalRead(P_SELECT)) {
          // select is pressed
          buttons|=1;
      }
      
      if(!digitalRead(P_INPUT)) {
        // data input is pressed
        buttons|=2;
      }
                   
      // any chamge to button presses
      if(buttons != m_buttons) {
        if((buttons&1) && !(m_buttons&1)) {
          if(!m_menuOn) {
            Sounds.play(CSounds::S_TICK);
            m_settingIndex = CSettings::GEN_GAME;
            m_settingValue = Settings.get(m_settingIndex);
            m_setting = Settings.getSettingInfo(m_settingIndex);
            m_game = Settings.getGameInfo(m_settingValue);
            setScrollText(m_setting.name);
            m_menuOn = 1;
          }
          else {
            // select was pressed
            onSelectButton();
          }
        }
        if(m_menuOn && (buttons&2) && !(m_buttons&2)) {
          // mode was pressed
          onDataButton();
        }
        // debounce
        m_buttons = buttons;
        m_keyRepeat = 2000;
        m_debounce = 20;
      }
      else  if(m_menuOn && (buttons & 2) && !m_keyRepeat)
      {
        m_keyRepeat = 100;
        onDataButton();
      }      
    }
   
   
   
   if(!m_menuOn) {
     // we have no business here..
     return 0;
   }

   if(m_szScrollText && !m_scrollDelay) {
     m_scrollDelay = 100;
     ++m_scroll;
     if(m_scroll > m_maxScroll) {
       m_scroll = INITIAL_SCROLL;
     }
     if(m_scroll >= 0) {       
       Display.showText(m_szScrollText, m_scroll);   
     }
     else {
       Display.showText(m_szScrollText, 0);   
     }
   }
    // return 1 to display menu - once menu selected
    // we stay in that mode until the game is reset
    return 1;
  }  
};

