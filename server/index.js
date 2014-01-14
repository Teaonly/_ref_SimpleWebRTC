// Global function
Array.prototype.remove = function(e) {
  for (var i = 0; i < this.length; i++) {
    if (e == this[i]) { return this.splice(i, 1); }
  }
};

// PeerClass
function PeerClass(sock) {
    this.pid = null;
    this.role = null;
    this.xmlWord = "";
    this.sid = sock.remoteAddress + sock.remotePort;
    this.sock = sock;
}
var peerObjects = {};

// PeerServer
var PeerServer = {};

PeerServer.processDialog = function(peer) {
    var wordBegin = peer.xmlWord.indexOf("<");
    var wordEnd = peer.xmlWord.indexOf(">");
    var sentence = peer.xmlWord.substring(wordBegin+1, wordEnd);
    
    var words = sentence.split(":");
    if ( words[0] == "login" && words.length == 3) {
        // kickoff the new one with same id
        if ( peerObjects.hasOwnProperty(words[1]) ) {
            peer.sock.write("<login:error>");
            peer.sock.end();
            return false;
        }

        // Ok, this is valid user
        peer.pid = words[1];
        peer.role = words[2];
        peer.sock.write("<login:ok>");

        // send the online list 
        for (var p in peerObjects) {
            var pid = peerObjects[p].pid;
            var xml = "<online:" + pid + ":" + peerObjects[p].role + ">";
            peer.sock.write(xml);
        }
       
        //tell to all the others 
        var xml = "<online:" + peer.pid + ":" + peer.role + ">";
        for(var p in peerObjects) {
            peerObjects[p].sock.write(xml);
        } 

        //added to hash
        peerObjects[peer.pid] = peer;         
        console.log("New user " + peer.pid);

    } else if ( words[0] == "message" && words.length >= 3) {
        if ( peer.pid == null)                  //login first
            return false;
        
        var remote = words[1];

        if ( peerObjects.hasOwnProperty(remote) ) {
            var remotePeer = peerObjects[remote];
            var sndMessage = "<message:" + peer.pid;
            for ( var i = 2; i < words.length; i++) {
                //remotePeer.sock.write("<message:" + peer.pid + ":" + content + ">");
                sndMessage = sndMessage + ":" + words[i];
            }
            sndMessage = sndMessage + ">"
            remotePeer.sock.write(sndMessage);
        }
    }

    return true;
}

PeerServer.onAccept = function(sock) {
    var peer = new PeerClass(sock);    

   return peer;
};

PeerServer.onClose = function(peer, hasError) {
    if ( peerObjects.hasOwnProperty(peer.pid) ) {
        
        // We delete *REAL* user only
        if ( peer.sid != peerObjects[peer.pid].sid ) {
            return;
        }

        console.log("Delete user " + peer.pid);
        // remove from hash
        delete peerObjects[peer.pid];
        var xml = "<offline:" + peer.pid + ":" + peer.role + ">";

        // tell the others
        for (var p in peerObjects) {
            peerObjects[p].sock.write(xml);
        }     
    }
}

PeerServer.onData = function(peer, d) {
    for(var i = 0; i < d.length; i++) {
        peer.xmlWord += d.substr(i,1);

        if(d.charAt(i) == '>') {
            PeerServer.processDialog(peer);
            peer.xmlWord = "";
        }
    };
    return; 
}

// create the server for raw tcp user
var net = require("net");
net.createServer(function(sock) {
    
    sock.setTimeout(0);
    sock.setEncoding("utf8");
    var peer = PeerServer.onAccept(sock);
    
    sock.addListener("error", function(){
        PeerServer.onClose(peer);    
    });
    sock.addListener("close", function(){
        PeerServer.onClose(peer);
    });

    sock.addListener("data", function(d){
        PeerServer.onData(peer, d);
    });
    
}).listen(19790);        

///////////////////////////////////////////////////////////////////////////////////////////

// Prepare for the WebSocket 
var WebSocketServer = require('websocket').server;
var WebSocketConnection = require('websocket').connection;

WebSocketConnection.prototype.end = function() {
        this.close();
};    
WebSocketConnection.prototype.write = function(msg) {
    this.sendUTF(msg);
};

// Create websocket server
var http = require('http');
var server = http.createServer(function(request, response) {
    console.log((new Date()) + ' Received request for ' + request.url);
    response.writeHead(404);
    response.end();
});
server.listen(19800, function() {
});
wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false
});

// setup the websocket server
wsServer.on('request', function(request) {
    var connection = request.accept('dialog', request.origin);

    var peer = PeerServer.onAccept(connection);
    connection.on('message', function(message) {
        PeerServer.onData(peer, message.utf8Data);
    });
    connection.on('close', function(reasonCode, description) {
        PeerServer.onClose(peer);
    });

});

////////////////////////////////////////////////////////////////////////////////////////
// A simple static file based http server
if ( process.argv.length < 3) {
    console.log("Please input webclient folder path");
    process.exit(-1);
}

var webPath = process.argv[2];

var connect = require('connect');
connect.createServer(
        connect.static(webPath)
    ).listen(18080);
    
console.log("=========================================");
console.log("   Peer server is running.... ");
console.log("   The client tcp port is 19790");
console.log("   The websocket port is 19800");
console.log("   The webclient is http://localhost:18080");
console.log("   The webclient folder is " + webPath);
console.log("=========================================");
