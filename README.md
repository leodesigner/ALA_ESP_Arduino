# ALA for ESP8266 
Arduino Light Animation (ALA) is a library for Arduino boards to simplify the development of light animations using LEDs and LED strips.

More details, examples and projects can be found [here](http://yaab-arduino.blogspot.com/p/ala.html).

- Adapted for ESP8266
- added new commands (FADETO preset)
- added support for inverse led connection
- changed animation logic a bit.
- TLC5940 is disabled at the moment.

##Features

- Pure Arduino implementation. No external software is needed on the PC to design and drive light animations.
- Support for monochrome LED, RGB LED, multiple LEDs and addressable RGB LED strips using **PWM output pins**, **WS2812 RGB LED strips** and **TLC5940 chips**.
- Lots of cool animations ready to use.
- Precise timing to allow synching animations to music.
- No use of delay() to allow to drive multiple channels and lines independently.

