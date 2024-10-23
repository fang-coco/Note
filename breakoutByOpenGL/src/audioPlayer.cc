#include "audioPlayer.hh"
#include "AL/al.h"
#include "AL/alc.h"
#include "libavcodec/packet.h"
#include "libavutil/frame.h"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

std::stringstream streamData;

static int output_audio_frame(AVFrame *frame) {
  int ret = -1;
  AVChannelLayout dst_ch_layout = AV_CHANNEL_LAYOUT_STEREO;
  int dst_rate = 44100;
  uint8_t **dst_data = NULL;
  int dst_nb_channels = 0;
  int dst_linesize;
  int dst_nb_samples;
  enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;

  /* create resampler context */
  auto swr_ctx = swr_alloc();
  if (!swr_ctx) {
    fprintf(stderr, "Could not allocate resampler context\n");
    return ret;
  }

  /* set options */
  av_opt_set_chlayout(swr_ctx, "in_chlayout", &frame->ch_layout, 0);
  av_opt_set_int(swr_ctx, "in_sample_rate", frame->sample_rate, 0);
  av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", (AVSampleFormat)frame->format,
                        0);

  av_opt_set_chlayout(swr_ctx, "out_chlayout", &dst_ch_layout, 0);
  av_opt_set_int(swr_ctx, "out_sample_rate", dst_rate, 0);
  av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", dst_sample_fmt, 0);

  /* initialize the resampling context */
  if ((ret = swr_init(swr_ctx)) < 0) {
    fprintf(stderr, "Failed to initialize the resampling context\n");
    return ret;
  }

  dst_nb_samples = av_rescale_rnd(frame->nb_samples, dst_rate,
                                  frame->sample_rate, AV_ROUND_UP);

  /* buffer is going to be directly written to a rawaudio file, no alignment
   */
  dst_nb_channels = dst_ch_layout.nb_channels;
  ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize,
                                           dst_nb_channels, dst_nb_samples,
                                           dst_sample_fmt, 0);
  if (ret < 0) {
    fprintf(stderr, "Could not allocate destination samples\n");
    return ret;
  }

  // 进行重采样
  ret = swr_convert(swr_ctx, dst_data, dst_nb_samples,
                    (const uint8_t **)frame->data, frame->nb_samples);
  int dst_bufsize = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                                               ret, dst_sample_fmt, 1);
  if (dst_bufsize < 0) {
    fprintf(stderr, "Could not get sample buffer size\n");
    return -1;
  }

  // for (int i = 0; i < out_frame->ch_layout.nb_channels; ++i) {
  //   streamData.write(reinterpret_cast<const char *>(out_frame->data[i]),
  //                    out_frame->linesize[i]);
  // }

  streamData.write(reinterpret_cast<const char *>(dst_data[0]), dst_bufsize);
  return 0;
}

static int decode_packet(AVCodecContext *dec, const AVPacket *pkt) {
  int ret = 0;

  // submit the packet to the decoder
  ret = avcodec_send_packet(dec, pkt);
  if (ret < 0) {
    fprintf(stderr, "Error submitting a packet for decoding (%s)\n",
            av_err2str(ret));
    return ret;
  }

  AVFrame *frame = av_frame_alloc();
  // get all the available frames from the decoder
  while (ret >= 0) {
    ret = avcodec_receive_frame(dec, frame);
    if (ret < 0) {
      // those two return values are special and mean there is no output
      // frame available, but there were no errors during decoding
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
        return 0;

      fprintf(stderr, "Error during decoding (%s)\n", av_err2str(ret));
      return ret;
    }

    ret = output_audio_frame(frame);

    av_frame_unref(frame);
  }
  av_frame_free(&frame);

  return ret;
}

static std::string loadPCMFromFile(const std::string &file) {

  AVFormatContext *formatCtx = nullptr;

  if (avformat_open_input(&formatCtx, file.c_str(), nullptr, nullptr) < 0) {
    throw std::runtime_error("Failed to open input file.");
  }

  if (avformat_find_stream_info(formatCtx, nullptr) < 0) {
    throw std::runtime_error("Failed to retrieve stream information.");
  }

  int ret =
      av_find_best_stream(formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  if (ret < 0) {
    throw std::runtime_error("Cannot find an audio stream in the input file");
  }
  int audioStreamIndex = ret;

  auto decoder = avcodec_find_decoder(
      formatCtx->streams[audioStreamIndex]->codecpar->codec_id);

  AVCodecContext *decCtx = avcodec_alloc_context3(decoder);
  if (!decCtx) {
    throw std::runtime_error("Failed to allocate decoding context.");
  }

  AVCodecParameters *codecParams =
      formatCtx->streams[audioStreamIndex]->codecpar;

  if (avcodec_parameters_to_context(decCtx, codecParams) < 0) {
    throw std::runtime_error("Failed to copy codec parameters to context.");
  }

  if (avcodec_open2(decCtx, decoder, nullptr) < 0) {
    throw std::runtime_error("Failed to open codec.");
  }

  // av_dump_format(formatCtx, 0, file.c_str(), 0);

  AVSampleFormat sampleFormat = decCtx->sample_fmt;
  int sampleRate = decCtx->sample_rate;
  int channels = decCtx->ch_layout.nb_channels;

  AVPacket *packet = av_packet_alloc();
  while (av_read_frame(formatCtx, packet) >= 0) {
    if (packet->stream_index == audioStreamIndex)
      ret = decode_packet(decCtx, packet);
    av_packet_unref(packet);
    if (ret < 0)
      break;
  }

  if (decCtx)
    decode_packet(decCtx, NULL);

  // 清理资源
  avcodec_free_context(&decCtx);

  std::string data = streamData.str();
  streamData.str("");
  return data;
}

bool AudioPlayer::CreateSource(const std::string &file, std::string name) {
  ALuint buffer;
  alGenBuffers(1, &buffer);
  if (alGetError() != AL_NO_ERROR) {
    std::cout << "error::openal: buffer generate failed! " << std::endl;
    return false;
  }
  ALuint source;
  alGenSources(1, &source);
  if (alGetError() != AL_NO_ERROR) {
    std::cout << "error::openal: source generate failed! " << std::endl;
    return false;
  }

  std::string data = loadPCMFromFile(file);
  alBufferData(buffer, AL_FORMAT_STEREO16, data.c_str(), data.size(), 44100);
  if (alGetError() != AL_NO_ERROR) {
    std::cout << "error::openal: load data to buffer failed! " << std::endl;
    return false;
  }

  alSourcei(source, AL_BUFFER, buffer);
  this->Sources[name] = source;
  return true;
}

AudioPlayer::AudioPlayer() { this->Init(); }
void AudioPlayer::Init() {

  ALCdevice *device = alcOpenDevice(NULL);
  if (!device) {
    std::cout << "error::openal: device open failed!" << std::endl;
    return;
  }

  ALCcontext *context = alcCreateContext(device, NULL);
  if (!context) {
    std::cout << "error::openal: context create failed!" << std::endl;
    return;
  }

  if (!alcMakeContextCurrent(context)) {
    std::cout << "error::openal: set current context failed!" << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return;
  }
}

bool AudioPlayer::Play(const std::string &name, bool loop) {
  auto source = this->Sources[name];
  if (!source)
    return false;

  alSourcePlay(source);
  if (loop)
    alSourcei(source, AL_LOOPING, AL_TRUE);

  if (alGetError() != AL_NO_ERROR) {
    std::cout << "error::openal: source play failed!" << std::endl;
    return false;
  }
  return true;
}

bool AudioPlayer::Display(const std::string &name) {
  auto source = this->Sources[name];
  if (!source)
    return false;
  ALint state;
  alGetSourcei(source, AL_SOURCE_STATE, &state);

  if (alGetError() != AL_NO_ERROR) {
    std::cout << "error::openal: get source state failed!" << std::endl;
    return false;
  }

  // 如果声源正在播放或暂停，则停止它
  if (state == AL_PLAYING || state == AL_PAUSED) {
    alSourceStop(source);
    if (alGetError() != AL_NO_ERROR) {
      std::cout << "error::openal: stop source play failed!" << std::endl;
      return false;
    }
  }
  return true;
}
