# Flashing using Arduino CLI

## Installing arduino-cli

### Windows

Download and install from [here](https://arduino.github.io/arduino-cli/1.0/installation/#download), selecting Windows and probably 64 bit.

### MacOS / Linux

Either use brew or your distro's package manager or:

```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```

## Running

In the sketch directory (this directory) run `arduino-cli upload`.

This uses the [`sketch.yaml`](./sketch.yaml) to download required libraries for board and sketch.
