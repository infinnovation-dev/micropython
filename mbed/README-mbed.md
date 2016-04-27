This a port of [MicroPython](http://micropython.org) to the mbed Classic
platform.

This provides an interpreter running on the board's USB serial
connection.

Getting Started
---------------

Import the micropython-repl program into your IDE workspace on
developer.mbed.org.
Compile and download to your board.
Connect to the USB serial port in your usual manner.
You should get a startup message similar to the following:
```
  MicroPython v1.7-155-gdddcdd8 on 2016-04-23; K64F with ARM
  Type "help()" for more information.
  >>>
```
Then you can start using micropython.  For example:
```
  >>> from mbed import DigitalOut
  >>> from pins import LED1
  >>> led = DigitalOut(LED1)
  >>> led.write(1)
```

Requirements
------------

You need approximately 100K of flash memory, so this will be no good
for boards with smaller amounts of storage.

Caveats
-------

- This can be considered an alpha release of the port; things may not
  work; APIs may change in later releases.  It is NOT an official part
  part the micropython project, so if anything doesn't work, blame me.
  If it does work, most of the credit is due to micropython.

- Only a few of the mbed classes are available in micropython so far,
  and not all methods of those that are.

- Only a few boards have their full range of pin names available; for
  others, only a few standard ones (USBTX, USBRX, LED1) are
  implemented.

- The garbage collector is not yet implemented.  The interpreter will
  gradually consume memory and then fail.

- Exceptions from the mbed classes are not yet handled.

- Asynchronous processing (e.g. events on inputs) is not supported.

More Information
----------------

Further information may be available on the [mbed MicroPython wiki](https://github.com/infinnovation/micropython/wiki)

Credits
-------

- Damien P. George and other contributors who created micropython.

- Colin Hogben, author of this port.
