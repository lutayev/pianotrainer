#ifndef PLAYSOUNDLISTENER_H
#define PLAYSOUNDLISTENER_H

#include "abstractplayview.h"

class PlaySoundListener : public AbstractPlayView
{
public:
    PlaySoundListener(const QString& name = "Sound", QWidget* parent = 0, bool async = false);
    ~PlaySoundListener();

    // AbstractPlayView interface
protected:
    void dispatchPlay();
    void dispatchPause();
    void dispatchStop();
    void dispatchPlayPos(qint32 pos);
    void dispatchJmpPos(qint32 pos);
    void dispatchTempo(float tempo);
    void onEvent(QMidiEvent *event, float delay);

private:
    QMidiOut m_midiOut;
};

#endif // PLAYSOUNDLISTENER_H
