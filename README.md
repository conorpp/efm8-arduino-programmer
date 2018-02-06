# efm8-arduino-programmer
Program EFM8 devices using an arduino mega ou uno

Thanks to jaromir-sukuba and racerxdl for working on firmware to implement C2 protocol via arduino GPIO.  This work largely pulls from them.
Thanks to Conor Patrick
https://github.com/conorpp/efm8-arduino-programmer


# Setting up

C2 is a 2-pin protocol.  Any arduino should work to implement the protocol via GPIO.  Just need to make sure that the correct pins are mapped for your Arduino.  Check the [firmware file Arduino Mega](https://github.com/christophe94700/efm8-arduino-programmer/blob/master/prog/arduino_mega.ino#L11) or [firmware file Arduino Uno](https://github.com/christophe94700/efm8-arduino-programmer/blob/master/prog/arduino_uno.ino#L11)and change the pins to map to your device if needed.  Currently, it is:
- for Arduino Mega and maps C2D and C2CK to digital pins 2 and 3, respectively.
- for Arduino Uno and maps C2D and C2CK to digital pins 5 and 6, respectively.

Program the firmware to the arduino and connect C2D, C2CK, and GND to your target device.

### Software

You need to have Python installed.  Then, install some required python modules.Use Python 2.7 and Pyserial

```
pip install -r requirements.txt
```

# Running

Programming one target.

```
py flash.py <serial-port> <firmware.hex>
```

For the serial ports : E.g /dev/USB1 (Linux) or COM1 (Windows).
