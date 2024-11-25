# Firmware for extruder control

The firmware for the Arduino controlling the stepper motor and heater can
programmed using Arduino IDE.

[Downloading and installing the Arduino IDE 2](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-downloading-and-installing)

There are additional notes here on how to set it up for Controllino Mini since
that's used in the setup. See [Controllino User Manual](https://www.controllino.com/wp-content/uploads/2023/07/CONTROLLINO-Instruction-Manual-V1.3-2023-05-15.pdf) for more details.

## Additional requirements

### Libraries

The stepper_heater firmware requires a few libraries. They can be
installed with the Arduino IDE library manager.

[Installing libraries | Arduino Documentation](Installing libraries)

- [AccelStepper](https://www.arduino.cc/reference/en/libraries/accelstepper/)
- [Controllino](https://www.arduino.cc/reference/en/libraries/controllino/) (if using Controllino)

### Boards

Set board using Tools > Board. If you are using a non standard board (like
Controllino) you need install that too.

#### Add new URL to board manager

[Add or remove additional Board Manager URLs](https://support.arduino.cc/hc/en-us/articles/360016466340-Add-or-remove-third-party-boards-in-Boards-Manager?queryID=95df4328c1ab5b8d023c6897b359ab4d#add-or-remove)

For Controllino that URL is <https://raw.githubusercontent.com/CONTROLLINO-PLC/CONTROLLINO_Library/master/Boards/package_ControllinoHardware_index.json>

#### Install new board

[Installing a Core in the IDE 2](https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-board-manager)

## Flashing/programming the Arduino

After making changes to code it needs to be uploaded to the Arduino (also known
as programming or flashing).

[How to Upload a Sketch with the Arduino IDE 2](https://docs.arduino.cc/software/ide-v2/tutorials/getting-started/ide-v2-uploading-a-sketch)

## Modifications

Close to the top of the file there's two important constants that you can
change:

```c++
const int HOTEND_TEMP_DEGREES_C = 210;
const int EXTRUDER_RPM = 60;
```
