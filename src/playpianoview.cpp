#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <iostream>

#include "playpianoview.h"

PlayPianoView::PlayPianoView(const QString& name, QWidget *parent, bool async)
    : AbstractPlayView(name, parent, async)
{
    m_widget->setLayout(new QVBoxLayout);
    m_keyboard = new Keyboard(m_widget);
    m_lblLength = new QLabel;
    m_widget->layout()->addWidget(m_lblLength);
    m_widget->layout()->addWidget(m_keyboard);

}

PlayPianoView::~PlayPianoView()
{
    delete m_keyboard;
}

void PlayPianoView::dispatchPlay()
{
    m_keyboard->setRange(m_song->getNotesRange());
}

void PlayPianoView::dispatchPause()
{

}

void PlayPianoView::dispatchStop()
{
    m_lblLength->setText(QString::number(m_song->getEndPos()));
    m_keyboard->setRange(m_song->getNotesRange());
}

void PlayPianoView::dispatchPlayPos(qint32 pos)
{

}

void PlayPianoView::dispatchJmpPos(qint32 pos)
{
    m_keyboard->reset();
}

void PlayPianoView::dispatchTempo(float tempo)
{

}

void PlayPianoView::onEvent(QMidiEvent *event, float delay)
{
    if (delay < 0.01) {
        if (event->type() == QMidiEvent::NoteOn) {
            m_keyboard->press(event->note(), Keyboard::Pressed);
        } else if (event->type() == QMidiEvent::NoteOff) {
            m_keyboard->press(event->note(), Keyboard::Released);
        }
    } else {
        if (event->type() == QMidiEvent::Meta || event->type() == QMidiEvent::SysEx || m_keyboard->isPressed(event->note())) {
            return;
        }
        int note = event->note();
        if (!(note < 0 || note > 128)) {
            if (event->type() == QMidiEvent::NoteOn) {
                if (delay > 0.4 && delay <= 0.5) {
                    m_keyboard->press(note, Keyboard::Pending5);
                } else if (delay > 0.3 && delay <= 0.4) {
                    m_keyboard->press(note, Keyboard::Pending4);
                } else if (delay > 0.2 && delay <= 0.3) {
                    m_keyboard->press(note, Keyboard::Pending3);
                } else if (delay > 0.1 && delay <= 0.2) {
                    m_keyboard->press(note, Keyboard::Pending2);
                } else if (delay > 0.0 && delay <= 0.1) {
                    m_keyboard->press(note, Keyboard::Pending1);
                }
            }
        }
    }
}

void PlayPianoView::reset()
{

}
