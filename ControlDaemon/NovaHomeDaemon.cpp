//***************************************************************//
//********************Main Control Daemon************************//
//**********************S!NOVA 2015******************************//
//***************************************************************//
#include <iostream>
#include <string>
#include <fstream>
#include "zmq.hpp"
#include <zmq.h>
#include <zmq_utils.h>
#include <RF24/RF24.h>
#include <RF24Network/RF24Network.h>
#include <time.h>
#include <libconfig.h++>

#include <stdint.h>
#include "trng.h"
#include "AES.h"
#include "sha1.h"
#include <bitset>
#include <ctime>
#include <iomanip>
#include <curl/curl.h>
#include <cmath>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/rapidjson.h"
#include "zhelpers.hpp"
#include <rapidjson/filereadstream.h>
#include "HomeRF.h"
#include <stdio.h>
#include <stdlib.h>
#include <gnokii.h>
#include <signal.h>
#include <unistd.h>
#define _(x) x

/////////////////////////////////////////////////////
using namespace rapidjson;
using namespace std;
using namespace libconfig;
using namespace zmq;

/////////////////////////////////////////////////////GSM
static void businit(void);
static void busterminate(void) ;

///////////////////////////////////////// Encryption Setup

int bits = 128;
uint8_t hmacKey1[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14,
		0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
		0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
		0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f };
byte key[] = { 0x53, 0xDE, 0xF0, 0x33, 0x7F, 0x1E, 0x59, 0xB3, 0x0A, 0x38, 0xF4,
		0xC0, 0x11, 0x81, 0xFE, 0x9C };

//////////////////////////////////RF Setup
RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ); // Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
RF24Network network(radio);
struct gn_statemachine *state;
int quit = 0;
/////////////////////////////////////////////
typedef struct {
	char switchid;
	char state;
}node_message;
//////////////////////////////////////////////

int main(int argc, char** argv) {

	int call_id = 0;
	gn_call* pcall;
	bool thermostat = false ;
	bool heater=false;
	bool ac=false;
	bool auto_light=true;
	bool call2report=false;
	bool day_saving=false;
	bool geolocation=false;
	float maintained_temp=0;  //read from node.js Home Control Server ,when thermostat activated
	float out_temp ;
	float out_humid;
	float current_tem=0;
	float current_hum=0;
	float current_gas=0;
	//string city_name;
	string wan_ip;
	string country;
	string city;
	string lights_evening;
	string lights_off_ambient_off;
	////////////////////////////////////////////////////////////////////
	bool day_time=false;                //turn off all lights for late time, and activating ambient lights
	printf("Loading configuration file ''NovaHomeDaemon.cfg'' ... \n");
	Config cfg;
	try {
		cfg.readFile("NovaHomeDaemon.cfg");
	}  catch(const FileIOException &fioex) {
		std::cerr << "I/O error while reading file." << std::endl;
		return(EXIT_FAILURE);
	} catch(const ParseException &pex) {
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError() << std::endl;
		return(EXIT_FAILURE); }
	Setting &root = cfg.getRoot();
	unsigned int ctemp_l = cfg.lookup("ctemp_l");
	cout<<"Critical Lowest Temperature set to : " << ctemp_l << " °C "<< endl;
	unsigned int ctemp_h = cfg.lookup("ctemp_h");
	cout<<"Critical Highest Temperature set to : " << ctemp_h << " °C "<< endl;
	unsigned int temp_h = cfg.lookup("temp_h");
	cout<<"Hysteresis  Temperature set to : " << temp_h << " °C "<< endl;
	unsigned int cGas = cfg.lookup("cgas");
	cout<<"Critical Gas level set to : " << cGas <<" ppm "<< endl;
	unsigned int cHumid = cfg.lookup("chumid");           //Critical Humidty
	cout<<"Humidity Alert level set to : " << cHumid <<"% "<< endl;

	unsigned int light_timer = cfg.lookup("light_timer");           //Critical Humidty
	cout<<"Auto-lights timer set to : " << light_timer/1000.0 <<" seconds ."<< endl;

	unsigned int this_node= cfg.lookup("hub_addr");
	cout<<"RF Address of this RF Hub : "<< setfill('0')<< setw(2)<< this_node << endl;
	unsigned int thermostat_addr= cfg.lookup("thermostat_addr");
	cout<<"RF Address of the HVAC controller : " << setfill('0')<< setw(2)<< thermostat_addr << endl;
	unsigned int ambient_addr= cfg.lookup("ambient_addr");
	cout<<"Ambient Lights (at night) controller address : " << setfill('0')<< setw(2)<<ambient_addr << endl;
	const Setting &PSpairs = root["pirSwitchPairs"];
	int count = PSpairs.getLength();
	cout << "Number of PIR sensor/Light Switch pairs : " << count << " pairs ."
			<< endl;

	unsigned int pairsVector[count * 2]; //Array for pairs ,each pir_addr is followed by light_addr
	for (int i = 0; i < count; ++i) {
		const Setting &pair = PSpairs[i];
		unsigned int pir_addr;
		unsigned int light_addr;
		pair.lookupValue("pir_addr", pir_addr);
		pair.lookupValue("light_addr", light_addr);
		cout << "PIR Address : " <<pir_addr << " ; Light Switch Address : " << light_addr <<endl;
		if ((2*i)%2==0){
			pair.lookupValue("pir_addr", pairsVector[2*i]);
			pair.lookupValue("light_addr", pairsVector[2*i+1]);
		}
	}


	/* if(array)
	  {
	    for(i = 0; i < config_setting_length(array); i++)
	    {
	      long val = config_setting_get_int_elem(array, i);
	      printf("value [%i]: %ld\n", i, val);
	    }
	  }*/

	call2report = cfg.lookup("call2report");
	if (call2report)cout<<"Call2report enabled ." << endl; else
		cout<<"Call2report disabled  ." << endl;

	day_saving = cfg.lookup("day_saving");
	if (day_saving)cout<<"Lights day saving enabled ." << endl; else
		cout<<"Lights day saving disabled  ." << endl;

	Setting &alert_phones = root["alert_phones"];
	int number_of_phones = alert_phones.getLength();
	cout << "Number of phones in the emergency list , " << number_of_phones << " phones , ";
	std::string phonesVector[number_of_phones];
	for (int i = 0; i < number_of_phones; ++i) {
		string phone = alert_phones[i];
		phonesVector[i]=phone;
		/*phonesVector.lookupString();
	      phonesVector.lookupString();*/
		//phonesVector[i]=alert_phones.lookup(alert_phones[i]);
		cout<<", "<< phonesVector[i] ;
	}
	cout<<endl;
	if (number_of_phones<2) cout << "It is recommended to set at least three phone numbers ." << endl ;
	string lights_off_ambient_on = cfg.lookup("lights_off_ambient_on");
	cout<<"Turning off the main lights ,and switching to ambient lights (Late night) at : " << lights_off_ambient_on << endl;
	geolocation = cfg.lookup("geolocation");
	if (geolocation){
		cout<<"The function of geolocation is enabled." << endl;
		get_JSON("freegeoip.net/json","geo.json");
		cout<<"Auto-Geolocation setting is on ..."<<endl;
		FILE* pFile = fopen("geo.json", "rb");
		char buffer[65536];
		FileReadStream is(pFile, buffer, sizeof(buffer));
		Document document;
		document.ParseStream<0, UTF8<>, FileReadStream>(is);
		city=document["city"].GetString();
		country=document["country_name"].GetString() ;
		wan_ip=document["ip"].GetString() ;
		double longitude=document["longitude"].GetDouble() ;
		double latitude=document["latitude"].GetDouble() ;
		cout << "Country Detected : " << country << endl;
		cout<< "Region : " << document["region_name"].GetString()<<endl;
		cout << "City Detected : " << city << endl;
		cout << "Longitude : "<< longitude <<endl;
		cout << "Latitude : "<< latitude <<endl;
		cout << "External IP  : " << wan_ip << endl;
		string weather = "http://api.openweathermap.org/data/2.5/weather?lat="+to_string(latitude)+"&lon="+to_string(longitude)+"&units=metric";
		get_JSON(weather.c_str(),"weather.json");
		FILE* pFile2 = fopen("weather.json", "rb");
		char buffer2[65536];
		FileReadStream is2(pFile2, buffer2, sizeof(buffer2));
		Document document2;
		document2.ParseStream<0, UTF8<>, FileReadStream>(is2);
		time_t t = (time_t) document2["sys"]["sunrise"].GetInt();
		struct tm *tm = localtime(&t);
		char date2[10];
		strftime(date2, sizeof(date2), "%R", tm);
		lights_off_ambient_off = string(date2);
		cout<<"Turning off all lights (Morning) at  : " << lights_off_ambient_off << endl;
		t = (time_t) document2["sys"]["sunset"].GetInt();
		tm = localtime(&t);
		strftime(date2, sizeof(date2), "%R", tm);
		lights_evening = string(date2);
		cout<<"Turning on the main lights (Evening) at  : " << lights_evening << endl;
		out_temp = document2["main"]["temp"].GetDouble();
		cout << "Temperature outside the House : " << out_temp << "C°" << endl ;
		out_humid = document2["main"]["humidity"].GetDouble();
		cout << "Humidity outside the House : " << out_humid << "%" << endl ;
	} else {
		root.lookupValue("city_name", city);
		cout<<"The function of Geolocation is disabled" << endl;
		cout<<"Location is manually configured to : "<< city<<endl;
		root.lookupValue("lights_off_ambient_off", lights_off_ambient_off);
		//lights_off_ambient_off =(string) cfg.lookup("lights_off_ambient_off");
		cout<<"Turning off all lights (Morning) at  : " << lights_off_ambient_off << endl;
		//lights_evening = (string)cfg.lookup("lights_evening");
		root.lookupValue("lights_evening", lights_evening);
		cout<<"Turning on the main lights (Evening) at  : " << lights_evening << endl;
	}



	radio.begin();
	network.begin(/*channel*/90, /*node address*/this_node);
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	char *buffer;
	strftime (buffer,80,"Now it's %I:%M%p.",timeinfo);
	printf ("Current local time and date: %s \n", asctime(timeinfo));
	usleep(10);
	//radio.printDetails();
	cout<<"Listening for RF packets ..."<<endl;
	double last_pir_t;
	double last_;
	zmq::context_t context (1);
	zmq::context_t context2 (2);
	socket_t socket (context, ZMQ_PULL);
	socket.connect("tcp://127.0.0.101:5555");

	zmq::socket_t responder(context, ZMQ_REP);
	responder.bind("tcp://127.0.0.104:5559");

	int i=0;
	while (1) {
		////////////////////////////////////Checking the time for day and night lights control/////////////////////////////////
		if(day_saving==true) {
		day_time=checkoffTime(lights_off_ambient_on,lights_evening);
		usleep(10);}
		/////////////////////////////////Sending Temprature to Web Server////////////////////////////////////////
		message_t message1;
		int error1=responder.recv(&message1,ZMQ_NOBLOCK);
		if(error1!=0&&error1!=-1) {
		cout<<"Received temperature request from server ...";
        usleep (2);
        string json="{\"temp\":"+ to_string(round(current_tem)) +",\"humid\":"+ to_string(round(current_hum)) +"}";
        message_t reply(json.length());
        memcpy ((void *) reply.data (), json.c_str(), json.length());
        responder.send (reply);
        cout<<"reply sent ."<<endl;
		}
		////////////////////////////////////////ZMQ Listener ,Looking for Home Control Server Events /////////////////////////////////
		usleep(200);
		network.update();
		message_t message;
		int error=socket.recv(&message,ZMQ_NOBLOCK);
		if(error!=0&&error!=-1) {
			string mess = string(static_cast<char*>(message.data()), message.size());
			cout << mess << endl;
			Document document;
			document.Parse(mess.c_str());
			string switchid=document["switchid"].GetString() ;
			string state=document["state"].GetString();
			if (state.compare(1,3,"uto")==0)	{
				bool send=true;
				unsigned int light_addr = pairsVector[2*(stoi(switchid))+1];
				send_rfcommand('L', heater, ac, send,light_timer,light_addr,key,bits,hmacKey1,network);
				cout << "Command from Home Control Web Server: Auto lights are activated ."<<endl;
				auto_light=true;
			} else if (state.compare(1,1,"n")==0){
				bool send=true;
				unsigned int light_addr = pairsVector[2*(stoi(switchid))+1];
				cout << "Command from Home Control Web Server: "<< light_addr<<" Lights are on ." <<endl;
				send_rfcommand('N', heater, ac, send,5000,light_addr,key,bits,hmacKey1,network);
				auto_light=false;
			} else if (state.compare(1,2,"ff")==0){
				bool send=true;
				unsigned int light_addr = pairsVector[2*(stoi(switchid))+1];
				cout << "Command from Home Control Web Server: "<< light_addr<<" Lights are off ."<<endl;
				send_rfcommand('F', heater, ac, send,5000,light_addr,key,bits,hmacKey1,network);
				auto_light=false;
			}
		}
		//////////////////////////////////////////////////////////////////
		bool send_t=false;
		rf_paquet rfpaquet2 ;

		/////////////////////////////////////////RF Paquets Control
		if (network.available()) {
			RF24NetworkHeader header;
			network.read(header, &rfpaquet2, sizeof(rfpaquet2));
			//Sha1.initHmac(hmacKey1, 64);
			//Sha1.print((const char*) &rfpaquet2.aespaquet);
			//aes_paquet aespaquet;
			//memcpy(&aespaquet,&rfpaquet2.aespaquet,24);
			cout << "Received packet ... ************************************************************************" << endl;
			cout<<"Recieved Aespaquet in hex : "<<endl ;
			printHash((byte*)&rfpaquet2.aespaquet,24);
			cout<<"Recieved packet in hex : "<<endl ;
			printHash((byte*)&rfpaquet2,44);
			Sha1.initHmac(hmacKey1,64);
			Sha1.print((const char*)&(rfpaquet2.aespaquet));
			uint8_t * hmac_tag=Sha1.resultHmac();
			//memcpy(hmac_tag,Sha1.resultHmac(),20);
			cout<<"Calc HMAC in hex : "<<endl ;
			printHash((byte*)hmac_tag,20);
			cout<<"Pack HMAC in hex : "<<endl ;
			printHash((byte*)rfpaquet2.hmac_tag,20);
			//if (strncmp ((const char *)hmac_tag,(const char *)rfpaquet2.hmac_tag,20) == 0) { // Test HMAC
			if (true) { // Bypassing HMAC verification, will test only encryption
				long int i_v = rfpaquet2.aespaquet.i_v;
				cout <<"IV : ";
				cout<<i_v<<endl;
				cout<<"HMAC Success !"<<endl;
				byte dec_bytes2[16];
				byte iv2 [16];
				AES aes;
			    aes.set_IV((long int)rfpaquet2.aespaquet.i_v);
				aes.get_IV(iv2);
				//memcpy(iv2,&(rfpaquet2.aespaquet.i_v),sizeof(rfpaquet2.aespaquet.i_v));
				aes.do_aes_decrypt(rfpaquet2.aespaquet.enc_data,16,dec_bytes2,key,bits,iv2);          //if it didn't work change aes.get_size() to 15
				cout<<"********************************";
				if (rfpaquet2.aespaquet.paquet_type=='S') cout <<"Sensor Packet Identified  *********************************"<<endl;
				else if (rfpaquet2.aespaquet.paquet_type=='P') cout<<"PIR Packet Identified *********************************"<<endl;
				cout <<"HMAC tag : ";
				printHash((byte*)rfpaquet2.hmac_tag,20);
				cout <<"The RF Packet in hex : " ;
				printHash((byte*)&rfpaquet2,44);
				cout <<"Encrypted  bytes : ";
				printHash(rfpaquet2.aespaquet.enc_data,sizeof(rfpaquet2.aespaquet.enc_data));
				if (rfpaquet2.aespaquet.paquet_type=='S') {
					sensor_data dec_data;
					memcpy(&dec_data, dec_bytes2, sizeof(sensor_data));    //If messed up output change sizeof(sensor_data) to sizeof(dec_bytes2)
					cout <<"Decrypted bytes : ";
					printHash((byte*)&dec_data,sizeof(dec_data));
					current_tem=dec_data.tem;
					cout<<"Temperature : "<<to_string(dec_data.tem)<<"C°" <<endl;
					current_hum=dec_data.hum;
					cout<<"Humidity  : "<<to_string(dec_data.hum)<<"%" <<endl;
					current_gas=dec_data.gas;
					cout<<"Gas levels : "<<to_string(dec_data.gas)<<"ppm" <<endl;
					if (dec_data.gas>cGas) {//SMS Notification
						cout << "WARNING !! Possible smoke or gas leak in the house !" <<endl;
						for (int i=0;i<number_of_phones;i++){
							string sms_alert="echo \"Warning ! Possible Fire or Gas Leak . Attention ! Possibilite d'incendie ou fuite de gas ! - Test SmartHome  A.B. PFE 2015 ! Je vous remercie  ! "
									" \" | gnokii --sendsms "+phonesVector[i]+" &";
							system(sms_alert.c_str());
							cout << "An Alert SMS has been sent to " << phonesVector[i] <<endl;
							//sleep(2);
						}

					}

					/////////////////////////////////////////////////////Critical Temperatures Control
					if (thermostat==false) {
						if (dec_data.tem<ctemp_l && heater==false) {
							cout<<"Heater Activated ."<<endl;           //send the message Heat and Fan enabled
							heater=true;
							send_t=true;
						}
						else if(dec_data.tem>ctemp_l+temp_h&&heater==true){
							cout<<"Heater shutting down ."<<endl; //send the message Heat and Fan shut down
							heater=false;
							send_t=true;
						}
						if (dec_data.tem>ctemp_h&&ac==false) {
							cout<<"Air Conditioner  Activated ."<<endl; //send the message A/C and Fan enabled
							ac=true;
							send_t=true;
						}
						else if(dec_data.tem<ctemp_h-temp_h&&ac==true){
							cout<<"Air Conditioner shutting down ."<<endl; //send the message A/C and Fan shutdown
							ac=false;
							send_t=true;
						}  /////////////////////////Thermostat Control //////////////
					} else {
						if (dec_data.tem < maintained_temp-temp_h && heater == false) {
							cout << "Heater Activated ." << endl;
							heater = true;
							send_t=true;
						} else if (dec_data.tem > maintained_temp + temp_h && heater == true) {
							cout << "Heater shutdown ." << endl;
							heater = false;
							send_t=true;
						} else if (dec_data.tem > maintained_temp+temp_h+1 && ac == false) {
							cout << "Air Conditioner  Activated ." << endl;
							ac = true;
							send_t=true;
						} else if (dec_data.tem < maintained_temp-temp_h+1 && ac == true) {
							cout << "Air Conditioner  shutdown ." << endl;
							ac = false;
							send_t=true;
						}
					}
					send_rfcommand('T', heater, ac, send_t,5000,thermostat_addr,key,bits,hmacKey1,network);
					usleep(10);
				} else if (rfpaquet2.aespaquet.paquet_type == 'P' && auto_light==true && day_time==false) {
					bool send=true;
					unsigned int addrr;
					memcpy(&addrr,dec_bytes2,2);
					//memcpy(&dec_data, dec_bytes2, sizeof(pir_data));
					cout << "PIR trip from node "<< addrr<<" received !" << endl;
					unsigned int light_addr = find_lightaddr(pairsVector,addrr,count);
					send_rfcommand('L', heater, ac, send,light_timer,light_addr,key,bits,hmacKey1,network);
					usleep(10);
				} else cout << "Lights are set to manual control from web server ,or it's day time ! No light control packets are sent ." <<endl;

			}

			else cout<<"Wrong HMAC . Packet Dropped  ! " << endl;
		}
		usleep(1);
		//////////////////////////////////////////////////////////GSM Call Listener /////////////////
		//fprintf(stderr, _("Waiting for a call.\n"));

		if (call2report) {
			businit();
			gn_call_check_active(state);
			pcall = gn_call_get_active(call_id);
			if (NULL != pcall) {
				printf("%d\n", (int) pcall->status);
				if (1==pcall->status /*&& 0 == strcmp(pcall->remote_number, number1)*/) {
					gn_call_cancel (call_id);
					usleep(10);
					cout<<"Call Received from Number: "<<pcall->remote_number <<endl;
					gn_call_cancel(call_id);
					usleep(10);
					cout<<"Sending SMS ..."<<endl;
					string report_sms="echo \"Temperature=" +to_string(current_tem) + " Humidity=" +to_string(current_hum)+" Gas_Levels="+to_string(current_gas)+ " Thermostat=" +(thermostat ? "ON" : "OFF")+ " Air Conditioner=" +
							+(ac ? "ON" : "OFF")+ " .Call2Report Service By Adil Benabdallah . \" | gnokii --sendsms " +string(pcall->remote_number);
					system(report_sms.c_str());
				}
			}
			busterminate();
			usleep(10);
		}
		usleep(10);
	}
	return 0;
}

static void businit(void) {
	if (GN_ERR_NONE != gn_lib_phoneprofile_load(NULL, &state))
		exit(2);
	if (GN_ERR_NONE != gn_lib_phone_open(state))
		exit(2);
}
static void busterminate(void) {
	gn_lib_phone_close(state);
	gn_lib_phoneprofile_free(&state);
	gn_lib_library_free();
}
/*void signal_handler(int signal) {
	quit = 1;
}*/
