<img align="left" src=https://raw.githubusercontent.com/chrismec/octo_max31855/master/octo_max31855_board.jpg alt="MAX31855 Breakout Board">

# Octo MAX31855 Breakout Board -=[ESP HOME]=-


Board appears to be made by Whizoo.com and sold by Neem-Tech.

The breakout board marries a MAX31855 with a 74HC4051 Multiplexer, making an eight channel thermocouple breakout board.

This repository allows integration of this breakout board with ESPHome as an external component.

To install:
1. Create a mycomponents folder in your esphome directory and add this repo there
2. Add your device in ESPhome and configure the example yaml file to your specifications

Notes:
1. The mux_delay time, in my experience, needs to be pretty long in order to get the proper readings. Upwards of 80mSec or more seems to be necessary. This long switching time will cause warnings to be issued from the sensor component. I have not investigated this delay and do not plan to.
2. This external component returns °F.

To do:
1. Move the `cs_pin` config from the sensor to the component, so that it could be setup once in the component, rather than be repeated in every sensor.