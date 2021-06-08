# mipos

Midi Clock to sync with Pocket Operators and some LED status, heavily inspired (and partly borrowed) by [mirg](https://github.com/jpenca/mirg).

## What is it?

This is a simple program for the Raspberry Pi. It listens to MIDI clock/start/stop on a USB port, provides korg-style pulse sync on GPIO. For example, this lets you sync Korg Volca, Teenage Engineering Teeange Engineering OP-XX or Pocket Operators to any device which provides clock via USB, such as Elektron Analog series, Model Series or Teenage Engineering OP-1.

## Installation

### Dependencies

libasound2-dev is needed for USB-MIDI:

    sudo apt-get install libasound2-dev

this bcm2835 library is needed to use the GPIO pins: <http://www.airspayce.com/mikem/bcm2835/index.html>

Follow the instructions on that site for installation.

### Compile mipos

     gcc -o mipos mipos.c -lbcm2835 -lasound

## Usage

Connect the USB-MIDI device you want to use as master clock before starting mipos.
Find out the ALSA ID of your MIDI device using amidi:

    amidi -l

This will list all connected devices. Use your chosen device, e.g. `hw:1,0,0` as an argument to mipos:

    ./mipos hw:1,0,0

Make sure your MIDI device sends clock/start/stop via USB. Press play on your MIDI device. Mipos creates a Korg-style pulse every 8th-note (once every 12 MIDI clock ticks) on GPIO pin 18. You'll need to connect a minijack cable to that pin, e.g. use a standard modular patch cable and cut off one end. Connect tip to pin 18, connect sleeve to ground.

GPIO pin 17 is used to indicate if the program is running and if it is connected to your MIDI device. You can connect a LED to this pin. If it is not connected or it cannot find the device, the power LED wil blink. If everything works as expected, the LED wil remain lit. This allows you to automaticly start mipos during boot and have a visual indiciation if everything is working correctly.

## Special thanks

Special thanks go out to Jakob Penca, wo created [mirg](https://github.com/jpenca/mirg), which was the original base for this code.
