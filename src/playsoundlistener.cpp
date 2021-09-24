#include "playsoundlistener.h"

PlaySoundListener::PlaySoundListener(const QString& name, QWidget* parent, bool async)
    : AbstractPlayView(name, parent, async)
{

}

PlaySoundListener::~PlaySoundListener()
{
    m_midiOut.disconnect();
}

void PlaySoundListener::dispatchPlay()
{
    if (!m_midiOut.isConnected())
        m_midiOut.connect("0");
}

void PlaySoundListener::dispatchPause()
{

}

void PlaySoundListener::dispatchStop()
{
    m_midiOut.disconnect();
}

void PlaySoundListener::dispatchPlayPos(qint32 pos)
{

}

void PlaySoundListener::dispatchJmpPos(qint32 pos)
{
    m_midiOut.stopAll();
}

void PlaySoundListener::dispatchTempo(float tempo)
{

}

void PlaySoundListener::onEvent(QMidiEvent *event, float delay)
{
    if (delay == 0.0)
        m_midiOut.sendEvent(*event);
}
