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
            console.log(remoteDesc);
            myRTC._peerConnection.setRemoteDescription( remoteDesc );  
            if ( myRTC._isCaller === false) {
                myRTC._peerConnection.createAnswer( myRTC._onLocalDescription , null, { 'mandatory': { 'OfferToReceiveAudio': true, 'OfferToReceiveVideo': true } });
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
        myRTC._peerConnection.createOffer(myRTC._onLocalDescription);
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
        myRTC._peerConnection.setLocalDescription(desc);
        console.log(desc);
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
        console.log( stream );
        if ( myRTC._remoteVideo != null ) {
            //myRTC._remoteVideo.src = webkitURL.createObjectURL(stream.stream);
            attachMediaStream( myRTC._remoteVideo, stream.stream);
        }
    },

    _init: function(remote, remoteVideo, stream) {
        myRTC._remote = remote;
        myRTC._remoteVideo = remoteVideo;
        myRTC._localStream = stream;

        myRTC._peerConnection = new RTCPeerConnection(IceServers);
        myRTC._peerConnection.onicecandidate = myRTC._onLocalCandidate;
        myRTC._peerConnection.onaddstream = myRTC._onAddedRTCStream;
        if ( stream != null) {
            myRTC._peerConnection.addStream( stream);
        }
    },

    _remote:         "",
    _remoteVideo:    null,
    _localStream:    null,
    _isCaller:       true,
    _peerConnection: null
};

