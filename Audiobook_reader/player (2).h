#include "audiobook.h"
#include "QMediaPlayer"
#include "QAudioOutput"
#include "QAudioDecoder"

class Player
{
public:
    Player();

private:

    void play();
    void pause();

    void next();
    void prev();

    void jumpForeward();
    void jumpBack();

private:

    QMediaPlayer m_player;
    AudioBook* m_audiobook;

    QAudioDecoder* m_dec;
    QAudioOutput* m_out;

};
