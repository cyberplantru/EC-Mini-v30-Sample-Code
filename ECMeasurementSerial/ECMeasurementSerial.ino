/*
  Example code for the E.C. mini v3.0
  http://www.cyber-plant.com
  by CyberPlant LLC, 03 December 2015
  This example code is in the public domain.
  upd. 15 March 2016
*/
#include <SimpleTimer.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define alphaLTC 0.022 // The linear temperature coefficient
#define ONE_WIRE_BUS 3 // Connect DS18B20 temp sensor to pin D3

#define X0 0.0 //calibration buffers. You can use any other buffers
#define X1 2.0
#define X2 12.88
#define X3 80.0
unsigned int Y0; // calibration value
unsigned int Y1;
unsigned int Y2;
unsigned int Y3;

float EC;
float Temp;
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
  //pinMode(2, INPUT_PULLUP); //  An internal 20K-ohm resistor is pulled to 5V. If you use hardware pull-up delete this
  Serial.println("Calibrate commands:");
  Serial.println("E.C. :");
  Serial.println("      Cal. 0,000 uS ---- 0");
  Serial.println("      Cal. 2,000 uS ---- 1");
  Serial.println("      Cal. 12,880 uS --- 2");
  Serial.println("      Cal. 80,000 uS --- 3");
  Serial.println("      Reset E.C. ------ 5");
  Serial.println("  ");
  ReadEE();
  timer.setInterval(1000L, TotalEvents);
  attachInterrupt (0, eventISR, FALLING);
}

struct MyObject {
  unsigned int Y0;
  unsigned int Y1;
  unsigned int Y2;
  unsigned int Y3;
};

void ReadEE()
{
  int eeAddress = 0;
  MyObject customVar;
  EEPROM.get(eeAddress, customVar);
  Y0 = (customVar.Y0);
  Y1 = (customVar.Y1);
  Y2 = (customVar.Y2);
  Y3 = (customVar.Y3);
}

void SaveSet()
{
  int eeAddress = 0;
  MyObject customVar = {
    Y0,
    Y1,
    Y2,
    Y3
  };
  EEPROM.put(eeAddress, customVar);
}

void eventISR ()
{
  if (counting == true)
    events++;
}

void TotalEvents()
{
  if (counting == true) {
    counting = false;
    total = events;
    TempRead();
  }
  else if (counting == false) {
    noInterrupts ();
    events = 0;
    EIFR = bit (INTF0);
    counting = true;
    interrupts ();
  }
}

void TempRead()
{
  sensors.requestTemperatures();
  Temp = sensors.getTempCByIndex(0);
  if (-20 > Temp || Temp > 200) {
    Temp = TempManual;
  }
  ECcalculate();
}

void ECcalculate()
{
  float A;
  float B;
  float C;

  if (total < Y0)
    C = 0;
  else if (total >= Y0 && total < (Y0 + Y1))
  {
    A = (Y1 - Y0) / (X1 - X0);
    B = Y0 - (A * X0);
    C = (total  - B) / A;
  }
  else if (total >= (Y0 + Y1) && total < (Y2 + Y1 + Y0))
  {
    A = (Y2 - Y1) / (X2 - X1);
    B = Y1 - (A * X1);
    C = (total  - B) / A;
  }
  else if (total >= (Y2 + Y1 + Y0))
  {
    A = (Y3 - Y2) / (X3 - X2);
    B = Y2 - (A * X2);
    C = (total - B) / A;
  }

  EC = (C / (1 + alphaLTC * (Temp - 25.00)));

  Serial.print("E.C. ");
  Serial.print(EC, 2);
  Serial.print("  t ");
  Serial.print(Temp);
  Serial.println(" *C");
  Serial.println("");
}

void calECprobe() // calibration E.C. probe
{
  switch (incomingByte)
  {
    case 48: // key "0"
      Serial.print("Cal. 0,00 uS ...");
      Y0 = total / ((1 - ((Temp - 25.00) / 350)) + alphaLTC * (Temp - 25.00));
      break;

    case 49: // key "1"
      Serial.print("Cal. 2,00 uS ...");
      Y1 = total / ((1 - ((Temp - 25.00) / 350)) + alphaLTC * (Temp - 25.00));
      break;

    case 50: // key "2"
      Serial.print("Cal. 12,88 uS ...");
      Y2 = total / ((1 - ((Temp - 25.00) / 350)) + alphaLTC * (Temp - 25.00));
      break;

    case 51: // key "3"
      Serial.print("Cal. 80,000 uS ...");
      Y3 = total / ((1 - ((Temp - 25.00) / 350)) + alphaLTC * (Temp - 25.00));
      break;

    case 53: // key "5"
      Serial.print("Reset uS ...");
      Y0 = 235;
      Y1 = 1340;
      Y2 = 5150;
      Y3 = 16800;
      break;
  }
  Serial.println(" complete");
  SaveSet();
}

void loop()
{
  if (Serial.available() > 0)
  {
    incomingByte = Serial.read();
    calECprobe();
  }
  timer.run();
}
