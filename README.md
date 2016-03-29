![CyberPlant_Mini](http://image.cyber-plant.com/var/resizes/CyberPlantMiniSeriesC.jpg?m=1459175705)

##CyberPlant EC Mini v3.0
The EC mini v3.0 based on the precision timer NE555DR. The Conductivity circuit provides the AC excitation. The output signal then is a square wave. Its frequency varies from 240 Hz when probe is dry and up to 8950 Hz at the EC 80.00 mS/cm. This output signal can be measured using the interrupts. For temperature compensation can used waterproof DS18B20 temperature sensor. The circuit provides high accuracy readings of Conductivity measurement. Can be used with any conductive probe with BNC connector. Use EC Mini as Shield with Arduino Pro Mini for transmit the data through UART to any device
###Features:

- Power Supply: 5V - 15V
- Measuring Range EC: 0 - 160.00 mS/cm
- Accurate EC readings: ±2%
- Support Temp sensor DS18B20
- BNC Connector
- Can use as Shield for Arduino Pro Mini
- PCB Size : 33.02 mm×17.78 mm

![EC Reader](http://image.cyber-plant.com/var/resizes/ECminiBaner1.jpg?m=1458062720)

### E.C. measurement
For high accuracy of Conductivity measurements use the temperature compensation.
Connect the ds18b20 temperature sensor and solder a pull-up resistor 4k7. 
For pull-up Interrupts solder a second 4k7 resistor. You can enable the internal pull-up resistor in microcontroller. Provide supply least 200 mA to measure the full range of Conductivity. Connect the module to D2/D3 pins Arduino Uno via cable or use PLS connectors for Arduino Pro Mini

**Sample code:** [*ECMeasurementSerial.ino*](https://github.com/cyberplantru/EC-Mini-v30-Sample-Code/blob/master/ECMeasurementSerial/ECMeasurementSerial.ino)

The sample code uses the probe calibration feature with one touch.

![EC Reader](http://image.cyber-plant.com/var/resizes/ECSerial-01.jpg?m=1459219573)

### Galvanic Isolation
If you need to isolate the E.C. Mini can use the [I2Ciso](https://github.com/cyberplantru/I2C-iso). When reading the pH and Conductivity together, recommended to isolate the pH circuit and leave the E.C. unisolated.

![EC Reader](http://image.cyber-plant.com/var/resizes/pH-EC-meter-kit_3.jpg?m=1458063353)


----------

### Package include:

-     1 x E.C. Mini v3.0
-     2 x 10 PLS connectors
-     1 x 4 angle PLS
-     2 x 4k7 resistors

----------
