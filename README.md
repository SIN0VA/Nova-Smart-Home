# Nova Smart Home
I am gonna put here the code of my final year project, which is a Smart Home based on a Raspberry Pi and a couple of Arduino UNO boards. With the use of NRF24L01 modules, I managed to create a small secure protocol called HomeRF to handle the exchange of commands and data between the RF nodes and the Rapsberry Pi. I had a hard time making encryption work but the time I spent on it really paid off, so I got AES-128 working which is pretty secure. Authentication based on HMAC-SHA-1 doesn't work, I will fix it this month.
I will write the whole documentation once I finish writing my thesis, everything works except the HMAC check (gotta fix it).
# Getting Started
Just to introduce the workings of this project a little bit, there are two programs ; a Node.JS app  which is an Express Web Server so people can turn off/on lights and things like that.If you're familiar with NodeJS Express check the two main files of the server the [ExpressJS Routes](HomeControlServer/routes/index.js) file and the [ExpressJS App ](HomeControlServer/app.js) file.
The second app is a [Control Daemon](ControlDaemon/NovaHomeDaemon.cpp) coded with C++, this one controls and handles the requests from NodeJS Web Server (IPC via ZMQ sockets) and other RF nodes (via NRF24L01), it also performs the Home Automation side of things.
# IMPORTANT NOTE ! :
You can't sell, publish any of the code in here, it's free software under the GPLv3 licence.
Read the licence before using it.
A. Benabdallah 2015.
