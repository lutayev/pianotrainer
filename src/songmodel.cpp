#include <QDebug>

#include "songmodel.h"

SongModel::SongModel(std::shared_ptr<QMidiFile> file)
{
    m_midiFile = file;
}

quint32 SongModel::getEndPos()
{
    quint32 max {0};
    for (auto it : m_midiFile->tracks()) {
        quint32 end = m_midiFile->trackEndTick(it);
        if (end > max) {
            max = end;
        }
    }
    return max;
}

float SongModel::getTimeForTick(qint32 tick)
{
    return m_midiFile->timeFromTick(tick);
}

std::pair<uint8_t, uint8_t> SongModel::getNotesRange()
{
    uint8_t min {128};
    uint8_t max {0};

    QList<QMidiEvent*> list = m_midiFile->events();
    for (int i = 0; i < list.size(); ++i) {
        int note = list.at(i)->note();
        if (note > 0 && note < 128) {
            if (note > max)
                max = note;
            if (note < min)
                min = note;
        }
    }
    return std::make_pair(min, max);
}

uint32_t SongModel::getTickForTime(float time)
{
    qint32 locTime = m_midiFile->tickFromTime(time);
    if (locTime < 0)
        locTime = 0;
    return locTime;
}
