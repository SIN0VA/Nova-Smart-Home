//////******************************************************
//////*****************  HomeRF - S!NOVA ******************
//////****************** Home Automation *******************
//////****************** HomeRF for Rasp*******************
#include "HomeRF.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdint.h>
#include <curl/curl.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"
#include "trng.h"
#include "sha1.h"
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <gnokii.h>
#include <sstream>
#include <iomanip>
using namespace std;

void printHash(byte * hash,int j) {            //uint8_t
	int i;
	for (i=0; i<j; i++) {
		printf("%x",hash[i]>>4);
		printf("%x",hash[i]&0xf);
		// printf("%x",hash[i]);
	}
	printf("\n");
}

void get_JSON(const char* url, const char* file_name)
{
	CURL* easyhandle = curl_easy_init();
	curl_easy_setopt( easyhandle, CURLOPT_URL, url ) ;
	curl_easy_setopt(easyhandle, CURLOPT_FOLLOWLOCATION, 1L);
	FILE* file = fopen( file_name, "w");
	curl_easy_setopt( easyhandle, CURLOPT_WRITEDATA, file) ;
	curl_easy_perform( easyhandle );
	curl_easy_cleanup( easyhandle );
	fclose(file);
}

unsigned int find_lightaddr(unsigned int * pairsvector,unsigned int pir_addr,int pairs_count){
	for(int i=0;i<2*pairs_count;i++){
		if (pairsvector[i]==pir_addr) return pairsvector[i+1];
	}
	return 0;
}

byte * encrypt_command(char &type ,bool heater ,bool ac,unsigned int duration,long int & my_iv,byte* iv,byte * key,int bits,AES& aes){
	if (type=='T'){
		thermostat_c thermo_c={heater,ac};
		byte thermo_bytes[sizeof(thermo_c)];
		memcpy(thermo_bytes, &thermo_c, sizeof(thermo_c));
		byte * cipher=new byte [16];
		aes.set_IV(my_iv);
		aes.get_IV(iv);
		aes.do_aes_encrypt(thermo_bytes,sizeof(thermo_bytes),cipher,key,bits,iv);
		cout<<"Encrypt command run successfully ..."<<endl;
		return cipher;
	} else {
		light_c lightc;
		lightc.duration=duration;
		if (type=='L') strcpy((char*)lightc.command, "AUT");                     //AUTO
		else if (type=='N') strcpy((char*)lightc.command, "ONN");
		else if (type=='F') strcpy((char*)lightc.command, "OFF");
		byte *cipher=new byte[16];
		aes.set_IV(my_iv);
		aes.get_IV(iv);
		aes.do_aes_encrypt((byte*)&lightc,sizeof(lightc),cipher,key,bits,iv);
		cout<<"Encrypt command run successfully ..."<<endl;
		type='L';
		return cipher;
	}
	return NULL;
}


void send_rfcommand(char type,bool heater,bool ac,bool& send,unsigned int duration,unsigned int addr,byte * key,int bits,uint8_t *hmacKey,RF24Network& network){
	if(send==true) {
		trng<long int> rng;
		long int my_iv =rng.random();
		byte iv [N_BLOCK] ;
		AES aes;
		byte *cipher=encrypt_command( type , heater , ac,duration, my_iv, iv, key, bits, aes);
		Sha1.initHmac(hmacKey,64);
		//aes_paquet aespaquet = {my_iv,cipher,type };
		aes_paquet aespaquet ;
		aespaquet.i_v=my_iv;
		aespaquet.paquet_type=type;
		memcpy(aespaquet.enc_data,cipher,sizeof(aespaquet.enc_data));
		Sha1.print((const char*)&aespaquet);
		uint8_t *hmact = Sha1.resultHmac();
		rf_paquet rfpaquet ;
		rfpaquet.aespaquet = aespaquet;
		memcpy(&rfpaquet.hmac_tag,hmact,sizeof(rfpaquet.hmac_tag));
		RF24NetworkHeader header(addr);
		/*cout << "My_iv in hex : "<<endl;
		printHash((byte*)&my_iv,4);
		cout << "HMAC in hex : "<<endl;
		printHash((byte*)hmact,8);
		cout << "Cipher in hex : "<<endl;*/
		//printHash((byte*)cipher,sizeof(aespaquet.enc_data));
		if (type=='T') cout << "Sending Thermostat Control Packet ...";
		if (type=='L') cout << "Sending Light Control Packet ...";
		//cout << "RF Packet in hex : "<<endl;
		//printHash((byte*)&rfpaquet,28);
		bool ok=network.write(header,&rfpaquet,sizeof(rfpaquet));
		network.update();
		//cout << "Size of rf paquet :"<<sizeof(rfpaquet)<<endl;
		if (ok) cout <<"ok."<<endl;
		else cout<<"failed."<<endl;
		send=false;
		delete[] cipher ;
	}
}

int convert2minutes(string hours_minutes){
	const char *time_details = hours_minutes.c_str();
	struct tm tm;
	strptime(time_details, "%H:%M", &tm);
	return (tm.tm_hour *60+tm.tm_min);
}
bool checkoffTime(string lightsoff_time,string lightson_time){
		int lofft=convert2minutes(lightsoff_time);
		int lont=convert2minutes(lightson_time);
	    time_t     now = time(0);
	    struct tm  nowtm;
	    char       buf[10];
	    nowtm = *localtime(&now);
	    int nowmin=nowtm.tm_hour*60+nowtm.tm_min;
	    if (lofft >= nowmin && nowmin >= lont) return false ;
	    else return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////
/*void busterminate(void) {
	gn_lib_phone_close(state);
	gn_lib_phoneprofile_free(&state);
	gn_lib_library_free();
}

void businit(void) {
	gn_error	error;

	atexit(busterminate);

	error = gn_lib_phoneprofile_load(NULL, &state);
	if (GN_ERR_NONE == error) {
		error = gn_lib_phone_open(state);
	}

	if (GN_ERR_NONE != error) {
		fprintf(stderr, "%s\n", gn_error_print(error));
		exit(2);
	}
}

void signal_handler(int signal) {
	(void)signal;
	quit = 1;
}

void incoming_call(int lowlevel_id, char *number) {
	// sometimes the callback function gets called twice for the same call
	if (call_detected == lowlevel_id) return;

	printf("%s\n", number);
	call_detected = lowlevel_id;
}

gn_error hangup(int lowlevel_id) {
	gn_call_info	callinfo;
	gn_data		data;

	memset(&callinfo, 0, sizeof(callinfo));
	callinfo.call_id = lowlevel_id;

	gn_data_clear(&data);
	data.call_info = &callinfo;

	return gn_sm_functions(GN_OP_CancelCall, &data, state);
}

gn_error answer(int lowlevel_id) {
	gn_call_info	callinfo;
	gn_data		data;

	memset(&callinfo, 0, sizeof(callinfo));
	callinfo.call_id = lowlevel_id;

	gn_data_clear(&data);
	data.call_info = &callinfo;

	return gn_sm_functions(GN_OP_AnswerCall, &data, state);
}

void notify_call(gn_call_status call_status, gn_call_info *call_info, struct gn_statemachine *state, void *callback_data) {
	//in this callback function you can't use those libgnokii functions that send a packet
	(void)state;
	(void)callback_data;

	if (call_status != GN_CALL_Incoming) return;

	incoming_call(call_info->call_id, call_info->number);

	return;
}*/











/*cout <<"IV : "<<my_iv3<<endl;
byte iv3 [N_BLOCK] ;
AES aes3;
byte*enc=encrypt_command ('L',true,true ,my_iv3,iv3 ,key,128,aes3);
printHash(iv3,sizeof(iv3)) ;
byte dec_bytes3 [sizeof(enc) + (N_BLOCK - (sizeof(enc) % 16))-1];
aes.set_IV(my_iv3);
aes.get_IV(iv3);
aes3.do_aes_decrypt(enc,aes3.get_size(),dec_bytes3,key,bits,iv3);
printHash(iv3,sizeof(iv3)) ;
cout<<"Sensor packet received !"<<endl;
char  dec_data2[4];
memcpy(dec_data2, dec_bytes3, sizeof(dec_bytes3));
string str=string(dec_data2);
cout<<"Decrypted Data : "<< str<<endl;
printHash(dec_bytes3,sizeof(dec_bytes3)) ;
delete []enc;*/




/////////////////////////In case things are messed up this code works .
/*trng<uint64_t> rng;
uint64_t my_iv3 =rng.random();
cout <<"IV : "<<my_iv3<<endl;
byte iv3 [N_BLOCK] ;
AES aes3;
byte*enc=encrypt_command ('L',true,true ,my_iv3,iv3 ,key,128,aes3);
printHash(iv3,sizeof(iv3)) ;
byte dec_bytes3[sizeof(enc) + (N_BLOCK - (sizeof(enc) % 16))-1];
aes.set_IV(my_iv3);
aes.get_IV(iv3);
aes3.do_aes_decrypt(enc,aes3.get_size(),dec_bytes3,key,bits,iv3);
printHash(iv3,sizeof(iv3)) ;
cout<<"Sensor packet received !"<<endl;
char  dec_data2[4];
memcpy(dec_data2, dec_bytes3, sizeof(dec_bytes3));
string str=string(dec_data2);
cout<<"Decrypted Data : "<< str<<endl;
printHash(dec_bytes3,sizeof(dec_bytes3)) ;
delete []enc;*/



/*void rf_decipher(AES aes,rf_paquet rfpaquet2,RF24NetworkHeader header,int bits,byte * key){
		byte dec_bytes2[sizeof(sensor_data) + (N_BLOCK - (sizeof(sensor_data) % 16))-1];
		byte iv2 [N_BLOCK];
		aes.set_IV(rfpaquet2.aespaquet.i_v);
		aes.get_IV(iv2);
		aes.do_aes_decrypt(rfpaquet2.aespaquet.enc_data,aes.get_size(),dec_bytes2,key,bits,iv2);
	if (rfpaquet2.aespaquet.paquet_type=='S') {
		cout<<"Sensor packet received !"<<endl;
		sensor_data dec_data2;
		memcpy(&dec_data2, dec_bytes2, sizeof(sensor_data));
		cout<<"Temperature : "<<dec_data2.tem<<"C°" <<endl;
		cout<<"Humidity  : "<<dec_data2.hum<<"%" <<endl;
		cout<<"Gas levels : "<<dec_data2.gas<<"ppm" <<endl;
		if (dec_data2.gas>cGas)  cout << "WARNING !! Smoke or Gas leak alert !" <<endl;//SMS Notification
		if (dec_data2.tem<ctemp_l) {}
		else (dec_data2.tem<ctemp_l)
	}  else if (rfpaquet2.aespaquet.paquet_type=='P'){
		aes.do_aes_decrypt(rfpaquet2.aespaquet.enc_data,aes.get_size(),dec_bytes2,key,bits,iv2);
		cout<<"PIR trip from node packet received !"<<endl;
		char  dec_data2[10];
		memcpy(&dec_data2, dec_bytes2, sizeof(sensor_data));
	}
}*/
/*rapidjson::Document get_subobject(rapidjson::Document& base, const char *subobject_name){
	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer( sb );
	base[subobject_name].Accept( writer );
	rapidjson::Document subobject;
	subobject.Parse<0>(sb.GetString());
	return subobject ;
}*/
