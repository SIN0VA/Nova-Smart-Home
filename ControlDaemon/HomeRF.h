//////******************************************************
//////***************** HomeRF libs - S!NOVA ******************
//////****************** Home Automation *******************
//////******************HomeRF for Rasp*******************
#ifndef HomeRF_H
#define HomeRF_H
#include "HomeRF.h"
#include <iostream>
#include <cstdio>
#include <stdint.h>
#include <string>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include "trng.h"
#include "sha1.h"
#include "AES.h"

void get_JSON(const char*, const char* );
void printHash(byte * ,int );
unsigned int find_lightaddr(unsigned int * ,unsigned int ,int );
byte * encrypt_command (char ,bool  ,bool ,unsigned int ,uint64_t& ,byte* ,byte * ,int ,AES&);
void send_rfcommand    (char ,bool  ,bool ,bool& ,unsigned int ,unsigned int,byte * ,int ,uint8_t *,RF24Network& );
bool checkoffTime(string ,string );
int convert2minutes(string );
//static rapidjson::Document get_subobject(rapidjson::Document&, const char *);


typedef struct sensor_ {             // S
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
	char paquet_type ;                 //'L' for Light Command ,'S' for Sensor Data ,'P' for PIR alert ,'T' for thermostat command
	char padding[3];
}aes_paquet;

typedef struct rf_paqu {
	aes_paquet aespaquet ;
	uint8_t hmac_tag[20] ;
}rf_paquet;
#endif
