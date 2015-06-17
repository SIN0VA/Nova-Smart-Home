# Nova Smart Home
##Introduction 
This is my final year proejct which is a design (both software and hardware) for a Home Automation system, where a Raspberry Pi will act as the central board for controling a bunch of Arduino UNO  cards (RF Nodes) ; I made 4 types of these RF nodes .
  -RF Switches (to switch any electrical appliance, it's implemented here for lights only).
  -PIR Nodes (A bunch of Arduino's coupled with PIR detectors for autolights).
  -A Sensor Node (An Arduino with special Sensor Shield : temperature ,humidty and somke detection ).
  -An HVAC Controller (An Arduino with my HVAC Shield to control the "Conventional HVAC System" of the house).
I should note that the communication between the Raspberry Pi and the other nodes is achieved using the NRF24L01  RF module, each type of the RF nodes has its own Shield, except the light switch which is a RF light switch based on an Atmega328p MCU and a relay (compatible with Arduino).

## Getting Started
Just to introduce the workings of this project a little bit, there are two programs ; a Node.JS app  which is an Express Web Server so people can turn off/on lights and things like that.If you're familiar with NodeJS Express check the two main files of the server the [ExpressJS Routes](HomeControlServer/routes/index.js) file and the [ExpressJS App ](HomeControlServer/app.js) file.
The second app is a [Control Daemon](ControlDaemon/NovaHomeDaemon.cpp) coded with C++, this one controls and handles the requests from NodeJS Web Server (IPC via ZMQ sockets) and other RF nodes (via NRF24L01), it also performs the Home Automation side of things.
## IMPORTANT NOTE ! :
You can't sell, publish any of the code in here, it's free software under the GPLv3 licence.
Read the licence for more information.
When you use this project or any code of it especially in an academic paper, you have to link to this website.
A. Benabdallah 2015.
