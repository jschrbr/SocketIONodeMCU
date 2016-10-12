#include "Energia.h"
#include <ESP8266AT.h>				//For ESP8266

// Length of static data buffers
#define DATA_BUFFER_LEN 120
#define SID_LEN 24

class SocketIOClient {

public:
	bool begin(int baud);
	bool start(String ssid, String pwd);
	bool connect(char hostname[], int port = 80);
	bool connected();
	void disconnect();
	bool on();
	void emit(String fun, String dat);
	void heartbeat(int select);

private:
	ESP8266AT client;
	void socker(int index);
	bool readHandshake();
	char *dataptr;
	char databuffer[DATA_BUFFER_LEN];
	char sid[SID_LEN];
	char key[28];
	char *hostname;
	int port;
	bool sent = true;
	bool emittin;
};
