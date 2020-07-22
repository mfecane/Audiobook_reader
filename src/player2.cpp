#include <QAudioBuffer>
#include <QAudioDeviceInfo>
#include <QFile>

#include "player2.h"

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

Player2::Player2() {

}

void Player2::start(QString filename)
{
//    const char *outfilename, *filename;
     const AVCodec *codec;
     AVCodecContext *c= NULL;
     AVCodecParserContext *parser = NULL;
     int len, ret;
     FILE *f, *outfile;
     uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
     uint8_t *data;
     size_t   data_size;
     AVPacket *pkt;
     AVFrame *decoded_frame = NULL;
     enum AVSampleFormat sfmt;
     int n_channels = 0;
     const char *fmt;

     QFile file(filename);

//     if (argc <= 2) {
//         fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
//         exit(0);
//     }
//     filename    = argv[1];
//     outfilename = argv[2];

     pkt = av_packet_alloc();

     /* find the MPEG audio decoder */
     codec = avcodec_find_decoder(AV_CODEC_ID_MP2);
     if (!codec) {
         fprintf(stderr, "Codec not found\n");
         exit(1);
     }

     parser = av_parser_init(codec->id);
     if (!parser) {
         fprintf(stderr, "Parser not found\n");
         exit(1);
     }

     c = avcodec_alloc_context3(codec);
     if (!c) {
         fprintf(stderr, "Could not allocate audio codec context\n");
         exit(1);
     }

     /* open it */
     if (avcodec_open2(c, codec, NULL) < 0) {
         fprintf(stderr, "Could not open codec\n");
         exit(1);
     }

     if (!file.open(QIODevice::ReadOnly)) {
         qDebug() << "Could not open " << filename;
         exit(1);
     }

//     outfile = fopen(outfilename, "wb");
//     if (!outfile) {
//         av_free(c);
//         exit(1);
//     }

     // TODO: open audio device;

     /* decode until eof */
     data      = inbuf;
     //data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
     data_size = file.read((char*)inbuf, AUDIO_INBUF_SIZE);

     while (data_size > 0) {
         if (!decoded_frame) {
             if (!(decoded_frame = av_frame_alloc())) {
                 fprintf(stderr, "Could not allocate audio frame\n");
                 exit(1);
             }
         }

         ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size,
                                data, data_size,
                                AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
         if (ret < 0) {
             fprintf(stderr, "Error while parsing\n");
             exit(1);
         }
         data      += ret;
         data_size -= ret;

         if (pkt->size)
             decode(c, pkt, decoded_frame, outfile);

         if (data_size < AUDIO_REFILL_THRESH) {
             memmove(inbuf, data, data_size);
             data = inbuf;
             len = fread(data + data_size, 1,
                         AUDIO_INBUF_SIZE - data_size, f);
             if (len > 0)
                 data_size += len;
         }
     }

     /* flush the decoder */
     pkt->data = NULL;
     pkt->size = 0;
     decode(c, pkt, decoded_frame, outfile);

     /* print output pcm infomations, because there have no metadata of pcm */
     sfmt = c->sample_fmt;

     if (av_sample_fmt_is_planar(sfmt)) {
         const char *packed = av_get_sample_fmt_name(sfmt);
         printf("Warning: the sample format the decoder produced is planar "
                "(%s). This example will output the first channel only.\n",
                packed ? packed : "?");
         sfmt = av_get_packed_sample_fmt(sfmt);
     }

     n_channels = c->channels;
     if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0)
         goto end;

//     printf("Play the output audio file with the command:\n"
//            "ffplay -f %s -ac %d -ar %d %s\n",
//            fmt, n_channels, c->sample_rate,
//            outfilename);
 end:
     //fclose(outfile);
     //fclose(f);

     file.close();

     avcodec_free_context(&c);
     av_parser_close(parser);
     av_frame_free(&decoded_frame);
     av_packet_free(&pkt);
}

void Player2::decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame,
                     FILE *outfile){
    int i, ch;
    int ret, data_size;

    /* send the packet with the compressed data to the decoder */
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        fprintf(stderr, "Error submitting the packet to the decoder\n");
        exit(1);
    }

    /* read all the output frames (in general there may be any number of them */
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        if (data_size < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }
        for (i = 0; i < frame->nb_samples; i++)
            for (ch = 0; ch < dec_ctx->channels; ch++)
                fwrite(frame->data[ch] + data_size*i, 1, data_size, outfile);
    }
}

int Player2::get_format_from_sample_fmt(const char **fmt, enum AVSampleFormat sample_fmt)
{
    int i;
    struct sample_fmt_entry {
        enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
    } sample_fmt_entries[] = {
        { AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
        { AV_SAMPLE_FMT_S16, "s16be", "s16le" },
        { AV_SAMPLE_FMT_S32, "s32be", "s32le" },
        { AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
        { AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
    };
    *fmt = NULL;

    for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
        struct sample_fmt_entry *entry = &sample_fmt_entries[i];
        if (sample_fmt == entry->sample_fmt) {
            *fmt = AV_NE(entry->fmt_be, entry->fmt_le);
            return 0;
        }
    }

    fprintf(stderr,
            "sample format %s is not supported as output format\n",
            av_get_sample_fmt_name(sample_fmt));
    return -1;
}














void Player2::setFile(QString filename, qint64 pos)
{
    if(!QFile(filename).exists()) return; // TODO: handlebetter
    m_filename = filename;
    m_request_pos = pos;
}

void Player2::setPosition(qint64 pos)
{
}

void Player2::bufferReadySlot()
{
}
