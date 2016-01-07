

/*
  Example code for the EC Reader v1.0

  http://www.cyber-plant.com
  by CyberPlant LLC, 03 December 2015
  This example code is in the public domain.
  upd. 07 January 2016

*/
#include <EEPROM.h>
#include <OneWire.h>
#include <DS18B20.h>

#define X0 0.0
#define X1 2.0
#define X2 12.88
#define X3 80.0

#define alphaLTC 0.022 // The linear temperature coefficient



float A;
float B;
float C;

long pulseCount = 0;  //a pulse counter variable
long pulseCal;
byte ECcal = 0;

unsigned long pulseTime, lastTime, duration, totalDuration;


unsigned int Interval = 1000;
long previousMillis = 0;
unsigned long Time;

float EC;
float temp;
float tempManual = 25.0;

int sequence = 0;

const byte ONEWIRE_PIN = 3; // temperature sensor ds18b20, pin D3
byte address[8];
OneWire onewire(ONEWIRE_PIN);
DS18B20 sensors(&onewire);

int incomingByte = 0;

long Y0;
long Y1;
long Y2;
long Y3;

struct MyObject {
  long Y0;
  long Y1;
  long Y2;
  long Y3;
};

void setup()
{
  int eeAddress = 0;

  MyObject customVar;
  EEPROM.get(eeAddress, customVar);

  Y0 = (customVar.Y0);
  Y1 = (customVar.Y1);
  Y2 = (customVar.Y2);
  Y3 = (customVar.Y3);

  Serial.begin(9600);
  Time = millis();
  pinMode(2, INPUT_PULLUP); //  An internal 20K-ohm resistor is pulled to 5V. If you use hardware pull-up delete this
  sensors.begin();
  Search_sensors();

  Serial.println("Calibrate commands:");
  Serial.println("E.C. :");
  Serial.println("      Cal. 0,00 uS ---- 0");
  Serial.println("      Cal. 2,00 uS ---- 1");
  Serial.println("      Cal. 12,88 uS --- 2");
  Serial.println("      Cal. 80,00 uS --- 3");
  Serial.println("      Reset E.C. ------ 5");
  Serial.println("  ");
  delay(750);

}

void temp_read()
{
  sensors.request(address);
  while (!sensors.available());
  temp = sensors.readTemperature(address);
  if (temp < 0.1) {
    temp = tempManual;
    Serial.println(F("DS18B20 connection error!"));
    Search_sensors();
  }
}


void ECread()  //graph function of read EC
{
  if (pulseCal < Y0)
  {
    C = 0;
  }
  if (pulseCal > Y0 && pulseCal < Y1 )
  {
    A = (Y1 - Y0) / (X1 - X0);
    B = Y0 - (A * X0);
    C = (pulseCal - B) / A;
  }

  if (pulseCal > Y1 && pulseCal < Y2 )
  {
    A = (Y2 - Y1) / (X2 - X1);
    B = Y1 - (A * X1);
    C = (pulseCal - B) / A;
  }
  if (pulseCal > Y2)
  {
    A = (Y3 - Y2) / (X3 - X2);
    B = Y2 - (A * X2);
    C = (pulseCal - B) / A;
  }

  EC = (C / (1 + alphaLTC * (temp - 25.00)));
}

void onPulse() // EC pulse counter
{
  pulseCount++;
}

void Search_sensors() // search ds18b20 temperature sensor
{
  address[8];

  onewire.reset_search();
  while (onewire.search(address))
  {
    if (address[0] != 0x28)
      continue;
  }
}
void cal_sensors()
{
  Serial.println(" ");

  if (incomingByte == 53) // press key "5"
  {
    Reset_EC();
  }
  else if (incomingByte == 48) // press key "0"
  {
    ECcal = 1;
    Serial.print("Cal. 0,00 uS ...");
    Y0 = pulseCal;
    int eeAddress = 0;
    EEPROM.put(eeAddress, Y0);
    Serial.println(" complete");
    ECcal = 0;
  }

  else if (incomingByte == 49) // press key "1"
  {
    ECcal = 1;
    Serial.print("Cal. 2,00 uS ...");
    Y1 = pulseCal / (1 + alphaLTC * (temp - 25.00));
    ECread();
    while (EC > X1)
    {
      Y1++;
      ECread();
    }
    int eeAddress = 0 + sizeof(long);
    EEPROM.put(eeAddress, Y1);
    Serial.println(" complete");
    ECcal = 0;
  }

  else if (incomingByte == 50) // press key "2"
  {
    ECcal = 1;
    Serial.print("Cal. 12,88 uS ...");
    Y2 = pulseCal / (1 + alphaLTC * (temp - 25.00));
    ECread();
    while (EC > X2)
    {
      Y2++;
      ECread();
    }
    int eeAddress = 0 + (sizeof(long) * 2);
    EEPROM.put(eeAddress, Y2);
    Serial.println(" complete");
    ECcal = 0;
  }

  else if (incomingByte == 51) // press key "3"
  {
    ECcal = 1;
    Serial.print("Cal. 80,00 uS ...");
    Y3 = pulseCal / (1 + alphaLTC * (temp - 25.00));
    ECread();
    while (EC > X3)
    {
      Y3++;
      ECread();
    }
    int eeAddress = 0 + sizeof(long) * 3;
    EEPROM.put(eeAddress, Y3);
    Serial.println(" complete");
    ECcal = 0;
  }

}

void Reset_EC()
{
  ECcal = 1;
  int eeAddress = 0;

  Serial.print("Reset EC ...");

  MyObject customVar = {
    230,
    1245,
    5282,
    17255
  };

  EEPROM.put(eeAddress, customVar);

  EEPROM.get(eeAddress, customVar);

  Y0 = (customVar.Y0);
  Y1 = (customVar.Y1);
  Y2 = (customVar.Y2);
  Y3 = (customVar.Y3);
  Serial.println(" complete");
  ECcal = 0;
}


void loop()
{

  if (ECcal == 0)
  {
    if (millis() - Time >= Interval)
    {
      Time = millis();

      sequence ++;
      if (sequence > 1)
        sequence = 0;


      if (sequence == 0)
      {
        pulseCount = 0; //reset the pulse counter
        attachInterrupt(0, onPulse, RISING); //attach an interrupt counter to interrupt pin 1 (digital pin #3) -- the only other possible pin on the 328p is interrupt pin #0 (digital pin #2)
      }

      if (sequence == 1)
      {
        detachInterrupt(0);
        pulseCal = pulseCount;
        temp_read();
        ECread();

        // Prints measurements on Serial Monitor
        Serial.println("  ");
        Serial.print("t ");
        Serial.print(temp);
        Serial.print(F(" *C"));
        Serial.print("    E.C. ");
        Serial.print(EC); // uS/cm
        Serial.print("    pulses/sec = ");
        Serial.print(pulseCal);
        Serial.print("    C = ");
        Serial.println(C); // Conductivity without temperature compensation
      }
    }

    if (Serial.available() > 0) //  function of calibration E.C.
    {
      incomingByte = Serial.read();
      cal_sensors();
    }
  }
}

