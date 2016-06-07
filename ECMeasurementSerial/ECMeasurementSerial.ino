/*
  Example code for the E.C. Mini v3.0
  Works in Arduino IDE 1.6.8
  http://www.cyber-plant.com
  by CyberPlant LLC, 03 December 2015
  This example code is in the public domain.
  upd. 27.03.2016
*/
#include <SimpleTimer.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define alphaLTC 0.022 // Temperature correction coefficient
#define ONE_WIRE_BUS 3 // Connect DS18B20 temp sensor to pin D3

#define X0 0.00   //  The conductivity probe is dry
#define X1 2.00   //  Value of calibration buffers
#define X2 12.88  //  You can use any other buffers
#define X3 80.00  //  and their number

unsigned int Y0, Y1, Y2, Y3;  //  The variables of calibration buffer values
float EC, Temp;
float TempManual = 25.0;
volatile bool counting;
volatile unsigned long events;
unsigned long total;
int incomingByte = 0;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP); //  An internal 20K-ohm resistor is pulled to 5V. If you use hardware pull-up delete this
  ReadEE(); //  Get calibration values from EEPROM
  Serial.println("E.C. Mini v3.0");
  Serial.println("\n\      Cal. 0.00 mS/cm --> 0");
  Serial.println("      Cal. 2.00 mS/cm --> 1");
  Serial.println("      Cal. 12.88 mS/cm -> 2");
  Serial.println("      Cal. 80.00 mS/cm -> 3");
  Serial.println("      Reset ------------> 5");
  for (int i = 0; i < 14; i++) {
    Serial.print(". ");
    delay(100);
  }
  Serial.println();
  timer.setInterval(1000L, TotalEvents);
  attachInterrupt (0, eventISR, FALLING);
}

struct MyObject { // Array for storage calibration values in the EEPROM
  unsigned int Y0;
  unsigned int Y1;
  unsigned int Y2;
  unsigned int Y3;
};

void ReadEE() {  //  Get calibration values from EEPROM
  int eeAddress = 0;
  MyObject customVar;
  EEPROM.get(eeAddress, customVar);
  Y0 = (customVar.Y0);
  Y1 = (customVar.Y1);
  Y2 = (customVar.Y2);
  Y3 = (customVar.Y3);
}

void SaveSet() {  //  Save calibration values to EEPROM
  int eeAddress = 0;
  MyObject customVar = {
    Y0,
    Y1,
    Y2,
    Y3
  };
  EEPROM.put(eeAddress, customVar);
  Serial.println(". . . complete");
}

void eventISR () {  //  Get the value of frequency from conduction circuit
  if (counting == true)
    events++;
}

void TotalEvents() {  //  Turn on/off counter of interrupts
  if (counting == true) {
    counting = false;
    total = events;
    TempRead();
  } else if (counting == false) {
    noInterrupts ();
    events = 0;
    EIFR = bit (INTF0);
    counting = true;
    interrupts ();
  }
}

void TempRead() {  //  Get temperature value from DS18B20
  sensors.requestTemperatures();
  Temp = sensors.getTempCByIndex(0);
  if (-20 > Temp || Temp > 200) {
    Temp = TempManual;
  }
  ECcalculate();
}

void ECcalculate() {  //  Calculate the measurement

  float A;
  float B;
  float C;

  if (total < Y0)
    C = 0;
  else if (total >= Y0 && total < (Y0 + Y1)) {
    A = (Y1 - Y0) / (X1 - X0);
    B = Y0 - (A * X0);
    C = (total  - B) / A;
  }
  else if (total >= (Y0 + Y1) && total < (Y2 + Y1 + Y0)) {
    A = (Y2 - Y1) / (X2 - X1);
    B = Y1 - (A * X1);
    C = (total  - B) / A;
  }
  else if (total >= (Y2 + Y1 + Y0)) {
    A = (Y3 - Y2) / (X3 - X2);
    B = Y2 - (A * X2);
    C = (total - B) / A;
  }

  EC = (C / (1 + alphaLTC * (Temp - 25.00))); //  temperature compensation

  Serial.println(' ');
  Serial.print("  t ");
  Serial.print(Temp, 1);
  Serial.print("*C ");
  Serial.print("  EC ");
  Serial.print(EC, 2);
  Serial.print("   PPM ");
  Serial.print(EC * 500, 0);
  Serial.print("   Frequency ");
  Serial.print(total);
  Serial.println(" Hz");
}

void calECprobe() // calibration probe
{
  Serial.println(' ');
  switch (incomingByte)
  {
    case 48: // key "0"
      Serial.print("  Cal. 0.00 mS/cm ");  //  The conductivity probe is dry
      Y0 = total;
      SaveSet();
      break;

    case 49: // key "1"
      Serial.print("  Cal. 2.00 mS/cm ");
      Y1 = total / (1 + alphaLTC * (Temp - 25.00));
      SaveSet();
      break;

    case 50: // key "2"
      Serial.print("  Cal. 12.88 mS/cm ");
      Y2 = total / (1 + alphaLTC * (Temp - 25.00));
      SaveSet();
      break;

    case 51: // key "3"
      Serial.print("  Cal. 80.00 mS/cm ");
      Y3 = total / (1 + alphaLTC * (Temp - 25.00));
      SaveSet();
      break;

    case 53: // key "5"
      Serial.print("  Reset ");
      Y0 = 230;
      Y1 = 1340;
      Y2 = 4140;
      Y3 = 7350;
      SaveSet();
      break;
  }
}

void loop()
{
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    calECprobe();
  }

  timer.run();
}
