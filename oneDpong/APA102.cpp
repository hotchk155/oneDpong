#include "Arduino.h"
#include "pins_arduino.h"
#include "APA102.h"


APA102 *p_instance = NULL;


///////////////////////////////////////////////////////
// constructor
APA102::APA102(int numLeds) 
{    
  m_numLeds = numLeds;
  m_data = NULL;
  m_ledIndex = 0;
  m_sendState = -1;  
  p_instance= this;
}

///////////////////////////////////////////////////////
void APA102::begin() 
{
  // set pin modes
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(10, OUTPUT); // must be output for SPI master mode

  digitalWrite(SCK, LOW);
  digitalWrite(MOSI, LOW);
  digitalWrite(10, LOW);

  // set SPI control register
  SPCR = 
    (1<<SPIE) | // 1 = Enable SPI interrupts
    (1<<SPE)  | // 1 = Enable SPI
    (0<<DORD) | // 0 = MSB first
    (1<<MSTR) | // 1 = SPI master mode
    (0<<CPOL) | // SPI mode 0
    (0<<CPHA) | // :
    (1<<SPI2X) |
    (0<<SPR1) |
    (0<<SPR0);

 m_data = new LED[m_numLeds];
 m_sendState = -1;
 m_ledIndex = 0; 
}    

///////////////////////////////////////////////////////
// Start updating the strip. The previous send must have completed
void APA102::begin_transfer() 
{
  if(m_data) {
    m_ledIndex = 0;
    m_sendState = 0;
    SPDR=0;
  }
}

///////////////////////////////////////////////////////
// Check whether the send has completed
int APA102::is_transfer_complete() 
{
  return (m_sendState < 0);
}

///////////////////////////////////////////////////////
// Start a transfer and wait for it to complete
void APA102::refresh() 
{
  while(m_sendState >= 0);
  begin_transfer();
  while(m_sendState >= 0);
}

///////////////////////////////////////////////////////
// Set all LEDs off and send the data
void APA102::cls()
{
  for(int i=0; i<m_numLeds; ++i)    
    m_data[i] = RGB_LED(0,0,0);
}

void APA102::run_spi() {
  if(m_sendState >= 0) // sending?
  {
    switch(m_sendState) {
      case 0: case 1: case 2: case 3: // sending start frame
        ++m_sendState;
        SPDR = 0; 
        break;
      case 4:
        SPDR = 0xFF;  // start byte for LED frame
        m_sendState = 5;
        break;
      case 5: 
        SPDR = m_data[m_ledIndex].b;  // BLUE byte
        m_sendState = 6;
        break;
      case 6: 
        SPDR = m_data[m_ledIndex].g; // GREEN byte
        m_sendState = 7;
        break;
      case 7: // LED frame data bytes
        SPDR = m_data[m_ledIndex].r; // RED byte
        if(++m_ledIndex < m_numLeds) {
          m_sendState = 4;
        }
        else {
          m_sendState = 8;
        }
        break;
      case 8: case 9: case 10: case 11: // End frame
        SPDR = 0xFF; 
        ++m_sendState;
        break;
      case 12:
      default:  // final byte has been sent... all done!
        m_ledIndex = 0;
        m_sendState = -1;
        break;
    }
  }
}

///////////////////////////////////////////////////////
// Interrupt service routine is called each time the 
// SPI peripheral has finished sending a byte
ISR (SPI_STC_vect)
{
  if(p_instance) {
    p_instance->run_spi();
  }
}
  

