# efm8-arduino-programmer
Program EFM8 devices using an Arduino Mega.

Thanks to jaromir-sukuba and racerxdl for working on firmware to implement C2 protocol via arduino GPIO.  This work largely pulls from them.

## Pre-Steps
Currently, it is for Arduino Mega and maps C2D and C2CK to digital pins 2 and 3, respectively. 

C2 is a 2-pin protocol.  Any arduino should work to implement the protocol via GPIO.  Just need to make sure that the correct pins are mapped for your Arduino.

### Update Pins
Check the [firmware file](https://github.com/conorpp/efm8-arduino-programmer/blob/master/prog/prog.ino#L11) and change the pins to map to your device if needed.

### Arduino Uno support
To use it for Arduino Uno you can just swap from E to D. Just replace all `PORTE`, `DDRE` and `PINE` with `PORTD`, `DDRD` and `PIND`. You can read about Port Registers here: https://www.arduino.cc/en/Reference/PortManipulation

### Write firmware to Arduino
Program the firmware to the arduino and connect C2D, C2CK, and GND to your target device.

## Setup for flashing EFM8
It is set up in a client/server model to be able to easily support programming multiple targets at the same time.

### Requirements
- You need to have Python (2.7) installed.
- Then, install some required python modules.

```
pip install -r requirements.txt
```

### Setup of Server & Client

#### Server 
First you must run the server that will handle communication to the arduino.

```
python prog_server.py <serial-port> [<serial-port2> ...]
```

Supply a list of serial ports (you can use more than one Arduino at a time) to handle programming.  E.g /dev/USB1 or COM1 or /dev/cu.usbmodem*.

#### Client
Then you can finally program something using the client script.

```
python prog_client.py <serial-port> <firmware.hex>
```

This will connect to the server and tell it to download specified firmware via specified arduino.

## Troubleshooting

- If your server can't start make sure you have port 4040 available
- If you get python errors make sure you're not running python3
- Some modules need sudo on some systems