/*
  Example code for the EC Reader v1.0
  
  http://www.cyberplant.info
  by CyberPlant LLC, 03 December 2015
  This example code is in the public domain.

*/
 
#include <EEPROM.h>
#include <OneWire.h>
#include <DS18B20.h>

#define X0 0.0
#define X1 2.0
#define X2 12.88
#define X3 80.0

int addressCheckMemory = 0;
int addressY0 = addressCheckMemory+sizeof(float);
int addressY1 = addressY0+sizeof(float);
int addressY2 = addressY1+sizeof(float);
int addressY3 = addressY2+sizeof(float);

const float CheckMemory = 3.14159;

long Y0 = EEPROM_float_read(addressY0);
long Y1 = EEPROM_float_read(addressY1);
long Y2 = EEPROM_float_read(addressY2);
long Y3 = EEPROM_float_read(addressY3);

float A;
float B;
float C;

long pulseCount = 0;  //a pulse counter variable
long pulseCal;
unsigned long pulseTime,lastTime, duration, totalDuration;


unsigned int Interval=1000;
long previousMillis = 0;
unsigned long Time;

float EC;
float temp;
float tempDefault = 25.0;

const int numReadings_EC = 10;
float readings_EC[numReadings_EC];      // the readings from the analog input
int index_EC = 0;                  // the index of the current reading
float total_EC = 0;                  // the running total
float average_EC = 0;                // the average

int sequence = 0;

const byte ONEWIRE_PIN = 3; // temperature sensor ds18b20
byte address[8];
OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);

int incomingByte = 0;  
 
int cal_Y0 = 0;
int cal_Y1 = 0;
int cal_Y2 = 0;
int cal_Y3 = 0;

void setup()
{
  Serial.begin(9600);
  Time=millis();
  //pinMode(2, INPUT_PULLUP); //  An internal 20K-ohm resistor is pulled to 5V. If you use hardware pull-up delete this
  sensors.begin();
  Search_sensors();

  if (EEPROM_float_read(addressCheckMemory) != CheckMemory)
  {
  EEPROM_float_write(addressCheckMemory, CheckMemory);
  EEPROM_float_write(addressY0, 240);
  EEPROM_float_write(addressY1, 1245);
  EEPROM_float_write(addressY2, 5282);
  EEPROM_float_write(addressY3, 17255);
  Y0 = EEPROM_float_read(addressY0);
  Y1 = EEPROM_float_read(addressY1);
  Y2 = EEPROM_float_read(addressY2);
  Y3 = EEPROM_float_read(addressY3);
  Serial.println("Primary auto setting is complete. Device is ready");
  Serial.println("_________________________");
  }
  Serial.println("Calibrate commands:");
  Serial.println("E.C. :");
  Serial.println("      Cal. 0,00 uS ---- 0");
  Serial.println("      Cal. 2,00 uS ---- 1");
  Serial.println("      Cal. 12,88 uS --- 2");
  Serial.println("      Cal. 80,00 uS --- 3");
  Serial.println("      Reset E.C. ------ 5");
  Serial.println("  ");
  delay(250);

for (int thisReading_EC = 0; thisReading_EC < numReadings_EC; thisReading_EC++)
readings_EC[thisReading_EC] = 0;

}


void loop()
{
  if (millis()-Time>=Interval)
{  
  Time = millis();
  
  sequence ++;
  if (sequence > 1)
  sequence = 0;


   if (sequence==0)
  {
    pulseCount=0; //reset the pulse counter
    attachInterrupt(0,onPulse,RISING); //attach an interrupt counter to interrupt pin 1 (digital pin #3) -- the only other possible pin on the 328p is interrupt pin #0 (digital pin #2)
  }
  
   if (sequence==1)
  {
     detachInterrupt(0);
     

 // Requests sensor for measurement
  sensors.request(address);
  
  // Waiting (block the program) for measurement reesults
  while(!sensors.available());
  
  // Reading of sensors  
  temp = sensors.readTemperature(address);
  if (temp > 200 || temp < -20 )
  { 
    temp = tempDefault;
    Serial.println("temp sensor connection error!");
    Search_sensors();
  }
  
  pulseCal = ECread();
  EC = ECcal();
  
  // Prints measurements on Serial Monitor
  Serial.println("  ");
  Serial.print("  t ");
  Serial.print(temp, 1);
  Serial.print(F(" *C"));
  Serial.print("    E.C. ");
  Serial.println(EC); // uS/cm
  Serial.print("pulses/sec = ");
  Serial.println(pulseCal);
  //Serial.print("C = ");
  //Serial.println(C); // Conductivity without temperature compensation
  }
}

    if (Serial.available() > 0) //  function of calibration E.C.
    {  
        incomingByte = Serial.read();
        cal_sensors();
    }

}

/*-----------------------------------End loop---------------------------------------*/

long ECread()
{
    total_EC = total_EC - readings_EC[index_EC];
    readings_EC[index_EC] = pulseCount;
    total_EC = total_EC + readings_EC[index_EC];
    index_EC = index_EC + 1;
    // if we're at the end of the array...
    if (index_EC >= numReadings_EC)              
    // ...wrap around to the beginning: 
    index_EC = 0;    
    pulseCount = total_EC / numReadings_EC;
    pulseCal = pulseCount;
}

float ECcal()  //graph function of read EC
{
     if (pulseCal>Y0 && pulseCal<Y1 )
      {
        A = (Y1 - Y0) / (X1 - X0);
        B = Y0 - (A * X0);
        C = (pulseCal - B) / A;
      }
      
      if (pulseCal > Y1 && pulseCal<Y2 )
      {
        A = (Y2-Y1) / (X2 - X1);
        B = Y1 - (A * X1);
        C = (pulseCal - B) / A;
      }
      if (pulseCal > Y2)
      {
        A = (Y3-Y2) / (X3 - X2);
        B = Y2 - (A * X2);
        C = (pulseCal - B) / A;
      }
      
    EC = (C / (1 + 0.019 * (temp-25.00))); // At higher conductivity, about 6 uS/cm at 25°C, the neutral salt correction behaves like the linear temperature correction model, albeit a correction in which the slope is temperature dependent. At 5°C the coefficient is 0.019, and at 90°C the coefficient is 0.025.
}

void EEPROM_float_write(int addr, float val) // write to EEPROM
{  
  byte *x = (byte *)&val;
  for(byte i = 0; i < 4; i++) EEPROM.write(i+addr, x[i]);
}

float EEPROM_float_read(int addr) // read of EEPROM
{    
  byte x[4];
  for(byte i = 0; i < 4; i++) x[i] = EEPROM.read(i+addr);
  float *y = (float *)&x;
  return y[0];
}

void onPulse() // EC pulse counter
{
  pulseCount++;
}

void Search_sensors() // search ds18b20 temperature sensor
{
  address[8];
  
  onewire.reset_search();
  while(onewire.search(address))
  {
    if (address[0] != 0x28)
      continue;
      
    if (OneWire::crc8(address, 7) != address[7])
    {
      Serial.println(F("temp sensor connection error!"));
      temp = 25.0;
      break;
    }
   /*
    for (byte i=0; i<8; i++)
    {
      Serial.print(F("0x"));
      Serial.print(address[i], HEX);
      
      if (i < 7)
        Serial.print(F(", "));
    }
    
    */
  }

}

void cal_sensors()
{
  Serial.println(" ");
  
 if (incomingByte == 53) // press key "5"
 {
  Serial.print("Reset EC ...");
  EEPROM_float_write(addressY0, 240);
  EEPROM_float_write(addressY1, 1245);
  EEPROM_float_write(addressY2, 5282);
  EEPROM_float_write(addressY3, 17255);
  Y0 = EEPROM_float_read(addressY0);
  Y1 = EEPROM_float_read(addressY1);
  Y2 = EEPROM_float_read(addressY2);
  Y3 = EEPROM_float_read(addressY3);
  Serial.println(" complete");
 }
 
 else if (incomingByte == 48) // press key "0"
 {
  Serial.print("Cal. 0,00 uS ...");  
  Y0 = pulseCal / (1 + 0.019 * (temp-25.00));
  EC = ECcal();
  while (EC > 0.005)
    {
    Y0++;
    EC = ECcal();
    }
  EEPROM_float_write(addressY0, Y0);
  Serial.println(" complete");
 }
 
 else if (incomingByte == 49) // press key "1"
 {
  Serial.print("Cal. 2,00 uS ...");  
  Y1 = pulseCal / (1 + 0.019 * (temp-25.00));
  EC = ECcal();
  while (EC > X1)
    {
    Y1++;
    EC = ECcal();
    }
  EEPROM_float_write(addressY1, Y1);
  Serial.println(" complete");
 }
 
 else if (incomingByte == 50) // press key "2"
 {
  Serial.print("Cal. 12,88 uS ...");  
  Y2 = pulseCal / (1 + 0.019 * (temp-25.00));
  EC = ECcal();
  while (EC > X2)
    {      
    Y2++;
    EC = ECcal();
    }
  EEPROM_float_write(addressY2, Y2);
  Serial.println(" complete");
 }
 
  else if (incomingByte == 51) // press key "3"
 {
  Serial.print("Cal. 80,00 uS ..."); 
  Y3 = pulseCal / (1 + 0.019 * (temp-25.00));
  EC = ECcal(); 
  while (EC > X3)
    { 
    Y3++;
    EC = ECcal();
    }
  EEPROM_float_write(addressY3, Y3);
  Serial.println(" complete");
 }
}
