# Daily Reminder

A small toy thing to remind myself of daily tasks.

It consists of a hardware piece as human interface and a stupid server to keep
track of the state.

This is mostly a toy for myself because I wanted to use the lolin board for
something. If you're interested in this and the code / documentation is lacking,
feel free to open an issue :-)

## Parts List

* A microcontroller with an ESP8266 Wifi module, I'm using a NodeMCU Lolin v3
* Two LEDs (e.g. a green and a red one)
* Resistors for the LEDs
* A button

## The hardware piece

The circuit is very simple, just connect the LEDs to ditigal outputs of your
micro controller. The lolin has 3.3V outputs, so I got away without any
resistors, YMMV. For the button, make sure to connect it to a digital I/O that
supports interrupts. D7 on the lolin works for me.

Customize `config.h` and then upload the client program.

## The server software

It's really stupid yet. Doesn't even store the last state, but works.
