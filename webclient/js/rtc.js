//////////////////////////////////////////////
// RTC object define
//////////////////////////////////////////////
IceServers = { "iceServers": [ createIceServer("stun:stunserver.org:3478") ] };

var myRTC = {
    onMessageOut: null,
    
    processRTCMessage:  function(msg) {
        if ( msg.length === 3 && msg[0] === "rtc" && msg[1] === "desc" ) {
            var desc = Base64.decode( msg[2] );
            var remoteDesc = new RTCSessionDescription(JSON.parse(desc));
            console.log("<<<<<< RECV REMOVE DESC <<<<< ");
            myRTC._peerConnection.setRemoteDescription( remoteDesc , function() {
                    
                    }, function(err) {
                        console.log("######## setRemoteDescription error #####:" + err.message);
                    }); 
            if ( myRTC._isCaller === false) {
                //{ 'mandatory': { 'OfferToReceiveAudio': true, 'OfferToReceiveVideo': true } });
                console.log(">>>>> CREATE ANSWER >>>>>");
                myRTC._peerConnection.createAnswer( myRTC._onLocalDescription , function(err) {
                            console.log("######### createAnswer error ##########:" + err.message);
                        }, null);
            }
        } else if ( msg.length === 3 && msg[0] === "rtc" && msg[1] === "cand" ) {
            var cand = Base64.decode(msg[2]);
            var candObj =  new RTCIceCandidate(JSON.parse(cand));
            myRTC._peerConnection.addIceCandidate(candObj);
        }
    },    
    
    initWithCall: function(remote, remoteVideo, stream) {
        myRTC._isCaller = true;
        myRTC._init(remote, remoteVideo, stream);
        console.log(">>>>> CREATE OFFER >>>>>");
        myRTC._peerConnection.createOffer(myRTC._onLocalDescription, function(err) {
                        console.log("####### creatOffer error#########:" + err.message);
                }, null);
    },

    initWithData: function(remote) {
        myRTC._isCaller = true;
        myRTC._initData(remote);
        console.log(">>>>> CREATE DATA OFFER >>>>>");
        myRTC._peerConnection.createOffer(myRTC._onLocalDescription, function(err) {
                        console.log("####### creatOffer error#########:" + err.message);
                }, null);

    },

    initWithAnswer: function(remote, remoteVideo, stream) {
        myRTC._isCaller = false;
        myRTC._init(remote, remoteVideo, stream);
    },

    release: function() {
        if ( myRTC._peerConnection != null) {
            delete myRTC._peerConnection;
            myRTC._peerConnection = null; 
        }
    },

   // callbacks from PeerConnection and userMedia objects
    _onLocalDescription: function(desc) {
        console.log(">>>>>> get and set Local DESC>>>>");
        myRTC._peerConnection.setLocalDescription(desc, function() {
                    console.log(" >>>>>>> setLocalDescription is OK>>>");
                }, function(err) {
                    console.log("###### setLocalDescription error ###:" + err.message);
                });


        var descJsonObj = JSON.stringify(desc);
        var descBE = Base64.encode (descJsonObj);
        myRTC.onMessageOut(myRTC._remote, "rtc:desc:" + descBE );
    },    
    _onLocalCandidate:  function(evt) {
        if ( evt.candidate ) {    
            var candJsonObj = JSON.stringify(evt.candidate);
            var candBE = Base64.encode(candJsonObj);
            myRTC.onMessageOut(myRTC._remote, "rtc:cand:" + candBE);
        }
    },
    _onAddedRTCStream: function(stream) {
        if ( myRTC._remoteVideo != null ) {
            //myRTC._remoteVideo.src = webkitURL.createObjectURL(stream.stream);
            attachMediaStream( myRTC._remoteVideo, stream.stream);
        }
    },
    _onAddedDataChannel: function(evt) {
        console.log(">>>>> onAddedDataChannel >>>>");
        myRTC._dataChannel = evt.channel;
        myRTC._dataChannel.onopen = myRTC._onDataChannelState;
        myRTC._dataChannel.onclose = myRTC._onDataChannelState;
        myRTC._dataChannel.onmessage = myRTC._onDataChannelMessage;
    },

    _onDataChannelState: function() {
        var readyState = myRTC._dataChannel.readyState;
        console.log('Data channel state is: ' + readyState);
        if (readyState === "open") {
            myRTC._dataChannel.send("Hello World");        
        } else {
        }  
    },

    _onDataChannelMessage: function(evt) {
        console.log('Data channel receive message: ' + evt.data);
    },

    _init: function(remote, remoteVideo, stream) {
        myRTC._remote = remote;
        myRTC._remoteVideo = remoteVideo;
        myRTC._localStream = stream;

        myRTC._peerConnection = new RTCPeerConnection(IceServers,  {optional: [{RtpDataChannels: true}]});
        myRTC._peerConnection.onicecandidate = myRTC._onLocalCandidate;
        myRTC._peerConnection.onaddstream = myRTC._onAddedRTCStream;
        myRTC._peerConnection.ondatachannel = myRTC._onAddedDataChannel;
        if ( stream != null) {
            myRTC._peerConnection.addStream( stream);
        }
    },

    _initData: function(remote) {
        myRTC._remote = remote;

        myRTC._peerConnection = new RTCPeerConnection(IceServers,  {optional: [{RtpDataChannels: true}]});
        myRTC._peerConnection.onicecandidate = myRTC._onLocalCandidate;
        myRTC._dataChannel = myRTC._peerConnection.createDataChannel("sendDataChannel",{reliable: false});
        myRTC._dataChannel.onopen = myRTC._onDataChannelState;
        myRTC._dataChannel.onclose = myRTC._onDataChannelState;
        myRTC._dataChannel.onmessage = myRTC._onDataChannelMessage;
    },

    _remote:         "",
    _remoteVideo:    null,
    _localStream:    null,
    _isCaller:       true,
    _peerConnection: null,
    _dataChannel:    null
};

