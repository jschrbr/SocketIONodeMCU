/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>

SocketIOClient socket;
const char* ssid     = "SSID";
const char* password = "WPA KEY";

char host[] = "192.168.0.5";
int port = 3000;
extern String fun;
extern String dat;
String message;


void setup() {
  Serial.begin(115200);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  if (!socket.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }

  if (socket.connected()){
    Serial.println("Emit Connection");
    socket.emit("connection", "Connected !!!!");
  }
}

void loop() {
  if (Serial.available()){
    message = (Serial.readString());
    message = message.substring(0, message.indexOf("\n")-1);
    socket.emit("chat message", message);
    Serial.read();
  }

  if (socket.on() && (fun == "chat message")){
    Serial.print("Message:\t");
    Serial.println(dat);
  }
}
