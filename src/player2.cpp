#pragma once

#include "pch.h"

#include <QAudioBuffer>
#include <QAudioDeviceInfo>
#include <QBuffer>
#include <QFile>

#include "player2.h"

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096


const char* averr(int ret) {
    char* gay;
    av_make_error_string(gay, 256, ret);
    return gay;
}

Player2::Player2():
    m_format()
{
    m_dataframe = new QByteArray();
    m_readBuffer = new QBuffer(m_dataframe, this);
    m_readBuffer->open(QIODevice::ReadOnly);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

    setOpenMode(QIODevice::ReadOnly);
    // Set up the format, eg.

    //m_format = info.preferredFormat(); // Apparently, you need QApplicaion for this call

    m_format.setSampleRate(44100);
    m_format.setChannelCount(2);
    m_format.setCodec("audio/pcm");
    m_format.setSampleType(QAudioFormat::Float);
    m_format.setSampleSize(16);
    m_format.setByteOrder(QAudioFormat::LittleEndian);

//    This stream has  2  channels and a sample rate of  44100 Hz
//    The data is in the format  fltp
//    Audio frame info:
//       Sample count:  47
//       Channel count:  2
//       Format:  fltp
//       Bytes per sample:  4
//       Is planar?  1

    qDebug() << "m_format" << m_format;

    if (!info.isFormatSupported(m_format)) {
        throw std::exception("Raw audio format not supported by backend, cannot play audio.");
    }

    m_audio = new QAudioOutput(m_format, this);
    m_audio->setNotifyInterval(512); // TODO: play with this number
    m_audio->setVolume(1.0f);
    connect(m_audio, &QAudioOutput::stateChanged, this, &Player2::handleStateChanged);
    connect(m_audio, &QAudioOutput::notify, this, &Player2::notifySlot);
    start();
    m_audio->start(this);
}

void Player2::setFile(QString filename)
{
    if(!QFile(filename).exists()) return; // TODO: handlebetter
        m_filename = filename;
}

void Player2::handleStateChanged(QAudio::State state)
{
    switch(state) {
        case QAudio::ActiveState:
            qDebug() << "QAudio::ActiveState";
            break;
        case QAudio::SuspendedState:
            qDebug() << "QAudio::SuspendedState";
            break;
        case QAudio::StoppedState:
            qDebug() << "QAudio::StoppedState";
            emit finished();
            break;
        case QAudio::IdleState:
            qDebug() << "QAudio::IdleState";
            break;
        case QAudio::InterruptedState:
            qDebug() << "QAudio::InterruptedState";
            break;
    }
}

void Player2::notifySlot()
{
    if(m_audio->bytesFree() > 20000) {
        // encode some more frames to m_dataframe

    }
    qDebug() << "m_dataframe->size()" << m_dataframe->size();
    qDebug() << "m_readBuffer->pos()" << m_readBuffer->pos();
    qDebug() << "m_audio->processedUSecs()" << m_audio->processedUSecs() / 1000 / 1000;
    qDebug() << "m_audio->periodSize()" << m_audio->periodSize();
}

qint64 Player2::readData(char *data, qint64 maxlen)
{
    if(maxlen <= 0) return 0;
    int readlen = qMin((int)maxlen, (int)m_readBuffer->bytesAvailable()); // TODO: fix cast\
    // TODO IMPLEMENT MIN
    // Write zero to data
    memset(data, 0, maxlen);
    //QBuffer buff(m_dataframe);
    //buff.open(QIODevice::ReadOnly);
    //buff.read(data, readlen);
    m_readBuffer->read(data, readlen);
    //qDebug() << "m_readBuffer->pos()" << m_readBuffer->pos();
    //m_dataframe->remove(0, readlen);
    return maxlen;
}

qint64 Player2::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    throw std::logic_error("Deleted funcion");
    return 0;
}

void Player2::setPosition(qint64 pos)
{

}

void Player2::start()
{
    AVFrame* frame = av_frame_alloc();
    if (!frame) throw std::exception("Error allocating the frame");

    AVFrame* output = av_frame_alloc();
    if (!output) throw std::exception("Error allocating the frame");
    output->format = AV_SAMPLE_FMT_S16;
    output->channel_layout = AV_CH_LAYOUT_STEREO;
    output->sample_rate = 44100;

    av_frame_set_channels(output, 2);
    av_frame_set_channel_layout(output, 3);
    av_frame_copy_props(output, frame);

    AVFormatContext* formatContext = NULL;
    qDebug() << "m_filename.toLatin1()" << m_filename.toStdString().c_str();
    if (avformat_open_input(&formatContext, "E:/Music/Pump It Up   Danzel.mp3", NULL, NULL) != 0) {
        av_free(frame);
        throw std::exception("Error opening the file");
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        throw std::exception("Error finding the stream info" );
    }


    AVCodec* cdc = nullptr;
    int streamIndex = av_find_best_stream(formatContext, AVMEDIA_TYPE_AUDIO, -1, -1, &cdc, 0);
    if (streamIndex < 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        throw std::exception("Could not find any audio stream in the file");
    }

    AVStream* audioStream = formatContext->streams[streamIndex];
    AVCodecContext* codecContext = audioStream->codec;
    codecContext->codec = cdc;

    SwrContext *swr = swr_alloc();

    swr = swr_alloc();
    av_opt_set_int(swr, "in_channel_layout",  AV_CH_LAYOUT_STEREO, 0);
    av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO,  0);
    av_opt_set_int(swr, "in_sample_rate", 44100, 0);
    av_opt_set_int(swr, "out_sample_rate", 44100, 0);
    av_opt_set_sample_fmt(swr, "in_sample_fmt",  AV_SAMPLE_FMT_FLTP, 0);
    av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
    swr_init(swr);

//  This is just ignored
//    codecContext->request_sample_fmt = AV_SAMPLE_FMT_S16;
//    codecContext->sample_fmt = AV_SAMPLE_FMT_S16;

    if (avcodec_open2(codecContext, codecContext->codec, NULL) != 0)
    {
        av_free(frame);
        avformat_close_input(&formatContext);
        throw std::exception("Couldn't open the context with the decoder");
    }

    qDebug() << "This stream has " << codecContext->channels << " channels and a sample rate of " << codecContext->sample_rate << "Hz";
    qDebug() << "The data is in the format " << av_get_sample_fmt_name(codecContext->sample_fmt);

    AVPacket readingPacket;
    av_init_packet(&readingPacket);

    // Read the packets in a loop
    while (av_read_frame(formatContext, &readingPacket) == 0)
    {
        decode(codecContext, &readingPacket, frame, output, audioStream, swr);
    }
    qDebug() << "decoding finished";
    qDebug() << "m_dataframe->size()" << m_dataframe->size();
}

void Player2::decode(AVCodecContext* codecContext, AVPacket* readingPacket, AVFrame* frame, AVFrame* output, AVStream* audioStream, SwrContext *swr)
{
    if (readingPacket->stream_index == audioStream->index)
    {
        AVPacket decodingPacket = *readingPacket;

        // Try to decode the packet into a frame
        // Some frames rely on multiple packets, so we have to make sure the frame is finished before
        // we can use it
        int ret = avcodec_send_packet(codecContext, &decodingPacket);
        if (ret < 0) {
            throw std::exception("Couldn't send packet data");
        }
        // Audio packets can have multiple audio frames in a single packet
        while (ret >= 0) {
            ret = avcodec_receive_frame(codecContext, frame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return;
            else if (ret < 0) {
                throw std::exception("Error during decoding");
            }
            printAudioFrameInfo(codecContext, frame, output, swr);
        }
    }


    // You *must* call av_free_packet() after each call to av_read_frame() or else you'll leak memory
    av_free_packet(readingPacket);
}

void Player2::printAudioFrameInfo(const AVCodecContext* codecContext, const AVFrame* frame, AVFrame* output, SwrContext *swr)
{

    //Input and output AVFrames must have channel_layout, sample_rate and format set.

    // See the following to know what data type (unsigned char, short, float, etc) to use to access the audio data:
    // http://ffmpeg.org/doxygen/trunk/samplefmt_8h.html#af9a51ca15301871723577c730b5865c5
//    qDebug() << "Audio frame info:\n"
//        << "  Sample count: " << frame->nb_samples << '\n'
//        << "  Channel count: " << codecContext->channels << '\n'
//        << "  Format: " << av_get_sample_fmt_name(codecContext->sample_fmt) << '\n'
//        << "  Bytes per sample: " << av_get_bytes_per_sample(codecContext->sample_fmt) << '\n'
//        << "  Is planar? " << av_sample_fmt_is_planar(codecContext->sample_fmt) << '\n';


//    qDebug() << "frame->linesize[0] tells you the size (in bytes) of each plane\n";

    if (codecContext->channels > AV_NUM_DATA_POINTERS && av_sample_fmt_is_planar(codecContext->sample_fmt))
    {
//        qDebug() << "The audio stream (and its frames) have too many channels to fit in\n"
//            << "frame->data. Therefore, to access the audio data, you need to use\n"
//            << "frame->extended_data to access the audio data. It's planar, so\n"
//            << "each channel is in a different element. That is:\n"
//            << "  frame->extended_data[0] has the data for channel 1\n"
//            << "  frame->extended_data[1] has the data for channel 2\n"
//            << "  etc.\n";
    }
    else
    {
        m_dataframe->append((const char*)frame->data, frame->linesize[0]);

//        qDebug() << "frame->linesize[0]" << frame->linesize[0];
//        qDebug() << "frame->nb_samples * frame->channels * 2" << frame->nb_samples * frame->channels * 2;

//        int outputBufferLen = frame->nb_samples * frame->channels * 2;
//        uint8_t* outputBuffer = new uint8_t[outputBufferLen];
//        swr_convert(swr, &outputBuffer, frame->nb_samples, (const uint8_t **)frame->extended_data, frame->nb_samples);
//        m_dataframe->append(*outputBuffer, outputBufferLen);

        //swr_config_frame(swr, output, frame);

        //set avframe format
//        int ret = swr_convert_frame(swr, output, frame);
//        if(ret != 0) {
//            qDebug() << averr(ret);
//            throw std::exception("Error during swr_convert_frame");
//        }
//        qDebug() << "frame->linesize[0]: " << frame->linesize[0];
//        qDebug() << "output->linesize[0]: " << output->linesize[0];
//        m_dataframe->append((const char*)output->data, output->linesize[0]);
//        while((ret = swr_convert_frame(swr, output, NULL)) == 0)
//        {
//            qDebug() << "swr_get_delay(swr, 44100)" << swr_get_delay(swr, 44100);
//            qDebug() << "additional output->linesize[0]: " << output->linesize[0];
//            m_dataframe->append((const char*)output->data, output->linesize[0]);
//        }
//        qDebug() << "Either the audio data is not planar, or there is enough room in\n"
//            << "frame->data to store all the channels, so you can either use\n"
//            << "frame->data or frame->extended_data to access the audio data (they\n"
//            << "should just point to the same data).\n";
    }

//    qDebug() << "If the frame is planar, each channel is in a different element.\n"
//        << "That is:\n"
//        << "  frame->data[0]/frame->extended_data[0] has the data for channel 1\n"
//        << "  frame->data[1]/frame->extended_data[1] has the data for channel 2\n"
//        << "  etc.\n";

//    qDebug() << "If the frame is packed (not planar), then all the data is in\n"
//        << "frame->data[0]/frame->extended_data[0] (kind of like how some\n"
//        << "image formats have RGB pixels packed together, rather than storing\n"
//        << " the red, green, and blue channels separately in different arrays.\n";
}


