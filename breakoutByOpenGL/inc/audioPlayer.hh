#pragma once

#include <string>
#include <unordered_map>
extern "C" {
#include <AL/al.h>
#include <AL/alc.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}

class AudioPlayer {
public:
  std::unordered_map<std::string, ALuint> Sources;
  AudioPlayer();
  bool CreateSource(const std::string &file, std::string name);
  void Init();
  bool Play(const std::string &name, bool loop = false);
  bool Display(const std::string &name);
};
