#include <cstddef>
#include <cstdint>
extern "C" {
#include "libavutil/channel_layout.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
}
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

std::stringstream streamData;
AVPacket *packet = av_packet_alloc();

AVFrame *frame = av_frame_alloc();

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

  // out_frame->nb_samples = av_rescale_rnd(
  //     swr_get_delay(swr_ctx, frame->sample_rate) + frame->nb_samples,
  //     out_frame->sample_rate, frame->sample_rate, AV_ROUND_UP);

  dst_nb_samples = av_rescale_rnd(frame->nb_samples, dst_rate,
                                  frame->sample_rate, AV_ROUND_UP);

  /* buffer is going to be directly written to a rawaudio file, no alignment */
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

  return ret;
}

struct AudioMesg {
  std::string Data;
  int DataSize = 0;
  int Channels = 0;
  int SampleRate = 0;
  AVSampleFormat SampleFormat;
  AudioMesg(std::string data, int dataSize, int channels, int sampleRate,
            AVSampleFormat sampleFormat)
      : Data(data), DataSize(dataSize), Channels(channels),
        SampleRate(sampleRate), SampleFormat(sampleFormat) {}
  void Print() {
    std::cout << "DataSize: " << DataSize << " <<< Channels: " << Channels
              << " <<< SampleRate: " << SampleRate
              << " <<< SampleFormat: " << SampleFormat << std::endl;
  }
};

// 解析并加载 WAV 文件
AudioMesg *loadWAVFile(const std::string &file) {

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

  av_dump_format(formatCtx, 0, file.c_str(), 0);

  AVSampleFormat sampleFormat = decCtx->sample_fmt;
  int sampleRate = decCtx->sample_rate;
  int channels = decCtx->ch_layout.nb_channels;

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
  av_frame_free(&frame);

  // 返回样本数据、样本大小和采样率
  return new AudioMesg(streamData.str(), streamData.str().length(), channels,
                       sampleRate, sampleFormat);
}

int main() {
  // Initialize OpenAL
  ALCdevice *device = alcOpenDevice(NULL);
  if (!device) {
    std::cerr << "Failed to open default device." << std::endl;
    return 1;
  }

  ALCcontext *context = alcCreateContext(device, NULL);
  if (!context) {
    std::cerr << "Failed to create context." << std::endl;
    alcCloseDevice(device);
    return 1;
  }

  if (!alcMakeContextCurrent(context)) {
    std::cerr << "Failed to make context current." << std::endl;
    alcDestroyContext(context);
    alcCloseDevice(device);
    return 1;
  }

  // Create a buffer and a source
  ALuint buffer;
  alGenBuffers(1, &buffer);
  if (alGetError() != AL_NO_ERROR) {
    std::cerr << "Failed to generate buffer." << std::endl;
    return 1;
  }

  ALuint source;
  alGenSources(1, &source);
  if (alGetError() != AL_NO_ERROR) {
    std::cerr << "Failed to generate source." << std::endl;
    return 1;
  }

  auto bleepAudioMesg = loadWAVFile("./audio/bleep.mp3");
  bleepAudioMesg->Print();

  alBufferData(buffer, AL_FORMAT_STEREO16,
               (ALvoid *)bleepAudioMesg->Data.c_str(), bleepAudioMesg->DataSize,
               bleepAudioMesg->SampleRate);
  int error = alGetError();
  if (error != AL_NO_ERROR) {
    std::cerr << "Failed to load data into buffer. error: " << error
              << std::endl;
    return 1;
  }

  // Play the sound
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);
  if (alGetError() != AL_NO_ERROR) {
    std::cerr << "Failed to play source." << std::endl;
    return 1;
  }

  // Wait for the sound to finish
  ALint state;
  do {
    alGetSourcei(source, AL_SOURCE_STATE, &state);
  } while (state == AL_PLAYING);

  // std::cout << state << std::endl;
  if (alGetError() != AL_NO_ERROR) {
    std::cerr << "Failed to get source state." << std::endl;
    return 1;
  }

  // Cleanup
  // alDeleteBuffers(1, &buffer);
  // alDeleteSources(1, &source);
  // if (alGetError() != AL_NO_ERROR) {
  //   std::cerr << "Failed to delete buffers and sources." << std::endl;
  //   return 1;
  // }

  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);

  return 0;
}
