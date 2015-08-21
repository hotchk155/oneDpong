// Game settings

// Structure to hold type info for a game
typedef struct {
  char *name;
  int settingMin;  
  int settingMax;  
} GAME_TYPE;

// Structure to hold type info for a game setting
typedef struct {
  char *name;
  int minValue;
  int maxValue;  
  int defaultValue;
} GAME_SETTING;


// Game settings manager
class CSettings {
  
public:

  enum {
    MAGIC_COOKIE_1 = 24,
    MAGIC_COOKIE_2 = 1,
    DEFAULT_STRIP_LEN = 240
  };
  
  // Define EEPROM addresses
  enum {
    EE_MAGIC_COOKIE_1 = 0,
    EE_MAGIC_COOKIE_2,
    EE_GAME_TYPE,
    EE_STRIP_LEN_L,
    EE_STRIP_LEN_H,
    EE_SETTINGS // base address for the settings
  };

  enum {
    GEN_GAME = -2,
    GEN_STRIP_LEN = -1,
  };
  
  // Define the settings for each game
  enum {
    PONG_INITSPEED = 0,
    PONG_ACCELERATION,
    PONG_MAXSPEED,  
    PONG_SIMPLIFY1,
    PONG_SIMPLIFY2,
    MAX_SETTING
  };

  // Look up information about a game, including display name and range of settings  
  GAME_TYPE getGameInfo(int game) {
    switch(game) {
      case GAME_PONG:         return (GAME_TYPE){ "Pong", PONG_INITSPEED, PONG_SIMPLIFY2 };
      case GAME_PONG_1P:      return (GAME_TYPE){ "1 Player Pong", PONG_INITSPEED, PONG_SIMPLIFY2 };
      case GAME_BREAKOUT:     return (GAME_TYPE){ "Breakout", -1, -1 };
      default: return (GAME_TYPE){ NULL };
    }
  }
  
  // Look up information about a setting including display name, min value, max value, default value
  GAME_SETTING getSettingInfo(int setting) {
    switch(setting) {
      case GEN_GAME:          return (GAME_SETTING){ "Game Type", 0,GAME_MAX-1,0 };
      case GEN_STRIP_LEN:     return (GAME_SETTING){ "Strip Length", 100,APA102_MAX_LEDS,240 };
      case PONG_INITSPEED:    return (GAME_SETTING){ "Speed", 1, 9, 5 };
      case PONG_ACCELERATION: return (GAME_SETTING){ "Acceleration", 1, 9, 5 };
      case PONG_MAXSPEED:     return (GAME_SETTING){ "Limit", 1, 9, 9 };
      case PONG_SIMPLIFY1:    return (GAME_SETTING){ "P1 Simplify", 0, 9, 0 };
      case PONG_SIMPLIFY2:    return (GAME_SETTING){ "P2 Simplify", 0, 9, 0 };
      default: return (GAME_SETTING){ NULL };
    }
  }
  
  // Initialise the settings in EEPROM if they are not already initialised
  void  init() {
    // check if "magic cookie" is present
    if( EEPROM.read(EE_MAGIC_COOKIE_1) != MAGIC_COOKIE_1 ||
        EEPROM.read(EE_MAGIC_COOKIE_2) != MAGIC_COOKIE_2 ) {
          
      // no, so store the default value for each setting
      for(int i=0; i<MAX_SETTING; ++i) {
        GAME_SETTING info = getSettingInfo(i);
        EEPROM.write(EE_SETTINGS + i, info.defaultValue);
      }      
      
      // store default game and strip len
      EEPROM.write(EE_GAME_TYPE, GAME_PONG);
      EEPROM.write(EE_STRIP_LEN_L, DEFAULT_STRIP_LEN);
      EEPROM.write(EE_STRIP_LEN_H, 0);
      
      // write "magic cookie" value
      EEPROM.write(EE_MAGIC_COOKIE_1, MAGIC_COOKIE_1);
      EEPROM.write(EE_MAGIC_COOKIE_2, MAGIC_COOKIE_2);
    }
  }
  
  ////////////////////////////////////////////////////////////////
  // Get a setting value
  int get(int setting) {
    int value;
    switch(setting) {
      case GEN_GAME:
        value = EEPROM.read(EE_GAME_TYPE);
        break;
      case GEN_STRIP_LEN:
        value = (int)EEPROM.read(EE_STRIP_LEN_L) + 256 * EEPROM.read(EE_STRIP_LEN_H);
        break;
      default:
        value = EEPROM.read(EE_SETTINGS + setting);        
        break;
    }
    GAME_SETTING s = getSettingInfo(setting);
    return constrain(value, s.minValue, s.maxValue);
  }

  ////////////////////////////////////////////////////////////////
  // Store a setting value
  void set(int setting, int value) {
    GAME_SETTING s = getSettingInfo(setting);
    value = constrain(value, s.minValue, s.maxValue);
    switch(setting) {
      case GEN_GAME:
        EEPROM.write(EE_GAME_TYPE, (byte)value);
        break;
      case GEN_STRIP_LEN:
        EEPROM.write(EE_STRIP_LEN_L, (byte)value);
        EEPROM.write(EE_STRIP_LEN_H, (byte)(value>>8));
        break;
      default:
        EEPROM.write(EE_SETTINGS + setting, (byte)value);
        break;
    }
  }  
};

extern CSettings Settings;
