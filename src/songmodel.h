#ifndef SONGMODEL_H
#define SONGMODEL_H

#include <memory>
#include <utility>

#include <src/QMidi/src/QMidiFile.h>

class SongModel
{
public:
    SongModel(std::shared_ptr<QMidiFile> file);

    uint32_t getEndPos();
    float getTimeForTick(qint32 tick);
    std::pair<uint8_t, uint8_t> getNotesRange();
    quint32 getTickForTime(float time);

    //TEMP
    inline QMidiFile* getMidiFile() {return m_midiFile.get();}

private:
    std::shared_ptr<QMidiFile> m_midiFile;
};

#endif // SONGMODEL_H
