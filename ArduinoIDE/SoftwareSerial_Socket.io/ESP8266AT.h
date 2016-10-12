#include "Arduino.h"
#include <SoftwareSerial.h>

SoftwareSerial serial(2,3);
class ESP8266AT {

public:
	bool setup(int baud);
	bool begin(String ssid, String pwd);
  bool connect(char addr[], int port);
  bool connected();
  bool stop();
  bool SEND(int len);
	String recvString(String target, uint32_t timeout = 1000);
	String recvString(String *target, uint32_t timeout = 1000);
	bool recvFind(String target, uint32_t timeout = 1000);
	bool recvFind(String* target, uint32_t timeout = 1000);
	void sendHandshake(char hostname[]);
	void readHandshake(char sid[], char hostname[]);
	void sendSocket(String data);
	bool sendSocket(String *data, int count);
	void sendSocket(char data);
	void sendSocket(int data);
  void rx_empty(void);
private:
	char *hostname;
	int port;

};
