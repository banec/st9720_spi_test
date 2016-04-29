# Simple test for ST9720 LCD and Raspberry Pi

This is sample program to test connectivity of ST9720 LCD controller connected to Raspberry Pi.

Upon starting up, program will reset the display, clear graphic ram, and display Raspberry logo in GFX mode. 
There is no test of text mode as it will be used in graphics mode only (text mode is HD4770 emulation and looks exceptionally ugly on display of this size).
To compile this you will need working GCC installed and [WiringPi library](http://wiringpi.com/).

### Connecting to Raspberry Pi

Since Raspberry Pi has 3.3v IO, you should use level shifter so you don't burn your Pi.

RPi        | Level Converter | LCD
-----------|-----------------|----
1  (3v3)   | --------------> | BLA
2  (5v)    | --------------> | Vcc
4  (5v)    | -> 5v IN        |
6  (GND)   | -> GND IN       |  
9  (GND)   | --------------> | GND
16 (GPIO4) | --------------> | RST
19 (MOSI)  | -> Converter -> | R/W (MISO)
20 (GND)   | --------------> | PSB
23 (SCLK)  | -> Converter -> | E (SCK)
24 (CE0)   | -> Converter -> | RS (CS)
30 (GND)   | --------------> | BLK

As always with Chinese eBay parts, you might get one with different labeling, in which case you need to figure out where are MISO, CS and CLK pins.

D0-D8 pins do not interest us, since we are not using parallel to drive this display. This means that we need to tie PSB to ground so controller will switch to SPI mode.
You can also tie BLA pin to any free PWM modulated output on Pi in order to get nice dimmable backlight. This is left out from this simple demo, but you can easily add it in with WiringPi library.