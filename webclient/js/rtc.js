//////////////////////////////////////////////
// RTC object define
//////////////////////////////////////////////
SessionDescription = window.webkitRTCSessionDescription || window.mozRTCSessionDescription || window.RTCSessionDescription;
IceCandidate = window.webkitRTCIceCandidate || window.mozRTCIceCandidate || window.RTCIceCandidate;
PeerConnection = window.webkitRTCPeerConnection || window.mozRTCPeerConnection || window.RTCPeerConnection;
IceServers = { "iceServers": [ ] };

var myRTC = {
    onMessageOut: null,
    
    processRTCMessage:  function(msg) {
        if ( msg.length === 3 && msg[0] === "rtc" && msg[1] === "desc" ) {
            var desc = Base64.decode( msg[2] );
            var remoteDesc = new SessionDescription(JSON.parse(desc));
            myRTC._peerConnection.setRemoteDescription( remoteDesc );  
            if ( myRTC._isCaller === false) {
                myRTC._peerConnection.createAnswer( myRTC._onLocalDescription );
            }
        } else if ( msg.length === 3 && msg[0] === "rtc" && msg[1] === "cand" ) {
            var cand = Base64.decode(msg[2]);
            var candObj =  new IceCandidate(JSON.parse(cand));
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
            myRTC._remoteVideo.src = webkitURL.createObjectURL(stream.stream);
        }
    },

    _init: function(remote, remoteVideo, stream) {
        myRTC._remote = remote;
        myRTC._remoteVideo = remoteVideo;
        myRTC._localStream = stream;

        myRTC._peerConnection = new PeerConnection(IceServers);
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

