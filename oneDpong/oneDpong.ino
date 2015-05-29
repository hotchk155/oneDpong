

#define P_DS    8
#define P_SHCP  9
#define P_STCP  10
#define P_D0 0
#define P_D1 1
#define P_D2 2
#define P_D3 3
#define P_D4 4
#define P_D5 5
#define P_D6 6
#define P_D7 7
byte disp[8];

void setup() 
{
  pinMode(P_DS, OUTPUT);  
  pinMode(P_SHCP, OUTPUT);  
  pinMode(P_STCP, OUTPUT);  
  pinMode(P_D0, OUTPUT);  
  pinMode(P_D1, OUTPUT);  
  pinMode(P_D2, OUTPUT);  
  pinMode(P_D3, OUTPUT);  
  pinMode(P_D4, OUTPUT);  
  pinMode(P_D5, OUTPUT);  
  pinMode(P_D6, OUTPUT);  
  pinMode(P_D7, OUTPUT);  
  
  disp[0] = 0b00000001;
  disp[1] = 0b00000011;
  disp[2] = 0b00000111;
  disp[3] = 0b00001111;
  disp[4] = 0b00011111;
  disp[5] = 0b00111111;
  disp[6] = 0b01111111;
  disp[7] = 0b11111111;
}

void refreshMatrix() {
  int i;  

   // clear shift reg
  digitalWrite(P_DS, LOW);
  for(i=0; i<8; ++i) {
    digitalWrite(P_SHCP, LOW);
    digitalWrite(P_SHCP, HIGH);
  }
    
  // clock in single bit
  digitalWrite(P_DS, HIGH);
  digitalWrite(P_SHCP, LOW);
  digitalWrite(P_SHCP, HIGH);
  digitalWrite(P_DS, LOW);
  
  for(i=0; i<8; ++i) {
    // clock to next position
    digitalWrite(P_STCP, LOW);
    digitalWrite(P_STCP, HIGH);
    
    PORTD = disp[i];
    delay(1);
    PORTD = 0;
    digitalWrite(P_SHCP, LOW);
    digitalWrite(P_SHCP, HIGH);
    
  }    
}

void loop() 
{
  refreshMatrix();  
}
