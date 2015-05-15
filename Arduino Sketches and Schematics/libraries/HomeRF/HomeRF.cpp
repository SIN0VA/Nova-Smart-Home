//////******************************************************
//////****************** HomeRF - S!NOVA ******************
//////****************** Home Automation *******************
//////******************************************************
#include "HomeRF.h"
#include <stdio.h>
#include <iostream>
#include <stdint.h>

float  temp_c() {
  int voltemp= analogRead(TEMP_PIN)-80;
  float millivolts= (voltemp/1023.0) * 5000;
  return  millivolts/10 - 273.15  ;
}

float capacitance (){  
  pinMode(HIN_PIN, INPUT);
  digitalWrite(HOUT_PIN, HIGH);
  int val = analogRead(HIN_PIN);  
  digitalWrite(HOUT_PIN, LOW);
  pinMode(HIN_PIN, OUTPUT);
  return ((float)val * IN_CAP_TO_GND / (float)(MAX_ADC_VALUE - val));
}

void printHash(unsigned char * hash,int j) {            //uint8_t
  int i;
  for (i=0; i<j; i++) {
    printf("%x",hash[i]>>4);
    printf("%x",hash[i]&0xf);
	 // printf("%x",hash[i]);
  }
  printf("\n");
}


void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex
{
     char tmp[length*2+1];
     byte first;
     byte second;
     for (int i=0; i<length; i++) {
           first = (data[i] >> 4) & 0x0f;
           second = data[i] & 0x0f;
           // base for converting single digit numbers to ASCII is 48
           // base for 10-16 to become lower-case characters a-f is 87
           // note: difference is 39
           tmp[i*2] = first+48;
           tmp[i*2+1] = second+48;
           if (first > 9) tmp[i*2] += 39;
           if (second > 9) tmp[i*2+1] += 39;
     }
     tmp[length*2] = 0;
     Serial.println(tmp);
}

