#ifndef PLAYSOUNDVIEW_H
#define PLAYSOUNDVIEW_H

#include <QSpinBox>
#include <QPushButton>
#include <QSlider>
#include "abstractplayview.h"

class PlaySoundView : public AbstractPlayView
{
public:
    PlaySoundView(const QString& name = "audioView", QWidget* parent = 0);
    virtual ~PlaySoundView();

protected:

    virtual void dispatchPlay();
    virtual void dispatchPause();
    virtual void dispatchStop();
    virtual void dispatchPlayPos(qint32 pos);
    virtual void dispatchJmpPos(qint32 pos);
    virtual void dispatchTempo(float tempo);

private:
    QMidiOut            m_midiOut;
    qint32              m_lastEventIndex{0};
    QSpinBox*           m_sbPosition;
    QPushButton*        m_btnStop;
    QPushButton*        m_btnPlay;
    QPushButton*        m_btnTempoReset;
    QSlider*            m_slTempo;
    QSlider*            m_slPosition;
    bool                m_pauseState{false};

};

#endif // PLAYSOUNDVIEW_H
