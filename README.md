# Nova Smart Home
I am gonna put here the code of my final year project, which is a Smart Home based on a Raspberry Pi and a couple of Arduino UNO cards. With the use of NRF24L01 modules, I managed to create a small secure protocol called HomeRF to handle the exhange of commands and data between the RF nodes and the Rapsberry Pi. I got a hard time making encryption work but the work really paid off, so I got AES-128 working which is pretty secure. Authentication based on HMAC-SHA-1 doesn't work, I will it fix this month if I have the time.
I will write the whole documentation once I finish writing my thesis, everything works except the HMAC check (gotta fix it).
# Getting Started
Just to introduce the workings of this project a little bit, there are two programs ; a Node.JS app  which is an Express Web Server so people can turn off/on lights and things like that. And a Control Process coded with C++, this one controls and handle the requests from the NodeJS WebServer and other RF nodes (Arduino UNO's), by receiving and sending HomeRF packets (via NRF24L01), it also performs the Home Automation side of things.
I am gonna upload the Arduino Shields that I designed at a later time.
