#include <QDebug>
#include <QLayout>
#include <QObject>


#include "abstractplayview.h"

AbstractPlayView::AbstractPlayView(const QString& name, QWidget* parent, bool async) : m_ModelName(name)
{
    m_widget = new QWidget(parent);
    m_thread = new QThread();
    if (async) {
        this->moveToThread(m_thread);
        m_thread->start();
    } else if (parent) {
        this->setParent(parent);
        this->moveToThread(parent->thread());
    }

    m_lazyDispatchPlay = false;
    m_lazyDispatchPause = false;
    m_lazyDispatchStop = false;
    m_lazyDispatchPlayPos = false;
    m_lazyDispatchJmpPos = false;
    m_lazyDispatchTempo = false;

    QObject::connect(this, &AbstractPlayView::sigInnerPlay, this, &AbstractPlayView::innerOnPlay, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerPause, this, &AbstractPlayView::innerOnPause, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerStop, this, &AbstractPlayView::innerOnStop, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerPlayPos, this, &AbstractPlayView::innerOnPlayPos, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerJmpPos, this, &AbstractPlayView::innerOnJmpPos, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerTempo, this, &AbstractPlayView::innerOnTempo, Qt::ConnectionType::QueuedConnection);
}

AbstractPlayView::~AbstractPlayView()
{
    delete m_widget;
    delete m_thread;
}

QString AbstractPlayView::getModelName() const noexcept
{
    return m_ModelName;
}

QWidget *AbstractPlayView::getWidget() const noexcept
{
    return m_widget;
}

void AbstractPlayView::setSong(std::shared_ptr<SongModel> song)
{
    m_song = song;
    m_midiFile = m_song->getMidiFile();
    m_events = m_midiFile->events();
}

void AbstractPlayView::attach(QWidget* parent)
{
    m_widget->setParent(parent);
    if (parent) {
        parent->layout()->addWidget(m_widget);
    } else {
    }
    m_widget->show();
}

void AbstractPlayView::easyParseEvents(quint32 pos)
{
    qint32 lastIndexTick = 0;
    int futureIndex = 0;
    while (m_lastIndex < m_events.size() && futureIndex < m_events.size()) {
        QMidiEvent* event = m_events.at(m_lastIndex);
        lastIndexTick = event->tick();

        if (lastIndexTick <= pos) {
            //Current and passed unprocessed events
            if (event->type() == QMidiEvent::Meta || event->type() == QMidiEvent::SysEx) {
                ++m_lastIndex;
                continue;
            }
            ++m_lastIndex;
            int note = event->note();
            if (!(note < 0 || note > 128)) {
                onEvent(event, 0.0);
            }
        } else {
            //Near future coming events
            float endTime = m_song->getTimeForTick(pos) + 0.5;
            for (int i = 1; i < m_events.size(); ++i) {
                futureIndex = m_lastIndex + i;
                if (futureIndex < m_events.size()) {
                    QMidiEvent* futureEvent = m_events.at(futureIndex);
                    float time = m_song->getTimeForTick(futureEvent->tick());
                    if (time < endTime) {
                        onEvent(futureEvent, endTime - time);
                    } else {
                        return;
                    }
                }
            }
        }
    }
}
