# Nova Smart Home
##Introduction 
This is a Home Automation project that I did for my final year, it uses a Raspberry Pi that acts as a central board for controling a bunch of Arduino UNO  cards (RF Nodes). For this project I made 4 types of these RF nodes.  

  1. [RF Switches](Arduino Sketches and Schematics/Arduino RF Controlled Lightswitch/) (to switch any electrical appliance, it's implemented here for lights only).  
  2. PIR Nodes (A bunch of Arduino's coupled with PIR detectors for autolights).  
  3. A Sensor Node (An Arduino with special [Sensor Shield](https://github.com/SIN0VA/Nova-Smart-Home/tree/master/Arduino%20Sketches%20and%20Schematics/Arduino%20Sensor%20Node) : to collect readings on temperature, humidty and somke detection ).  
  4. An HVAC Controller (An Arduino with a special [HVAC Shield](Arduino Sketches and Schematics/Arduino Sensor Node/) to   control a "Conventional HVAC System"). I haven't tested this one, so you should tread carefully.  
<p align="center">
![image-1](https://cloud.githubusercontent.com/assets/12392615/8198000/2fb06950-1498-11e5-875a-83fd387744ab.png)  
</p>  

The communication between the Raspberry Pi and the other nodes is achieved using the NRF24L01 RF module, each type of the RF nodes has its own Shield, except the light switch which is a RF light switch (Arduino compatible) based on an Atmega328p MCU and a relay.  

## Software Stack 
### Arduino Sketches
Each RF node has its owns sketch, the two types of nodes that send data to the Raspberry Pi are the PIR Node and the Sensor Node, the other two (the HVAC controller and the RF light switch) will only recieve commands from the Raspberry Pi.  

1. [RF Switch Sketch](Arduino Sketches and Schematics/Arduino RF Controlled Lightswitch/RF LightSwitch/RF LightSwitch.ino).  
2. [PIR Node Sketch](Arduino Sketches and Schematics/Arduino RF Motion Detection/Motion Detection/Motion Detection.ino).  
3. HVAC Controller Sketch (haven't made this one).  
4. [Sensor Node Sketch](Arduino Sketches and Schematics/Arduino Sensor Node/SensorNode/SensorNode.ino).  

If you can't make the Shields, just follow the schematics of every Shield and use wires (but you have to make the RF Switch to control lights).
### Raspberry Pi  
<p align="center">
<img src="https://cloud.githubusercontent.com/assets/12392615/8198416/d4577340-149d-11e5-90c2-8f8dbe72cc0a.png">  
</p>
Just to introduce the workings of this project a little bit, there are two programs ; a Node.JS app  which is an Express Web Server so people can turn off/on lights and things like that, If you're familiar with NodeJS Express check the two main files of the server the [ExpressJS Routes](HomeControlServer/routes/index.js) file and the [ExpressJS App ](HomeControlServer/app.js) file. For security reasons, this server uses SSL encryption and asks for a password and a username everytime someone tries to access this server.
The second app is a [Control Daemon](ControlDaemon/NovaHomeDaemon.cpp) coded with C++, this one controls and handles the requests from NodeJS Web Server (IPC via ZMQ sockets) and other RF nodes (via NRF24L01), it also performs the Home Automation side of things.  
### Control Daemon Configuration  
The Nova Control Daemon is small process that controls the different RF nodes using the RF interface (NRF24L01), it waits for requests from the web server (when a user turn on/off/auto the lights from the browser). This is the heart of this system, it's the decision making engine of this Smart Home, I should note that temprature control uses a simple non-tested algorithme so this area might need some improvements. The autolights (based on motiond detection) and manual light control (using web server) work flawlessly. But this app needs a configuration, for now to configure the control daemon you have to check its [configuration file](/ControlDaemon/NovaHomeDaemon.cfg), I commented on every lines so it's pretty straightforward, later I will add the ability to configure the Dameon from a browser.
### Web Server How-To Install  
Just install NodeJS and copy the HomeControlServer. (The complete guide is coming...)
### Nova Home Daemon How-To Install  
Install the dependencies and compile NovaHomeDaemon. (Wait for the complete tutorial, it's gonna be easy)
### HomeRF Protocol 
I made this protocol to secure and manage the exchange of data and commands between the Raspberry Pi and the RF Nodes, this protocol sits on top of the RF24Network Library. It uses AES-128 for encryption and HMAC-SHA1 for authentication.
Here is a diagram for the HomeRF paquet and the structure of data encrypted inside of it :  
<p align="center">
<img src="https://cloud.githubusercontent.com/assets/12392615/8198164/b3a0e486-149a-11e5-9df7-21f7e487125e.png">  
</p>  
### Hardware Stack  
(Schematics explanations and design choices,...)
#### What's working :  
These functions are tested and working properly :  
1. Autolights and day saving.  
2. Geolocation (sets the time to enable/disable the autolights function).  
3. Manual lights control from any web browser computers, tablets or smarthphones (thanks to responsive design).  
4. Secure Encryprion of RF data using the AES-128 Standard.  
5. SMS Alert in case of fire or smoke detection.  

#### What's not Working :  
1. HMAC-SHA1 Authentication (gotta fix it).  
2. I haven't tested temprature control (the algorithme needs some improvment maybe a PID code).  
3. Call2Report (call the Smart Home and then it will report back with an SMS), this one is not tested.  

#### IMPORTANT NOTE !
You can't sell, publish any of the code in here, it's free software under the GPLv3 licence.
Read the licence for more information.
When you use this project or any code of it especially in an academic paper, you have to link to this website.
A. Benabdallah 2015.
