#include <SocketIOClient.h>

SocketIOClient socket;

String ssid     = "SSID";
String password = "PASSWORD";

char host[] = "192.168.0.5";
int port = 3000;
extern String fun;
extern String dat;
#define buttonPin PUSH1

void setup(){
  pinMode(buttonPin,INPUT_PULLUP);
  Serial.begin(115200);  // set pin as output
  socket.begin(115200);
  socket.start(ssid, password);
    if (!socket.connect(host, port)) {
      Serial.println("connection failed");
    }
    if (socket.connected()){
      socket.emit("chat message", "Connected !!!!");
      delay(1000);
    }
}

void loop(){
  if (Serial.available()){
    String message = (Serial.readString());
    socket.emit("chat message", message);
    Serial.read();
  }

  if(socket.on() && (fun == "chat message")){
      Serial.print("message:\t");
      Serial.println(dat);
  }

  int val = !digitalRead(buttonPin);
  if (val == HIGH) {socket.emit("chat message", "Hiya");}
}
