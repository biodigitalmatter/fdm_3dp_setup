# Firmware for extruder control

The firmware for the Arduino controlling the stepper motor and heater can
programmed using Arduino IDE.

[Downloading and installing the Arduino IDE 2 | Arduino Documentation](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing)

## Additional requirements

The stepper_heater firmware requires the library AccelStepper. It can be
installed with the Arduino IDE library manager.

[Installing libraries | Arduino Documentation](Installing libraries)

## Flashing/programming the Arduino

After making changes to code it needs to be uploaded to the Arduino (also known
as programming or flashing).

[How to Upload a Sketch with the Arduino IDE 2](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-uploading-a-sketch)

## Modifications

Close to the top of the file there's two important constants that you can
change:

``` c++
const int HOTEND_TEMP_DEGREES_C = 210;
const int EXTRUDER_RPM = 60;
```
