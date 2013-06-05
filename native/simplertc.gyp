#
#   Building script for ipcamera application
#
{
  'includes': ['build/common.gypi'],
  'targets' : [
    {
        'target_name': 'turnserver',
        'type': 'executable',
        'dependencies': [
            'libjingle.gyp:libjingle_p2p',
        ],
        'sources': [
            'p2p/base/turnserver_main.cc',
        ],
    },
    {
      'target_name': 'simplertc',
      'type': 'executable',
      'include_dirs': [ 
          '../third_party/webrtc/modules/interface',
      ], 
      'dependencies': [
        'libjingle.gyp:libjingle_peerconnection',
        '<(DEPTH)/third_party/icu/icu.gyp:icuuc',
        '<(DEPTH)/third_party/jsoncpp/jsoncpp.gyp:jsoncpp',
        '<(DEPTH)/third_party/libvpx/libvpx.gyp:libvpx',
         '<(DEPTH)/third_party/expat/expat.gyp:expat',             
      ],
      'sources': [
        'simplertc/main.cpp',
        'simplertc/peer.h',
        'simplertc/peer.cpp',
        'simplertc/simplertc.h',
        'simplertc/simplertc.cpp',
        'simplertc/rtcfactory.h',
        'simplertc/rtcfactory.cpp',
        'simplertc/rtcstream.h',
        'simplertc/rtcstream.cpp',
        'simplertc/simplemedia.h',
        'simplertc/simplemedia.cpp',
        'simplertc/simplevoice.h',
        'simplertc/simplevoice.cpp',
      ],
    },
  ]
}
