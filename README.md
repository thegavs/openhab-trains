# openhab-trains
Openhab-trains.  Combining the power of home automation systems and cheap arduinos to provide open source modules for running model trains.

Designed for Garden railways where dcc running over tracks proves problematic but will work with smaller scales.  The wifi modules have been 
tested at over 200 feet (60 metres) both inside and outside.
All modules are designed to run off 12v stepped down to 5v on board.   Most modules could also be modified to run on battery power.

Using the home automation system allows for complex rules to be set up, allowing electronic interlocking to be used for route setting. 

The Aim for this project is for people to be able to build simple cheap modules to control their layout and develop and add other modules 
to expand the system.



The core of the system is Openhab2 running on a Raspberry Pi. 

https://www.openhab.org/

Openhab is open source and free to use.

For ease of install I suggest using Openhabian.

https://www.openhab.org/docs/installation/openhabian.html



The modules are arduinos

https://www.arduino.cc/

I suggest using the nodemcu exp8266 or esp32 modules as they are inexpensive and powerful.



Video of first two modules in action.

https://youtu.be/3NVWC1UaGtM

**Modules published.**

Levers - reads the position of levers and transmits value to server.

signal - moves two semaphore signals.

points - moves 3 points.


**Modules in development.**


ground signal - for two ground signals.


Brushless - allows control of brushless motors

