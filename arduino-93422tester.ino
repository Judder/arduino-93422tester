// NOTE: The power supply pins for this chip are non standard layout.

/*
 * Arduino 94322 RAM tester
 * Jon (Porchy)
 * Alex (Judder) Judd
 * December 2016
 */

// 93422
//
// A3  [1] (3)  (+5V) [22] Vcc
// A2  [2] (2)    (4) [21] A4
// A1  [3] (A0)  (A3) [20] _WE
// A0  [4] (A1) (GND) [19] _CS1
// A5  [5] (5)   (A2) [18] _OE
// A6  [6] (6)  (+5V) [17] CS2
// A7  [7] (7)   (A5) [16] O3
// GND [8] (GND) (11) [15] D3
// D0  [9] (8)   (A4) [14] O2
// O0 [10] (12)  (10) [13] D2
// D1 [11] (9)   (13) [12] O1

#define pinCS1 20 //not connected on UNO so use GND as this is active LOW
#define pinCS2 21 //not connected on UNO so use +5V as this is active HIGH
#define pinOE A2 //2.47V default
#define pinWE A3 //2.47V default

#define pinA0 A0 //2.47V default
#define pinA1 A1 //2.47V default
#define pinA2 2
#define pinA3 3
#define pinA4 4
#define pinA5 5
#define pinA6 6
#define pinA7 7

#define pinD0 8
#define pinD1 9
#define pinD2 10
#define pinD3 11

#define pinO0 12
#define pinO1 13
#define pinO2 A4 //2.47V default
#define pinO3 A5 //2.47V default

#define max_addr 0xff

uint8_t addr_pins[] = {pinA0, pinA1, pinA2, pinA3, pinA4, pinA5, pinA6, pinA7};
uint8_t out_pins[] = {pinD0, pinD1, pinD2, pinD3};
uint8_t in_pins[] = {pinO0, pinO1, pinO2, pinO3};

bool debug = false;

int failed = 0;

void setup() 
{
    Serial.begin(9600);
    Serial.println("Initialising:"); //debug
    
    //set chip select and enable/write to output and configure
    pinMode(pinOE, OUTPUT);
    pinMode(pinWE, OUTPUT);
    pinMode(pinCS1, OUTPUT);
    pinMode(pinCS2, OUTPUT);
    digitalWrite(pinOE, HIGH); //output disable, active low
    digitalWrite(pinWE, HIGH); //read mode, active low
    digitalWrite(pinCS1, LOW); //active low
    digitalWrite(pinCS2, HIGH); //active high
    
    //set address pins as output
    for (int i = 0; i < sizeof(addr_pins); i++)
    {
      pinMode(addr_pins[i], OUTPUT);
      digitalWrite(addr_pins[i], HIGH);
    }

    //set output pins as output
    for (int i = 0; i < sizeof(out_pins); i++)
    {
      pinMode(out_pins[i], OUTPUT);
      digitalWrite(out_pins[i], LOW);
    }

    digitalWrite(pinOE, HIGH); //output disable, active low
    digitalWrite(pinWE, LOW); //write mode, active low

    digitalWrite(pinOE, LOW); //enable, active low
    digitalWrite(pinWE, HIGH); //read mode, active low
    
    //set data pins as input
    for (int i = 0; i < sizeof(in_pins); i++)
    {
      pinMode(in_pins[i], INPUT_PULLUP);
      uint8_t readback = digitalRead(in_pins[i]);
      if (readback == 0) {
        Serial.println("Initialisation check: pin " + String(i, DEC) + " LOW passed");
      } else {
        Serial.println("Initialisation check: pin " + String(i, DEC) + " LOW failed");
        failed++;
      }
    }

    //set output pins as output
    for (int i = 0; i < sizeof(out_pins); i++)
    {
      pinMode(out_pins[i], OUTPUT);
      digitalWrite(out_pins[i], HIGH);
    }

    digitalWrite(pinOE, HIGH); //output disable, active low
    digitalWrite(pinWE, LOW); //write mode, active low

    digitalWrite(pinOE, LOW); //enable, active low
    digitalWrite(pinWE, HIGH); //read mode, active low
    
    //set data pins as input
    for (int i = 0; i < sizeof(in_pins); i++)
    {
      pinMode(in_pins[i], INPUT_PULLUP);
      uint8_t readback = digitalRead(in_pins[i]);
      if (readback == 1) {
        Serial.println("Initialisation check: pin " + String(i, DEC) + " HIGH passed");
      } else {
        Serial.println("Initialisation check: pin " + String(i, DEC) + " HIGH failed");
        failed++;
      }
    }
}

void setAddress(uint8_t address)
{        
    for (int i = 0; i < 8; i++) 
    {
       digitalWrite(addr_pins[i], ((address >> i) & 1));
    }
    if (debug) {
        Serial.println("Address set: " + String(address, BIN));
    }
}

void writeData(uint8_t data)
{
    //write enable start
    digitalWrite(pinWE, LOW);
    digitalWrite(pinOE, HIGH);
    //write enable end
    
    for (int i = 0; i < sizeof(out_pins); i++) 
    {
      pinMode(out_pins[i], OUTPUT);
      digitalWrite(out_pins[i], ((data >> i) & 1));
    }
    if (debug) {
        Serial.println("Write data: " + String(data, BIN));
    }
}

int readData()
{
    //read enable start
    digitalWrite(pinWE, HIGH);
    digitalWrite(pinOE, LOW);
    //read enable end
    
    uint8_t readback=0;
    for (int i = 0; i < sizeof(in_pins); i++) 
    {
      pinMode(in_pins[i], INPUT_PULLUP);
      readback = readback | ((digitalRead(in_pins[i]) << i));
    }
    if (debug) {
        Serial.println("Read data: " + String(readback, BIN));
    }
    return readback;
}

void incrementCheck()
{
  int stress_test = 0xff;
  for (int h=0; h <= stress_test; h++)
  {
    uint8_t currenti = 0xff;
    //write incrementing values
    for (int i=0; i <= B1111; i++)
    {
      if (currenti != i) {
        currenti = i;
        Serial.println("Testing value: [" + String(h, HEX) + "] " + String(i, BIN));
      }
      for (int j=0; j < max_addr; j++)
      {
        setAddress(j);
        writeData(i);
      }
  
      //read incrementing values
      for (int j=0; j < max_addr; j++)
      {
        setAddress(j);
        uint8_t readback = readData();
        if (readback != i)
        {
          Serial.println("Failed at address "+String(j, HEX));
          Serial.println("Expected "+String(i, BIN)+", found "+String(readback,BIN));
          failed++;
        }
      }
    }
  }
}

void loop()
{
    if (failed == 0) {
      incrementCheck();
    }
    if (failed > 0) {
      Serial.println("Result: Failed (" + String(failed, BIN) + ")");
    } else {
      Serial.println("Result: Passed");
    }
    //digitalWrite(LED_BUILTIN, HIGH);
    while(1); //end
}
