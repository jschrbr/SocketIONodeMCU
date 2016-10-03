var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var ucount = 0;

app.get('/', function(req,res){
        res.sendFile(__dirname + '/index.html');
});

io.on('connection', function(socket){
        console.log("User Connected!");

        socket.on('disconnect', function(){
        console.log("User Disconnected");
        });

        socket.on('chat message', function(msg){
                console.log('message: ' + msg);
                io.emit('chat message', msg);
        });
});

http.listen(3000, function(){
        console.log('listening on *:3000');
});
