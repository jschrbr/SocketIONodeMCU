//#define ESP8266
#include <SocketIOClient.h>

String tmpdata = "";	//External variables
String fun = "";
String dat = "";

bool SocketIOClient::begin(int baud) {
	return 	client.setup(baud);
}
bool SocketIOClient::start(String ssid, String pwd) {
	return 	client.begin(ssid, pwd);
}
bool SocketIOClient::connected() {
	return client.connected();
}
void SocketIOClient::disconnect() {
	client.stop();
}

bool SocketIOClient::connect(char thehostname[], int theport) {
	if (!client.connect(thehostname, theport)) return false;
	delay(500);
	if(!client.SEND(94)){Serial.println("Cant Send");return false;}
	hostname = thehostname;
	port = theport;
	client.sendHandshake(hostname);
	return readHandshake();
}

void SocketIOClient::socker(int index) {
	String rcvdmsg = "";
	int sizemsg = databuffer[index + 1];   // 0-125 byte, index ok        Fix provide by Galilei11. Thanks
	if (databuffer[index + 1]>125){
		sizemsg = databuffer[index + 2];    // 126-255 byte
		index += 1;       // index correction to start
	}
	for (int i = index + 2; i < index + sizemsg + 2; i++)
		rcvdmsg += (char)databuffer[i];
	String mask = rcvdmsg;
	switch (rcvdmsg[0])
	{
	case '2':
		Serial.println("Ping received - Sending Pong");
		heartbeat(1);
		break;
	case '3':
		Serial.println("Pong received - All good");
		break;
	case '4':
		switch (rcvdmsg[1])
		{
		case '0':
			Serial.println("Upgrade to WebSocket confirmed");
			break;
		case '2':
		fun = rcvdmsg.substring(4, rcvdmsg.indexOf("\","));
		dat = rcvdmsg.substring(rcvdmsg.indexOf("\",")+3, (sizemsg-2));
		break;
		}
	}
}

bool SocketIOClient::on() {
	int index = -1;
	int index2 = -1;
	String tmp = "";
	*databuffer = 0;
	tmp = client.recvString("+IPD", 100);
	tmp.concat(client.recvString("]", 100));
	tmp.concat(client.recvString("+IPD", 100));
	tmp.concat(client.recvString("]", 100));
	int tmplen = tmp.length();
	tmp.toCharArray(databuffer, tmplen);
	index = tmp.indexOf((char)129);	//129 DEC = 0x81 HEX = sent for proper communication
	index2 = tmp.indexOf((char)129, index + 1);
	if (index != -1){
		socker(index);
	}
	if (index2 != -1){
		socker(index2);
	}
	// if (!emittin){
	// 	if (!connected()) {
	// 		Serial.println("");
	// 		Serial.println("Reconnecting...");
	// 		if (!connect( hostname, port)) return 0;
	// 	}
	// 	client.rx_empty();
	// }
	if ((index == -1)&&(index2 == -1)){return 0;}
	return 1;
}

bool SocketIOClient::readHandshake() {

	String dat = client.recvString("upgrades",10000);
	int sidindex = dat.indexOf("sid\":\"");
	int count = 20;
	for (int i = 0; i < count; i++){
		sid[i] = dat[i + sidindex+6];
	}
	Serial.print(F("Connected. SID="));
	Serial.println(sid);	// sid:transport:timeout
	if(!client.SEND(254)){Serial.println("Cant Send");return false;}
	Serial.println(F("Connecting via Websocket"));
	client.readHandshake(sid, hostname);
	dat = client.recvString("Date",1000); // contains sec key
	// Generating a 32 bits mask requiered for newer version
	// Client has to send "52" for the upgrade to websocket
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "52";
	String message = "52";
	for (int i = 0; i < 4; i++){
		char a = random(48, 57);
		mask += a;
	}
	for (unsigned int i = 0; i < message.length(); i++){
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("52")
	}
	if(!client.SEND(2+mask.length()+masked.length())){Serial.println("Cant Send");return false;}
	client.sendSocket((char)0x81);
	client.sendSocket((char)130);
	client.sendSocket(mask);
	client.sendSocket(masked);
	Serial.println("Connected to Socket.IO");
	on();
	return true;
}

void SocketIOClient::emit(String fun, String dat) {
	if (dat.indexOf("\n") != -1){dat = dat.substring(0, dat.indexOf("\n")-1);}
	emittin = true;
delay(200);
client.rx_empty();
	String message = "42[\"" + fun + "\",\"" + dat + "\"]";
	if ((!sent)|| client.recvFind("STATUS:3",100)) {
		Serial.print("Could not send:\t");
		Serial.println(message);
		return;
	}
	int header[10], j = 0;
	header[0] = 0x81;
	j++;
	int msglength = message.length();
	randomSeed(analogRead(0));
	String mask = "";
	String masked = message;
	for (int i = 0; i < 4; i++){
		char a = random(48, 57);
		mask += a;
	}
	int masklen = mask.length();
	for (int i = 0; i < msglength; i++){
		masked[i] = message[i] ^ mask[i % 4];
	}
									//Depending on the size of the message
	if (msglength <= 125){
		header[1] = msglength + 128;
		j++;
		String head[] = {String((char)header[0]), String((char)header[1]), mask, masked};

		client.rx_empty();
		if(!client.SEND(j+msglength+masklen)){Serial.println("Cant Send"); emittin = false;return;}
		sent = client.sendSocket(head, (j+2));
		emittin = false;
	}
	else if (msglength >= 126 && msglength <= 65535){
		header[1] = 126 + 128; j++;
		header[2] = (msglength >> 8) & 255; j++;
		header[3] = (msglength)& 255; j++;
		String head[] = {String((char)header[0]), String((char)header[1]), String((char)header[2]), String((char)header[3]), mask, masked};

		client.rx_empty();
		if(!client.SEND(j+msglength+masklen)){Serial.println("Cant Send");emittin = false;return;}
		sent = client.sendSocket(head, (j+2));
		emittin = false;
	}
	else{
		header[1] = 127 + 128; j++;
		header[2] = (msglength >> 56) & 255; j++;
		header[3] = (msglength >> 48) & 255; j++;
		header[4] = (msglength >> 40) & 255; j++;
		header[5] = (msglength >> 32) & 255; j++;
		header[6] = (msglength >> 24) & 255; j++;
		header[7] = (msglength >> 16) & 255; j++;
		header[8] = (msglength >> 8) & 255; j++;
		header[9] = (msglength)& 255; j++;
		String head[] = {String((char)header[0]), String((char)header[1]), String((char)header[2]), String((char)header[3]),String((char)header[4]),String((char)header[5]),String((char)header[6]),String((char)header[7]),String((char)header[8]),String((char)header[9]), mask, masked};

		client.rx_empty();
		if(!client.SEND(j+msglength+masklen)){Serial.println("Cant Send");emittin = false;return;}
		sent = client.sendSocket(head, (j+2));
		emittin = false;
	}
}

void SocketIOClient::heartbeat(int select) {
	randomSeed(analogRead(0));
	String mask = "";
	String masked = "";
	String message = "";
	String sendat;
	if (select == 0){
		masked = "2";
		message = "2";
	}
	else{
		masked = "3";
		message = "3";
	}
	//generate a random mask, 4 bytes, ASCII 0 to 9
	for (int i = 0; i < 4; i++)	{
		char a = random(48, 57);
		mask += a;
	}

	for (int i = 0; i < 1; i++)
		masked[i] = message[i] ^ mask[i % 4];	//apply the "mask" to the message ("2" : ping or "3" : pong)

	if(!client.SEND(2+mask.length()+masked.length())){Serial.println("Cant Send");return;}
	client.sendSocket((char)0x81);	//has to be sent for proper communication
	client.sendSocket((char)129);	//size of the message (1) + 128 because message has to be masked
	client.sendSocket(mask);
	client.sendSocket(masked);
}
