#
#   Building script for ipcamera application
#
{
    'includes': ['build/common.gypi'],
    'targets' : [
    {
        'target_name': 'simplertc',
        'type': 'executable',
        'dependencies': [
            'libjingle.gyp:libjingle_peerconnection',
            '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
            '<(DEPTH)/third_party/expat/expat.gyp:expat',             
        ],
        'defines': [
            'GOOGLE_ENGINE',
        ],
        'sources': [
            'simplertc/main.cpp',
            'simplertc/peer.h',
            'simplertc/peer.cpp',
            'simplertc/simplertc.h',
            'simplertc/simplertc.cpp',
            'simplertc/rtcstream.h',
            'simplertc/rtcstream.cpp',
            'simplertc/simplerenderer.h',
            'simplertc/simplerenderer.cpp',
            'simplertc/simplecapturer.h',
            'simplertc/simplecapturer.cpp',
            'simplertc/simpleaudiodevice.h',
            'simplertc/simpleaudiodevice.cpp',
            'simplertc/simpleaudiodevice.cpp',
        ],
    },
    ],
}
