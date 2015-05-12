////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////**************************************** Motion Detector *******************************************************///////
////***************************************** By S!N0VA ************************************************************///////

//////////////////pir_addr = 03 ,light_addr=02
  #include <RF24.h>
  #include <RF24Network.h>
  #include <HomeRF.h>
  #include <SPI.h>
  #include <math.h>
  #include <AES.h>
  #include <stdint.h>
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
  const uint16_t this_node = 03;        // Address of our node in Octal format
  const uint16_t hub_addr = 00;       // Address of the other node in Octal format
  
  /////////////////////////////////////////
  int pir_pin = 6;
 
  
  void setup() {    
   pinMode(pir_pin, INPUT);
   SPI.begin();    
   radio.begin();
   network.begin(/*channel*/ 90, /*node address*/ this_node);
   Serial.begin(57600);
   //Entropy.initialize(); 
  }
  
  void loop() {
  int pir=analogRead(A3);   
  delay(1000);
   Serial.print("Pir a3 = ");Serial.println(pir);
  if (/*digitalRead(pir_pin)==HIGH*/pir>1020) {
    
   
    
    network.update();
      //////////////////////////////////////////////////
  unsigned long int my_iv =TrueRandom.random();
  pir_data plain_data ={(unsigned int)03,(unsigned int)03} ;
  byte pir_bytes[sizeof(pir_data)];
  memcpy(pir_bytes, &plain_data, sizeof(pir_data));
  PrintHex8(pir_bytes,sizeof(pir_data));
  byte iv [16] ;
  byte cipher[16];
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  RF24NetworkHeader header( hub_addr);
  aes.do_aes_encrypt(pir_bytes,sizeof(pir_bytes),cipher,key,bits,iv);
  Sha1.initHmac(hmacKey1,64);
  aes_paquet aespaquet ;
  aespaquet.i_v=my_iv;
  aespaquet.paquet_type='P';
  memcpy(aespaquet.enc_data,cipher,sizeof(aespaquet.enc_data)); 
  Sha1.print((const char*)&aespaquet);
  uint8_t *hmactag = Sha1.resultHmac();
 rf_paquet rfpaquet ;
 rfpaquet.aespaquet = aespaquet;
 memcpy(&rfpaquet.hmac_tag,hmactag,sizeof(rfpaquet.hmac_tag));
 
 bool ok=network.write(header,&rfpaquet,sizeof(rfpaquet));    
 if (ok) Serial.println("ok.");
 else Serial.println("failed.");
 network.update();
      /////////////////////////////////////////////            
    }
  }

