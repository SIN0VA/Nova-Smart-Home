  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  ////**************************************** RF Lightswitch ********************************************************///////
  ////***************************************** By S!N0VA ************************************************************///////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  #include <RF24.h>
  #include <RF24Network.h>
  #include <HomeRF.h>
  #include <SPI.h>
  #include <math.h>
  #include <AES.h>
  #include <Entropy.h>
  #include <stdint.h>
  #include <sha1.h>
  #include <elapsedMillis.h>
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
    //////////////////pir_addr = 03 ,light_addr=02
  RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 
  RF24Network network(radio);          // Network uses that radio
  const uint16_t this_node = 02;        // Address of our node in Octal format
  const uint16_t hub_addr = 00;       // Address of the other node in Octal format
  
  ///////////////////////////////////////////////Working LIGHT SWITCH
    int relay = 9;
    elapsedMillis elapsedTime;
    bool autotimer=true;
  
    void setup() {
      pinMode(relay, OUTPUT);           
      SPI.begin();
      radio.begin();
      network.begin(/*channel*/ 90, /*node address*/ this_node);
      
    }
    unsigned     int duration;
    void loop() {    
    network.update();
    if(network.available()) {
      RF24NetworkHeader header;
      rf_paquet rfpaquet2 ;
      network.read(header, &rfpaquet2, sizeof(rfpaquet2));
      Sha1.initHmac(hmacKey1, 64);
      Sha1.print((const char*) &(rfpaquet2.aespaquet));
      if (true){
	byte dec_bytes2[16];
	byte iv2 [16];
        aes.set_IV((long int)rfpaquet2.aespaquet.i_v);
        aes.get_IV(iv2);
        aes.do_aes_decrypt(rfpaquet2.aespaquet.enc_data,16,dec_bytes2,key,bits,iv2);
       	light_c lightc;
	memcpy(&lightc, dec_bytes2, sizeof(light_c)); 
        duration =lightc.duration;
        PrintHex8(dec_bytes2,16);
        if      (strncmp (lightc.command,"ONN",2)==0) {digitalWrite(relay, HIGH);autotimer=false;}
        else if (strncmp (lightc.command,"OFF",2)==0) {digitalWrite(relay, LOW) ;autotimer=false;}
        else if (strncmp (lightc.command,"AUT",3)==0) {digitalWrite(relay, HIGH);autotimer=true;elapsedTime=0;}
        
      }
    }
  if (elapsedTime > duration  && autotimer==true) { digitalWrite(relay, LOW) ;autotimer=false;}  
 }

