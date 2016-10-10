# Socket.io NodeMCU
Socket.io Library for Arduino IDE.
Specifically for ESP8266 boards.

This is a stripped down version of:
      https://github.com/washo4evr/Socket.io-v1.x-Library.

The library implements socket.io, in a very similar syntax to its javascript equivalent.
It works with the chat app tutorial found at:
      http://socket.io/get-started/chat/.

Can be useful for those new to socket.io, or the ESP8266/NodeMCU.

Or just quick and easy prototyping for web enabled device.







      socket.emit("chat message", message);     // Easily emit function name and data.

      if (socket.on() && (fun == "chat message")){    // Or read whats being sent on the socket.
        Serial.print("Message:\t");
        Serial.println(dat);
      }





The library supplies a sample '.ino' for ESP8266.

As well, a sample code for node.js server.
Dont forget to npm install your dependencies!
