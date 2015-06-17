# Nova Smart Home
##Introduction 
This is a Home Automation project that I did for my final year, it uses a Raspberry Pi that acts as a central board for controling a bunch of Arduino UNO  cards (RF Nodes). For this project I made 4 types of these RF nodes.  
  1. [RF Switches](Arduino Sketches and Schematics/Arduino RF Controlled Lightswitch/) (to switch any electrical appliance, it's implemented here for lights only).  
  2. PIR Nodes (A bunch of Arduino's coupled with PIR detectors for autolights).  
  3. A Sensor Node (An Arduino with special [Sensor Shield](https://github.com/SIN0VA/Nova-Smart-Home/tree/master/Arduino%20Sketches%20and%20Schematics/Arduino%20Sensor%20Node) : temperature, humidty and somke detection ).  
  4. An HVAC Controller (An Arduino with a special [HVAC Shield](Arduino Sketches and Schematics/Arduino Sensor Node/) to   control a "Conventional HVAC System"). I haven't tested this one, so you should tread carefully. 

The communication between the Raspberry Pi and the other nodes is achieved using the NRF24L01 RF module, each type of the RF nodes has its own Shield, except the light switch which is a RF light switch based on an Atmega328p MCU and a relay (compatible with Arduino).
## Software Stack 
### Arduino Sketches
Each RF node has its owns sketch, the two types of nodes that send data to the Raspberry Pi are the PIR Node and the Sensor Node, the other two (the HVAC controller and the RF light switch) will only recieve commands from the Raspberry Pi.  
1. [RF Switch Sketch](Arduino Sketches and Schematics/Arduino RF Controlled Lightswitch/RF LightSwitch/RF LightSwitch.ino).  
2. [PIR Node Sketch](Arduino Sketches and Schematics/Arduino RF Motion Detection/Motion Detection/Motion Detection.ino).  
3. HVAC Controller Sketch (haven't made this one).  
4. [Sensor Node Sketch](Arduino Sketches and Schematics/Arduino Sensor Node/SensorNode/SensorNode.ino).  

If you can't make the Shields, just follow the schematics of every Shield and use wires (but you have to make the RF Switch to control lights).
### Raspberry Pi
Just to introduce the workings of this project a little bit, there are two programs ; a Node.JS app  which is an Express Web Server so people can turn off/on lights and things like that, If you're familiar with NodeJS Express check the two main files of the server the [ExpressJS Routes](HomeControlServer/routes/index.js) file and the [ExpressJS App ](HomeControlServer/app.js) file.
The second app is a [Control Daemon](ControlDaemon/NovaHomeDaemon.cpp) coded with C++, this one controls and handles the requests from NodeJS Web Server (IPC via ZMQ sockets) and other RF nodes (via NRF24L01), it also performs the Home Automation side of things.
## IMPORTANT NOTE ! :
You can't sell, publish any of the code in here, it's free software under the GPLv3 licence.
Read the licence for more information.
When you use this project or any code of it especially in an academic paper, you have to link to this website.
A. Benabdallah 2015.
