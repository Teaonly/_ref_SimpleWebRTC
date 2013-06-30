#
#   Building script for ipcamera application
#
{
    'includes': ['build/common.gypi'],
    'targets' : [
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
        ],
        'cflags': [
            '<!@(pkg-config --cflags glib-2.0 gobject-2.0 gtk+-2.0)',
        ],
        'link_settings': {
            'ldflags': [
                '<!@(pkg-config --libs-only-L --libs-only-other glib-2.0'
                    ' gobject-2.0 gthread-2.0 gtk+-2.0)',
            ],
            'libraries': [
                '<!@(pkg-config --libs-only-l glib-2.0 gobject-2.0'
                        ' gthread-2.0 gtk+-2.0)',
                '-lX11',
                '-lXcomposite',
                '-lXext',
                '-lXrender',
            ],
        },
    },
    {
        'target_name': 'simplemedia',
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
            'simplertc/simplevideo.h',
            'simplertc/simplevideo.cpp',
            'simplertc/simplerenderer.h',
            'simplertc/simplerenderer.cpp',
        ],
    },
    {
        'target_name': 'libsimplertc_so',
        'type': 'loadable_module',
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
            'simplertc/main_jni.cpp',
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
            'simplertc/simplevideo.h',
            'simplertc/simplevideo.cpp',
            'simplertc/simplerenderer.h',
            'simplertc/simplerenderer.cpp',
        ],
    },
    ],
}
