//////******************************************************
//////****************** HomeRF - S!NOVA ******************
//////****************** Home Automation *******************
//////******************************************************
#include <stdint.h>
#include "Arduino.h"
#ifndef HOMERF_H
#define HOMERF_H

///////***************************Config. of Sensors,no RF***********************************
const int MQ2_PIN =A5 ;   //Smoke Detector MQ2
const int TEMP_PIN = A4;  //TempLM335
const int HOUT_PIN = A2; //HumidA2
const int HIN_PIN = A0;  //HumidA1
const float IN_STRAY_CAP_TO_GND = 24.48; //initially this was 30.00
const float IN_EXTRA_CAP_TO_GND = 0.0;
const float IN_CAP_TO_GND  = IN_STRAY_CAP_TO_GND + IN_EXTRA_CAP_TO_GND;
const int MAX_ADC_VALUE = 1023;
///////***************************************************************************************
//float  temp_c();
float capacitance ();
void printHash(byte * ,int );
void PrintHex8(uint8_t *data, uint8_t length);

//0549907542
typedef struct sensor_ {             // S        ///////If it didn't work ,try setRetries ,MaxPayload Check ,timeout check ,or change to float
	float tem;
	float hum;
	float gas;
	float pad;
}sensor_data;

typedef struct pir_ {                 // P
	unsigned int pir_addr;
	unsigned int pir_addr0;
}pir_data;

typedef struct thermo_ {              // T
	bool heater;
	bool ac;
}thermostat_c;

typedef struct light_ {              // L
	char command[4];
	unsigned int duration;
}light_c;

typedef struct aes_paqu {
	long int  i_v ;
	byte   enc_data[16];
	char paquet_type ;  
	char padding[3];
}aes_paquet;

typedef struct rf_paqu {
	aes_paquet aespaquet ;
	uint8_t hmac_tag[20] ;    //replace uint8_t with byte
}rf_paquet;
#endif