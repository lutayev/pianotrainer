#ifndef ABSTRACTPLAYVIEW_H
#define ABSTRACTPLAYVIEW_H

#include <QThread>
#include <QString>
#include <QWidget>
#include <src/QMidi/src/QMidiFile.h>
#include <src/QMidi/src/QMidiIn.h>
#include <src/QMidi/src/QMidiOut.h>

#include <QDebug>

#include <atomic>
#include <memory>
#include <mutex>

#include "songmodel.h"


class AbstractPlayView : public QObject
{
    Q_OBJECT
public:
    //Async flag for non-GUI listeners, run in separate thread
    AbstractPlayView(const QString& name = "default", QWidget* parent = 0, bool async = false);
    virtual ~AbstractPlayView();

    QString     getModelName() const noexcept;
    QWidget*    getWidget() const noexcept;
    void        setSong(std::shared_ptr<SongModel> song);
    void        attach(QWidget* parent);

public slots:
    //Virtual slots are 10 times slower, so implement usual slots and signals dispatchers, with handling "dispatcher busy" situation
    //Connected with controller, emit one message and skip others untill dispatcher is busy
    //We can't manipulate GUI from another thread, so use additional ("inner") layer of signal-slot connections
    inline void onPlay()
    {
        if (!m_lazyDispatchPlay) {
            dispatchPlay();
            return;
        }

        if (m_mtxPlay.try_lock()) {
            emit sigInnerPlay();
        }
    }
    inline void onPause()
    {
        if (!m_lazyDispatchPause) {
            dispatchPause();
            return;
        }

        if (m_mtxPause.try_lock()) {
            emit sigInnerPause();
        }
    }
    inline void onStop()
    {
        m_lastEventIndex = 0;
        m_cachedPosition = 0;
        m_lastIndex = 0;

        if (!m_lazyDispatchStop) {
            dispatchStop();
            return;
        }

        if (m_mtxStop.try_lock()) {
            emit sigInnerStop();
        }
    }
    inline void onPlayPos(qint32 pos)
    {
        easyParseEvents(pos);

        if (!m_lazyDispatchPlayPos) {
            dispatchPlayPos(pos);
            return;
        }

        if (m_mtxPlayPos.try_lock()) {
            emit sigInnerPlayPos(pos);
        }
    }
    inline void onJmpPos(qint32 pos)
    {
        for (qint32 i = 0; i < m_events.size(); ++i) {
            if (m_events.at(i)->tick() < pos) {
                continue;
            } else {
                m_lastIndex = i + 1;
                break;
            }
        }

        if (!m_lazyDispatchJmpPos) {
            dispatchJmpPos(pos);
            return;
        }

        if (m_mtxJmpPos.try_lock()) {
            emit sigInnerJmpPos(pos);
        }
    }
    inline void onTempo(float tempo)
    {
        if (!m_lazyDispatchTempo) {
            dispatchTempo(tempo);
            return;
        }

        if (m_mtxTempo.try_lock()) {
            emit sigInnerTempo(tempo);
        }
    }

protected:

    virtual void dispatchPlay() = 0;
    virtual void dispatchPause() = 0;
    virtual void dispatchStop() = 0;
    virtual void dispatchPlayPos(qint32 pos) = 0;
    virtual void dispatchJmpPos(qint32 pos) = 0;
    virtual void dispatchTempo(float tempo) = 0;

    //Utilitary functions to parse position and produce interface for handle events rather than raw position
    void easyParseEvents(quint32 pos);
    virtual void onEvent(QMidiEvent* event, float delay) = 0;

    //State
    std::shared_ptr<SongModel>      m_song;
    QList<QMidiEvent*>              m_events;
    std::atomic<qint32>             m_cachedPosition{0};
    QMidiFile*                      m_midiFile{nullptr};
    QWidget*                        m_widget {nullptr};
    QThread*                        m_thread {nullptr};
    qint32                          m_lastEventIndex{0};
    int                             m_lastIndex{0};

    //Strategy: skip or not incoming signals from controller when busy
    std::atomic<bool>               m_lazyDispatchPlay{true};
    std::atomic<bool>               m_lazyDispatchPause{true};
    std::atomic<bool>               m_lazyDispatchStop{true};
    std::atomic<bool>               m_lazyDispatchPlayPos{true};
    std::atomic<bool>               m_lazyDispatchJmpPos{true};
    std::atomic<bool>               m_lazyDispatchTempo{true};

private:

    //Inner slots for calling dispatcher and set "unbusy" flag
    inline void innerOnPlay()
    {
        dispatchPlay();
        m_mtxPlay.unlock();
    }
    inline void innerOnPause()
    {
        dispatchPause();
        m_mtxPause.unlock();
    }
    inline void innerOnStop()
    {
        dispatchStop();
        m_mtxStop.unlock();
    }
    inline void innerOnPlayPos(qint32 pos)
    {
        dispatchPlayPos(pos);
        m_mtxPlayPos.unlock();
    }
    inline void innerOnJmpPos(qint32 pos)
    {
        dispatchJmpPos(pos);
        m_mtxJmpPos.unlock();
    }
    inline void innerOnTempo(float tempo)
    {
        dispatchTempo(tempo);
        m_mtxTempo.unlock();
    }

    //Busy dispatchers flags
    std::mutex  m_mtxPlay;
    std::mutex  m_mtxPause;
    std::mutex  m_mtxStop;
    std::mutex  m_mtxPlayPos;
    std::mutex  m_mtxJmpPos;
    std::mutex  m_mtxTempo;

    QString     m_ModelName;

signals:
    //Signals for controller
    void sigPauseGenerated(bool paused);
    void sigStopGenerated();
    void sigPlayGenerated();
    void sigPosChanged(qint32 pos);
    void sigTempoChanged(float tempo);

    //Signals for inner message dispatch mechanism (skip when busy)
    void sigInnerPlay();
    void sigInnerPause();
    void sigInnerStop();
    void sigInnerPlayPos(qint32 pos);
    void sigInnerJmpPos(qint32 pos);
    void sigInnerTempo(float tempo);
};

#endif // ABSTRACTPLAYVIEW_H
