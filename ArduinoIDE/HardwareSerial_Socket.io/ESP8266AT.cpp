#include <ESP8266AT.h>

void ESP8266AT::sendSocket(String data) {
  Serial1.print(data);
}

bool ESP8266AT::sendSocket(String *data, int count) {
  rx_empty();
  // String tar[] = {"STATUS:3","OK", "no"};
  // if (recvFind(tar, 10)){return;}
  int i = 0;
  while(i<count){
  Serial1.print(data[i]);
  Serial.print(data[i]);
  ++i;
  }
  return recvFind("SEND OK", 5000);
}

void ESP8266AT::sendSocket(char data) {
  Serial1.print(data);
}

void ESP8266AT::sendSocket(int data) {
  Serial1.print(data);
}

void ESP8266AT::rx_empty(void){
    while(Serial1.available() > 0) {
        Serial1.read();
    }
}

void ESP8266AT::sendHandshake(char hostname[]) {
	Serial1.println(F("GET /socket.io/1/?transport=polling&b64=true HTTP/1.1"));
	Serial1.print(F("Host: "));
	Serial1.println(hostname);
	Serial1.println(F("Origin: Arduino"));
	Serial1.println("");
	Serial.println("Handshake Sent");
  return;
}

void ESP8266AT::readHandshake(char sid[], char hostname[]) {
  Serial1.print(F("GET /socket.io/1/websocket/?transport=websocket&b64=true&sid="));
  Serial1.print(sid);
  Serial1.println(F(" HTTP/1.1"));
  Serial1.print(F("Host: "));
  Serial1.println(hostname);
  Serial1.println(F("Origin: ArduinoSocketIOClient"));
  Serial1.print(F("Sec-WebSocket-Key: "));
  Serial1.println(sid);
  Serial1.println(F("Sec-WebSocket-Version: 13"));
  Serial1.println(F("Upgrade: websocket"));	// must be camelcase ?!
  Serial1.println(F("Connection: Upgrade"));
  Serial1.println("");
  return;
}

bool ESP8266AT::connected(){
  String data;
  rx_empty();
  Serial1.println("AT+CIPSTATUS");
  String tar[] = {"STATUS:4", "ERROR", "no"};
  return !recvFind(tar,60);
}

bool ESP8266AT::stop(){
  rx_empty();
  Serial1.println("AT+CIPCLOSE");
  return recvFind("OK", 5000);
}

bool ESP8266AT::setup(int baud){
  String data;
  Serial1.begin(baud);
  delay(100);
  rx_empty();
  Serial1.println("AT+RESTORE");
  delay(7000);
  Serial1.println("AT+CWMODE=1");
  delay(100);
  Serial1.println("AT+CIPMUX=0");
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
  Serial1.print("AT+CWJAP=\"");
  Serial1.print(ssid);
  Serial1.print("\",\"");
  Serial1.print(pwd);
  Serial1.println("\"");
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
    Serial1.print("AT+CIPSEND=");
    Serial1.println(len);
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
  Serial1.print("AT+CIPSTART=\"TCP\",\"");
  Serial1.print(add);
  Serial1.print("\",");
  Serial1.println(por);
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
        while(Serial1.available() > 0) {
            a = Serial1.read();
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
        while(Serial1.available() > 0) {
          a = Serial1.read();
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
