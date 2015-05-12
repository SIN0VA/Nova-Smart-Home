////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////**************************************** RF Sensor Node ********************************************************///////
////***************************************** By S!N0VA ************************************************************///////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <math.h>
#include <AES.h>
#include "./printf.h"
#include <stdint.h>
#include <HomeRF.h>
#include <sha1.h>
#include <TrueRandom.h>

/////////////////////////////////////Encryption setup
AES aes;
int bits = 128;
uint8_t hmacKey1[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
		0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
		0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f };
byte key[] = { 0x53, 0xDE, 0xF0, 0x33, 0x7F, 0x1E, 0x59, 0xB3, 0x0A, 0x38, 0xF4,
		0xC0, 0x11, 0x81, 0xFE, 0x9C };



////////////////////////////////////////////RF Setup 
RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 
RF24Network network(radio);          // Network uses that radio
const uint16_t this_node = 01;        // Address of our node in Octal format
const uint16_t hub_addr = 00;       // Address of the other node in Octal format

void setup(void)
{  
  pinMode(HOUT_PIN, OUTPUT);
  pinMode(HIN_PIN, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(0,OUTPUT);
  digitalWrite(0,HIGH);  
  SPI.begin();    
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
   Serial.begin(57600);
   //Entropy.initialize(); 
  //Serial.println("Sensor Node Started ...");
}
int long timer=millis();
float  temp_c() {
  int voltemp= analogRead(TEMP_PIN)-80;
  float millivolts= (voltemp/1023.0) * 5000;
  return  millivolts/10 - 273.15  ;
}

void loop() {

  
  //  Serial.println(millis()-timer);
  if (millis()-timer>60000)  { // Delay of one minute so the MQ2 heats up and give accurate readings .
      Serial.println("**********************************************************");
    float gas=0; for(int i=0;i<10;i++) {
      gas = gas + analogRead(MQ2_PIN);
    }
    gas=gas/10;
    float cap=0;
    for (int i=0;i<20;i++) {
      cap=cap+capacitance(); 
    }  
    cap=cap/20.0;
    float temp=0;
    for (int i=0;i<20;i++) {
      temp=temp+temp_c(); 
    }
  if (gas>600.0) digitalWrite(3,HIGH);else digitalWrite(3,LOW);
  temp=temp/20.0;
  Serial.print("Sending... ");
  unsigned long int my_iv_ =TrueRandom.random();
  Serial.print("Random IV :"); Serial.println(my_iv_);
  Serial.print("Temperature : ");
  Serial.print(temp);Serial.println(" °C");
  Serial.print("Gas Density : ");
  Serial.print(gas);Serial.println("ppm");
  Serial.print("Capacitance : ");
  Serial.print(cap); Serial.println("uF");
  float hum= map (cap,170,195,20,90) ;
  sensor_data plain_data ={temp,hum,gas} ;
  Serial.print("Sensor Data : ");
  PrintHex8((uint8_t*)&plain_data,sizeof(plain_data));
  byte sensor_bytes[sizeof(sensor_data)];
  memcpy(sensor_bytes, &plain_data, sizeof(sensor_data));
  byte iv [16] ;
  byte cipher[16];
  aes.set_IV(my_iv_);
  aes.get_IV(iv);  
  aes.do_aes_encrypt(sensor_bytes,sizeof(sensor_bytes),cipher,key,bits,iv);  
  
  aes_paquet aespaquet ;
  aespaquet.i_v=my_iv_;
  aespaquet.padding[0]=254;
  aespaquet.padding[1]=254;
  aespaquet.padding[3]=254;
  aespaquet.paquet_type='S';
  memcpy(aespaquet.enc_data,cipher,sizeof(aespaquet.enc_data));
  rf_paquet rfpaquet ;
  rfpaquet.aespaquet = aespaquet;
  Sha1.initHmac(hmacKey1,64);
  Sha1.print((const char*)&rfpaquet.aespaquet);
  memcpy(rfpaquet.hmac_tag,Sha1.resultHmac(),sizeof(rfpaquet.hmac_tag));
  Serial.print("HMAC tag : ");
  PrintHex8(rfpaquet.hmac_tag,20);
  Serial.print("AES paquet in hex : ");
  PrintHex8((uint8_t*)&aespaquet,sizeof(aespaquet));
  Serial.print("All paquet in hex : ");
  PrintHex8((uint8_t*)&rfpaquet,sizeof(rfpaquet));
  
  network.update();
  RF24NetworkHeader header( hub_addr);
  bool ok=network.write(header,&rfpaquet,sizeof(rfpaquet));   // Sending the Sensor Node RF packet
  delay(2000);                                                // Send every 2 seconds
   }
}



