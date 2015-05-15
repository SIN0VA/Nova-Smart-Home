//////******************************************************
//////****************** HomeRF - S!NOVA ******************
//////****************** Home Automation *******************
//////******************************************************
#include "HomeRF.h"
#include <stdio.h>
#include <iostream>
#include <stdint.h>


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




