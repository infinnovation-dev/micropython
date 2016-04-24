MicroPython port to mbed Classic platform
=========================================

This is a port of micropython to the mbed Classic platform, in
particular the online IDE and code repository hosted on
developer.mbed.org.

It differs from other ports in that compilation is not done within the
main tree.  Instead, we package up some files and generate others in
order to populate an alternative tree which may be used within the
mbed ecosystem, in particular the online IDE.
