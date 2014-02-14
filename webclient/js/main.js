//-------------------------------------------
//  global variable 
//-------------------------------------------
var myConfig =  {
    basicURL:       "",
    sessionAddr:    "",
    sessionPort:    19800,
    name:           "",
    role:           "webclient",
    remoteVideo:    null,
    remote:         "",
    stream:         null,
    state:          0              // 0: 1: 2
};

function Peer(name, role) {
    this.name = name;
    this.role = role;
}
var onlinePeers = {};
var currentPage = null;

//-------------------------------------------
//  GUI modules
//-------------------------------------------
var dialogInfo = {
    show: function(msg, to) {
        $('#head_message').html(msg);
        $('#dialog_info').modal("show");
        if ( to != undefined && to > 0 ) {
            setTimeout(function() {
                $("#dialog_info").modal("hide");
            }, 3000); 
        }
    },
    hide: function() {
        $("#dialog_info").modal("hide");
    }
};

var pageLogin = {
    onSignin: function() {
        myConfig.name = $("#txt_name").val();
        myConfig.state = 0;
        if ( myConfig.name === "") {
            dialogInfo.show("Please input user name!", 2000);
            return;
        }
        dialogInfo.show("Connecting to server...");
        myPeer.login(myConfig.name, myConfig.role, myConfig.sessionAddr);
    }, 
    onSigninOK: function() {
        dialogInfo.hide();
        $("#page_index").hide();
    },
    onSigninError: function(evt) {
        dialogInfo.hide();
        dialogInfo.show("Login error...", 3000);
    },
};

var pageOnline = {
    show: function() {
        currentPage = pageOnline;
        $("#page_online").show();
        $(".tr_online").remove(); 
    },
    hide: function() {
        currentPage = null;
        $("#page_online").hide();
    },
    updateOnlines: function() {
        $(".tr_online").remove(); 
        for (var i in onlinePeers ) {
            var p = onlinePeers[i];
            var tr = "<tr class='tr_online'>";
            tr = tr + "<td>" + p.name + "</td><td>" + p.role + "</td>";
            tr = tr + "<td><button class='btn btn-mini btn-info btn_call1' type='button' user_name='" + p.name + "'>Call with Media</button>";
            tr = tr + "&nbsp;&nbsp;&nbsp;"
            tr = tr + "<button class='btn btn-mini btn-info btn_call2' type='button' user_name='" + p.name + "'>Call without Media</button>";
            tr = tr + "</tr>";
            $("#list_online").append(tr);
        }
        $(".btn_call1").bind("click", function() {
            var uname = $(this).attr("user_name");
            getUserMedia({audio:true, video:true}, function(stream) {
                startCallWithMedia(uname, stream);
            }, function() {
            }); 
        });
        $(".btn_call2").bind("click", function() {
            var uname = $(this).attr("user_name");
            getUserMedia({audio:true, video:true}, function(stream) {
                startCallWithoutMedia(uname, stream); 
            }, function() {
            }); 
           
        });

    }
};

var pageRTC = {
    show: function() {
        currentPage = pageRTC;
        $("#page_rtc").show();
    }         
};
//-------------------------------------------
//  global functions 
//-------------------------------------------
$(document).ready(function(){
    init();
});

var init = function() {
    // setup config 
    myConfig.basicURL = $(location).attr('href');
    myConfig.sessionAddr = "ws://" + $(location).attr('hostname') + ":" + myConfig.sessionPort;
    myConfig.remoteVideo = video_camera; 
    // binding GUI event
    currentPage = pageLogin;
    $("#btn_signin").bind("click", pageLogin.onSignin);

    // binding peer event
    myPeer.onLogout = onLogout;
    myPeer.onLogin = onLogin;
    myPeer.onRemoteLogout = onRemoteLogout;
    myPeer.onRemoteLogin = onRemoteLogin;
    myPeer.onMessage = onMessage;
    myRTC.onMessageOut = onRTCMessageOut;
};

var startCallWithMedia = function(remote, stream) {
    myConfig.state = 1;
    myConfig.remote = remote;
    myConfig.stream = stream;
    myPeer.sendMessage(remote, "call:media" ); 
};

var startCallWithoutMedia = function(remote, stream) {
    myConfig.state = 1;
    myConfig.remote = remote;
    myConfig.stream = stream;
    myPeer.sendMessage(remote, "call:null" ); 
};

var onLogin = function() {
    if ( currentPage === pageLogin) {
        pageLogin.onSigninOK();
        pageOnline.show(); 
    } 
};
var onLogout = function(evt) {
     if ( currentPage === pageLogin) {
        pageLogin.onSinginError(evt);
     }
};
var onRemoteLogin = function(name, role) {
    if ( onlinePeers.hasOwnProperty(name) ) {
        return;
    }    
    var peer = new Peer(name, role);
    onlinePeers[name] = peer;
    pageOnline.updateOnlines();
};
var onRemoteLogout = function(name) {
    if (!onlinePeers.hasOwnProperty(name) ) {
        return;
    }    
    delete onlinePeers[name];
    pageOnline.updateOnlines();
};

var onRTCMessageOut = function(remote, msg) {
    myPeer.sendMessage(remote, msg);
};
var onMessage = function(remote, msg) {
    if ( msg.length === 2 && msg[0] == "call" ) {
        if ( myConfig.state === 0) {
            if ( msg[1] === "media" ) {
                myConfig.remote = remote;
                myConfig.state = 1;
                getUserMedia({audio:true, video:true}, function(stream) {
                    myPeer.sendMessage(myConfig.remote, "call:ok"); 
                    myConfig.state = 2;
                    myConfig.stream = stream;
                    //myConfig.remoteVideo.src = webkitURL.createObjectURL(stream); 
                    myRTC.initWithAnswer( myConfig.remote, myConfig.remoteVideo, stream);
                    //myRTC.initWithCall( myConfig.remote, myConfig.remoteVideo, stream);
                    
                    pageOnline.hide();
                    pageRTC.show(); 
                }, function() {
                    myConfig.state = 0;
                    myPeer.sendMessage(myConfig.remote, "call:error");
                }); 
            } else if ( msg[1] === "null" ) {
                myConfig.remote = remote;
                myConfig.state = 2;
                myPeer.sendMessage(myConfig.remote, "call:ok");
                myConfig.stream = null;
                myRTC.initWithAnswer( myConfig.remote, myConfig.remoteVideo, null);
                pageOnline.hide();
                pageRTC.show();                
            } 
        } else if ( myConfig.state === 1 && myConfig.remote === remote) {
            if ( msg[1] === "error") {
                myConfig.stream.stop();
                myConfig.stream = null;
                myConfig.state = 0;
            } else if (msg[1] === "ok") {
                myConfig.state = 2;
                myRTC.initWithCall( myConfig.remote, myConfig.remoteVideo, myConfig.stream);
                pageOnline.hide();
                pageRTC.show();                
            }
        }
    } else if ( msg.length === 3 && msg[0] === "rtc" ) {
        if ( myConfig.state === 2 && myConfig.remote === remote ) {
            myRTC.processRTCMessage( msg ); 
        }
    }
};


