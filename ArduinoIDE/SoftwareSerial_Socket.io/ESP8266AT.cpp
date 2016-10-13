#include <ESP8266AT.h>

void ESP8266AT::sendSocket(String data) {
  serial.print(data);
}

bool ESP8266AT::sendSocket(String *data, int count) {
  rx_empty();
  // String tar[] = {"STATUS:3","OK", "no"};
  // if (recvFind(tar, 10)){return;}
  int i = 0;
  while(i<count){
  serial.print(data[i]);
  Serial.print(data[i]);
  ++i;
  }
  return recvFind("SEND OK", 5000);
}

void ESP8266AT::sendSocket(char data) {
  serial.print(data);
}

void ESP8266AT::sendSocket(int data) {
  serial.print(data);
}

void ESP8266AT::rx_empty(void){
    while(serial.available() > 0) {
        serial.read();
    }
}

void ESP8266AT::sendHandshake(char hostname[]) {
	serial.println(F("GET /socket.io/1/?transport=polling&b64=true HTTP/1.1"));
	serial.print(F("Host: "));
	serial.println(hostname);
	serial.println(F("Origin: Arduino"));
	serial.println("");
	Serial.println("Handshake Sent");
  return;
}

void ESP8266AT::readHandshake(char sid[], char hostname[]) {
  serial.print(F("GET /socket.io/1/websocket/?transport=websocket&b64=true&sid="));
  serial.print(sid);
  serial.println(F(" HTTP/1.1"));
  serial.print(F("Host: "));
  serial.println(hostname);
  serial.println(F("Origin: ArduinoSocketIOClient"));
  serial.print(F("Sec-WebSocket-Key: "));
  serial.println(sid);
  serial.println(F("Sec-WebSocket-Version: 13"));
  serial.println(F("Upgrade: websocket"));	// must be camelcase ?!
  serial.println(F("Connection: Upgrade"));
  serial.println("");
  return;
}

bool ESP8266AT::connected(){
  String data;
  rx_empty();
  serial.println("AT+CIPSTATUS");
  String tar[] = {"STATUS:4", "ERROR", "no"};
  return !recvFind(tar,60);
}

bool ESP8266AT::stop(){
  rx_empty();
  serial.println("AT+CIPCLOSE");
  return recvFind("OK", 5000);
}

bool ESP8266AT::setup(int baud){
  String data;
  serial.begin(baud);
  delay(100);
  rx_empty();
  serial.println("AT+CWMODE=1");
  delay(100);
  serial.println("AT+CIPMUX=0");
  delay(100);
  String tar[] = {"OK", "FAIL", "no"};
  data = recvString(tar, 10000);
  if (data.indexOf("OK") != -1) {
    Serial.println("Ready!");
    return true;
  }
  Serial.print("Oops!");
  return false;
}

bool ESP8266AT::begin(String ssid, String pwd){
  rx_empty();
  String data;
  serial.print("AT+CWJAP=\"");
  serial.print(ssid);
  serial.print("\",\"");
  serial.print(pwd);
  serial.println("\"");
  String tar[] = {"OK", "FAIL", "no"};
  data = recvString(tar, 10000);
  if (data.indexOf("OK") != -1) {
    Serial.println("Connected to AP");
    return true;
  }
  Serial.print("Oops!");
  return false;
}

bool ESP8266AT::SEND(int len){
    rx_empty();
    serial.print("AT+CIPSEND=");
    serial.println(len);
    if (recvFind(">", 10000)) {
    return true;
    }
    stop();
    return false;
}

bool ESP8266AT::connect(char addr[], int port){
  String data;
  String add = String(addr);
  String por = String(port, DEC);
  rx_empty();
  serial.print("AT+CIPSTART=\"TCP\",\"");
  serial.print(add);
  serial.print("\",");
  serial.println(por);
  String tar[] = {"OK","ERROR", "ALREADY CONNECT", "no"};
  // data = recvString("OK","ERROR", "ALREADY CONNECT", 10000);
  data = recvString(tar, 10000);
  if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1) {
      Serial.println("Connected to Server");
      return true;
  }
  return false;
}

String ESP8266AT::recvString(String target, uint32_t timeout){
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(serial.available() > 0) {
            a = serial.read();
			if(a == '\0') continue;
            data += a;
        }
        if (data.indexOf(target) != -1) {
            break;
        }
    }
    return data;
}

String ESP8266AT::recvString(String *target, uint32_t timeout){
    String data;
    int i, x = 0;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        i =0;
        while(serial.available() > 0) {
          a = serial.read();
			    if(a == '\0') continue;
          data += a;
        }
        while (target[i] != "no"){
          if (data.indexOf(target[i]) != -1){
            x = 1;
            break;
          }
          i++;
        }
        if(x==1){break;}

      }
    return data;
}

bool ESP8266AT::recvFind(String target, uint32_t timeout){
    String data_tmp;
    data_tmp = recvString(target, timeout);
    if (data_tmp.indexOf(target) != -1) {
        return true;
    }
    return false;
}

bool ESP8266AT::recvFind(String *target, uint32_t timeout){
  String data_tmp;
  int i = 0;
  data_tmp = recvString(*target, timeout);
  while (target[i] != "no"){
    if (data_tmp.indexOf(target[i]) != -1) {
      return true;
    }
    i++;
  }
  return false;
}
