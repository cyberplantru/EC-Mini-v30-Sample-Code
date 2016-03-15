![EC Reader](http://image.cyber-plant.com/var/albums/miniSeriese.jpg?m=1458062565)
##CyberPlant E.C. mini v3.0

The E.C. mini v3.0 based on the precision timer NE555DR. The Conductivity circuit provides the AC excitation. The output signal then is a square wave. Its frequency varies from 240 Hz when the sensor is bone dry, up to 16000 Hz at the E.C. 80.000 μS/cm. This output can measured using the Interrupts. For temperature compensation can used Waterproof DS18B20 temperature sensor. The E.C. Reader provides high accuracy readings of conductivity measurement.

##Features:

- Power Supply: 5V - 15V
- Measuring Range EC: 0 - 160.000 μS/cm
- Accurate EC readings: ±2%
- Support Temp sensor DS18B20
- BNC Connector
- Can use as Shield for Arduino Pro Mini
- PCB Size : 33.02 mm×17.78 mm


## EC measurement
For high accuracy of conductivity measurements use the temperature compensation.
Connect the ds18b20 temperature sensor and solder a pull-up resistor 4k7. 

For pull-up Interrupts solder a second 4k7 resistor. You can enable the internal pull-up resistor in microcontroller,  but the result will be less stable. Provide supply least 200 mA to measure the full range of conductivity. Connect the module to D2/D3 pins Arduino Uno via cable or use PLS connectors for Arduino Pro Mini. Connect the EC electrode. 

![EC Reader](http://image.cyber-plant.com/var/resizes/ECminiBaner1.jpg?m=1458062720)

See the sample code *ECMeasurementSerial.ino*

The sample code uses the sensor calibration feature with one touch.
![EC Reader](http://image.cyber-plant.com/var/resizes/ECSerial.jpg?m=1449125597)

## Isolated sensor
If you need to isolate the E.C. Counter can use the [I2Ciso](https://github.com/cyberplantru/I2C-iso). When reading the pH and Conductivity together, recommended to isolate the pH circuit and leave the E.C. unisolated.

![EC Reader](http://image.cyber-plant.com/var/resizes/pH-EC-meter-kit_3.jpg?m=1458063353)


----------


[link to CyberPlant](http://www.cyber-plant.com).