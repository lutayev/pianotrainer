#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <iostream>

#include "playpianoview.h"

PlayPianoView::PlayPianoView(const QString& name, QWidget *parent) : AbstractPlayView(name, parent)
{
    this->setLayout(new QVBoxLayout);
    m_keyboard = new Keyboard(this);
    m_lblLength = new QLabel;
    this->layout()->addWidget(m_lblLength);
    this->layout()->addWidget(m_keyboard);
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
    m_keyboard->reset();
}

void PlayPianoView::dispatchPlayPos(qint32 pos)
{
    qint32 lastIndexTick = 0;
    int futureIndex = 0;
    while (m_lastIndex < m_events.size() && futureIndex < m_events.size()) {
        QMidiEvent* event = m_events.at(m_lastIndex);
        lastIndexTick = event->tick();

        //qDebug() << lastIndexTick << pos;
        if (lastIndexTick <= pos) {
            if (event->type() == QMidiEvent::Meta || event->type() == QMidiEvent::SysEx) {
                ++m_lastIndex;
                continue;
            }
            ++m_lastIndex;
            int note = event->note();
            if (!(note < 0 || note > 128)) {
                if (event->type() == QMidiEvent::NoteOn) {
                    m_keyboard->press(event->note(), Keyboard::Pressed);
                } else if (event->type() == QMidiEvent::NoteOff) {
                    m_keyboard->press(event->note(), Keyboard::Released);
                }
            }
        } else {
            //qDebug() << "Future";
            float endTime = m_song->getTimeForTick(pos) + 0.5;
            for (int i = 1; i < m_events.size(); ++i) {
                futureIndex = m_lastIndex + i;
                if (futureIndex < m_events.size()) {
                    QMidiEvent* futureEvent = m_events.at(futureIndex);
                    float time = m_song->getTimeForTick(futureEvent->tick());
                    //qDebug() << time << endTime;
                    if (time < endTime) {
                        if (futureEvent->type() == QMidiEvent::Meta || futureEvent->type() == QMidiEvent::SysEx /*|| m_keyboard->isPressed(futureEvent->note())*/) {
                            continue;
                        }
                        int note = futureEvent->note();
                        //qDebug() << "Note: " << note;
                        if (!(note < 0 || note > 128)) {
                            if (futureEvent->type() == QMidiEvent::NoteOn) {
                                //qDebug() << "Color future!";
                                if (time < endTime - 0.5) {
                                    m_keyboard->press(note, Keyboard::Pending5);
                                } else if (time < endTime - 0.4) {
                                    m_keyboard->press(note, Keyboard::Pending4);
                                } else if (time < endTime - 0.3) {
                                    m_keyboard->press(note, Keyboard::Pending3);
                                } else if (time < endTime - 0.2) {
                                    m_keyboard->press(note, Keyboard::Pending2);
                                } else if (time < endTime - 0.1) {
                                    m_keyboard->press(note, Keyboard::Pending1);
                                }
                            }
                        }
                    } else {
                        return;
                    }
                }
            }
        }
    }
}

void PlayPianoView::dispatchJmpPos(qint32 pos)
{
    for (qint32 i = 0; i < m_events.size(); ++i) {
        if (m_events.at(i)->tick() < pos) {
            qDebug() << "AAARGH" << m_events.at(i)->tick() << pos;
            continue;
        } else {
            qDebug() << "Set last index" << i << m_lastIndex;
            m_lastIndex = i + 1;
            m_keyboard->reset();
            break;
        }
    }
}

void PlayPianoView::dispatchTempo(float tempo)
{

}

void PlayPianoView::reset()
{
//    QLayoutItem* item;

//    while (item = m_mainWgt->layout()->takeAt(0)) {
//        delete item;
//    }

//    std::pair<uint8_t, uint8_t> range;
//    range = m_song->getNotesRange();

//    for (int i = range.first; i <= range.second; ++i) {
//        QLabel* key = new QLabel(QString::number(i), m_mainWgt);
//        key->setStyleSheet(m_defaultStyle);
//        m_keys[i] = key;
//        m_mainWgt->layout()->addWidget(key);
//    }
}
