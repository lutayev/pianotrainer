#include <QDebug>
#include <QLayout>
#include <QObject>


#include "abstractplayview.h"

AbstractPlayView::AbstractPlayView(const QString& name, QWidget* parent) : QWidget(parent), m_ModelName(name)
{
    m_lazyDispatchPlay = false;
    m_lazyDispatchPause = false;
    m_lazyDispatchStop = false;
    m_lazyDispatchPlayPos = true;
    m_lazyDispatchJmpPos = false;
    m_lazyDispatchTempo = false;

    QObject::connect(this, &AbstractPlayView::sigInnerPlay, this, &AbstractPlayView::innerOnPlay, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerPause, this, &AbstractPlayView::innerOnPause, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerStop, this, &AbstractPlayView::innerOnStop, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerPlayPos, this, &AbstractPlayView::innerOnPlayPos, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerJmpPos, this, &AbstractPlayView::innerOnJmpPos, Qt::ConnectionType::QueuedConnection);
    QObject::connect(this, &AbstractPlayView::sigInnerTempo, this, &AbstractPlayView::innerOnTempo, Qt::ConnectionType::QueuedConnection);
}

QString AbstractPlayView::getModelName() const noexcept
{
    return m_ModelName;
}

void AbstractPlayView::setSong(std::shared_ptr<SongModel> song)
{
    m_song = song;
    m_midiFile = m_song->getMidiFile();
    m_events = m_midiFile->events();
}

void AbstractPlayView::attach(QWidget* parent)
{
    this->setParent(parent);
    if (!parent) {
        this->moveToThread(&m_thread);
    } else {
        parent->layout()->addWidget(this);
        this->moveToThread(parent->thread());
    }

    this->show();
}
