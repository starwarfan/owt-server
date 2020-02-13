{
  'targets': [{
    'target_name': 'videoMixer-sw',
    'variables': {
      # relative source dir path
      'source_rel_dir': '../../../..',
      # absolute source dir path
      'source_abs_dir%': '<(module_root_dir)/../../../..',
      # absolute webrtc dir path
      'webrtc_abs_dir%': '<(module_root_dir)/../../../../../third_party/webrtc-m79'
    },
    'sources': [
      '../addon.cc',
      '../VideoMixerWrapper.cc',
      '../SoftVideoCompositor.cpp',
      '../VideoMixer.cpp',
      '<(source_rel_dir)/core/owt_base/I420BufferManager.cpp',
      '<(source_rel_dir)/core/owt_base/MediaFramePipeline.cpp',
      '<(source_rel_dir)/core/owt_base/FrameConverter.cpp',
      '<(source_rel_dir)/core/owt_base/VCMFrameDecoder.cpp',
      '<(source_rel_dir)/core/owt_base/VCMFrameEncoder.cpp',
      '<(source_rel_dir)/core/owt_base/FFmpegFrameDecoder.cpp',
      '<(source_rel_dir)/core/owt_base/FFmpegDrawText.cpp',
      '<(source_rel_dir)/core/owt_base/SVTHEVCEncoder.cpp',
    ],
    'cflags_cc': [
        '-Wall',
        '-O$(OPTIMIZATION_LEVEL)',
        '-g',
        '-std=c++11',
        '-DWEBRTC_POSIX',
        '-DENABLE_SVT_HEVC_ENCODER',
    ],
    'cflags_cc!': [
        '-fno-exceptions',
    ],
    'include_dirs': [
      '<(source_rel_dir)/core/common',
      '<(source_rel_dir)/core/owt_base',
      '<(webrtc_abs_dir)/src', # webrtc include files
      '<(webrtc_abs_dir)/src/third_party/abseil-cpp', # abseil-cpp include files used by webrtc
      '<(webrtc_abs_dir)/src/third_party/libyuv/include', # libyuv include files used by webrtc
      '<(source_rel_dir)/../build/libdeps/build/include',
    ],
    'libraries': [
      '-lboost_thread',
      '-llog4cxx',
      '-L<(webrtc_abs_dir)', '-lwebrtc',
      '-L<(source_abs_dir)/../third_party/openh264', '-lopenh264',
      '<!@(pkg-config --libs libavutil)',
      '<!@(pkg-config --libs libavcodec)',
      '<!@(pkg-config --libs libavformat)',
      '<!@(pkg-config --libs libavfilter)',
      '-L<(source_abs_dir)/../build/libdeps/build/lib', '-lSvtHevcEnc',
    ],
  }]
}
