var myPeer = {
    // callbacks
    onLogin:            null,
    onLogout:           null,
    onRemoteLogin:      null,
    onRemoteLogout:     null,
    onMessage:          null,

    // public stuff
    name:       "",
    role:       "",
    state:     -1,           //-1 offline; 0 connecting; 1 online;
    login: function (name, role, addr) {
        myPeer.state = 0;
        myPeer.name = name;
        myPeer.role = role;
        myPeer._conn = new  WebSocket(addr, "dialog");
        myPeer._conn.onopen = myPeer._onOpen;
        myPeer._conn.onmessage = myPeer._onMessage;
        myPeer._conn.onclose = myPeer._onClose;
        myPeer._xml = "";
    },
    logout: function () {
        myPeer._conn.close();        
    },
    sendMessage: function (remote, text) {
        myPeer._conn.send("<message:" + remote + ":" + text + ">"); 
    },

    // private stuff
    _conn:          null,
    _xml:           "",
    _onOpen: function() {
        myPeer._conn.send("<login:" + myPeer.name + ":" + myPeer.role  + ">");
    },
    _onClose: function() {
        delete myPeer._conn;
        myPeer._conn = null;
        myPeer._xml = null;
        myPeer.onLogout("network");
    },
    _onMessage: function(evt) {
        var msg = evt.data;
        for(var i =0; i < msg.length; i++) {
            myPeer._xml += msg.substr(i,1);
            if ( msg.charAt(i) == '>') {
                myPeer._processXML();
                myPeer._xml = "";
            }
        }
    },
    _processXML:function() {
        var wordBegin = myPeer._xml.indexOf("<");
        var wordEnd = myPeer._xml.indexOf(">");
        var sentence = myPeer._xml.substring(wordBegin+1, wordEnd);   
        var words = sentence.split(":");	
        
        if ( words.length == 0) 
            return;

        if ( words[0] == "login" && words.length == 2) {
            if ( words[1] == "ok" ) {
                if ( myPeer.state == 0) {
                    myPeer.state = 1;
                    myPeer.onLogin();
                } 
            } else if ( words[1] == "error" ) {
                delete myPeer._conn;
                myPeer._conn = null;
                myPeer._xml = null;
                myPeer.onLogout("error");
                return;
            }
        } 
        
        if ( myPeer.state <= 0) 
           return; 
        
        if ( words[0] == "online" && words.length == 3) {
            myPeer.onRemoteLogin( words[1], words[2] );
            return;
        }    
        
        if ( words[0] == "offline" && words.length == 3){
            myPeer.onRemoteLogout( words[1] );
            return;
        }
        
        if ( words[0] == "message" && words.length >= 3) {
            myPeer.onMessage( words[1], words.slice(2, words.length) );
            return;
        }
           
    }

};

