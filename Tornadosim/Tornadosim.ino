
/*Cyclone Tornado Simulator
 * Uses OneWire to talk to the 18B20 temp sensor.  Tested using OneWire from this source
 * https://github.com/ntruchsess/arduino-OneWire
 */

#include <OneWire.h>
int DS18B20_Pin=2;//18B20 signal pin D2
OneWire ds(DS18B20_Pin); // OneWire now applied to D2
//Sets Temp LED to D3
const int tled=3;
//Sets Water level LED to D4
const int wled=4;
//Sets AC Tailswitch to D6
const int cut=6;
//Sets float switch input to D5
const int fswitch=5;
//Sets the max water temp in Celisus
const int temp_limit = 49;

//Configure pinmodes and serial port
void setup() {
  Serial.begin(9600);
  pinMode(tled, OUTPUT);
  pinMode(wled, OUTPUT);
  pinMode(cut,OUTPUT);
  pinMode(fswitch,INPUT);
}

void loop() {
  float tempc=getTemp();// gets temperature in Celsius
  float tempf=tempc*9/5+32; // converts to F
  int switchstate=0;
  switchstate=digitalRead(fswitch);
  Serial.print(millis());
  Serial.print("\t");
  Serial.print(tempf);
  Serial.print("\n"); 
  digitalWrite(wled,LOW);
  digitalWrite(tled,LOW);
  if (switchstate==1 && tempf>=125.5)
  {
    // water level is too low and Temp is too high turn off mister and turn on both LEDs
    Serial.println("Water Low and Temp Shutdown");
    digitalWrite(wled,HIGH);
    digitalWrite(cut,LOW);
    digitalWrite(tled,HIGH);
    delay(30000);
  }
  else if (tempf>122 && tempf<125.5 && switchstate==1)
  {
    Serial.println("Water Low!!!");
    float tw=1/(tempf-110);
    int i=0;
    digitalWrite(wled,HIGH);
    digitalWrite(cut,LOW);
    digitalWrite(tled,HIGH);
    delay(tw*1000);
    digitalWrite(tled,LOW);
    delay(tw*1000);
  }
  else if (tempf>122 && tempf<125.5)
  {
    Serial.println("Temp warning!");
    float tw=1/(tempf-110);
    int i=0;
    digitalWrite(cut,HIGH);
    digitalWrite(tled,HIGH);
    delay(tw*1000);
    digitalWrite(tled,LOW);
    delay(tw*1000);
    
  }
  else if(tempf >=125.5)
  {
    Serial.println("Temperature Shutdown");
    //if temp is over 120 f turns on warning light and turns off mister 
    digitalWrite(tled,HIGH);
    digitalWrite(cut,LOW);
    delay(30000);
  }
  else if(switchstate==1)
  {
    // if water level drops too low turn on water warning LED and turn off mister
    digitalWrite(wled,HIGH);
    digitalWrite(cut,LOW);
    Serial.println("Warning Water Low");
    delay(1000);
  }
  else
  {
    //continues if there are no problems
    digitalWrite(tled,LOW);
    digitalWrite(cut,HIGH);
    digitalWrite(wled,LOW);
    delay(500);
   }
}

//getTemp function, reads the data from a DS18B20 and returns
//the value in degrees Celsius

float getTemp(){
  
  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return 1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -8000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,0); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
