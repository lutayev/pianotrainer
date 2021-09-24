#include <QDebug>
#include <QElapsedTimer>
#include <QApplication>

#include "playcontroller.h"

PlayController::PlayController(std::shared_ptr<SongModel> songModel, std::shared_ptr<AbstractPlayView> view) :
    m_songModel(songModel)
{
    addView(view);
}

bool PlayController::start()
{
    qDebug() << "Controller start";
    //Start async play loop
    if (m_state == PlayState::Stopped) {
        m_position = 0;
        m_state = PlayState::ToBePlaying;
        std::thread playLoop(&PlayController::updateStateLoop, this);
        playLoop.detach();
        return true;
    }
    return false;
}

void PlayController::setPos(qint32 pos)
{
    if (pos < 0 || pos >= m_songModel->getEndPos()) {
        pos = m_songModel->getEndPos() - 3;
    }
    m_position = pos;
    emit sigJmpPos(pos);
}

void PlayController::setTempo(float tempo)
{
    m_tempo = tempo;
    emit sigTempo(m_tempo);
}

void PlayController::pause(bool pause)
{
    //If stopped and unpause signal, start play
    if (m_state == PlayState::Stopped) {
        if (pause)
            return;
        start();
        return;
    }

    //Play/pause
    if (pause) {
        m_state = PlayState::ToBePaused;
        while(m_state != PlayState::Paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } else {
        m_state = PlayState::ToBePlaying;
        while(m_state != PlayState::Playing ){//&& m_state != PlayState::Stopped) {
            //QApplication::processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

bool PlayController::stop()
{
    if (m_state == PlayState::Stopped)
        return false;

    m_state = PlayState::Stopped;
    return true;
}

void PlayController::updateStateLoop()
{
    emit sigTempo(m_tempo);
    emit sigJmpPos(0);
    qint32 startPos = 0;
    qint32 lastPos = 0;
    qint32 endPos = m_songModel->getEndPos();

    QElapsedTimer timer;
    float time = 0;
    float elapsedTime = 0;
    float startTime = 0;
    float timeShift = 0;
    while(m_state != PlayState::Stopped) {

        switch (m_state.load()) {

        case PlayState::ToBePlaying :
            qDebug() << "LOOP::ToBePlaying";
            m_state = PlayState::Playing;
            startPos = m_position;
            emit sigPlay();
            timer.restart();
            break;

        case PlayState::Playing :
            elapsedTime = ((float)timer.elapsed() / 1000) * m_tempo;
            startTime = (m_songModel->getTimeForTick(startPos));
            time = elapsedTime + startTime + timeShift;
            m_position = m_songModel->getTickForTime(time);

            qDebug() << m_position << lastPos;
            if (m_position != lastPos) {
//                qDebug() << "LOOP::Playing:\tElapsed time - " + QString::number(elapsedTime) +
//                            ", Start time - " + QString::number(startTime) +
//                            ", Total time - " + QString::number(time) +
//                            ", Position - " + QString::number(m_position) + "\n";
                lastPos = m_position;
                if (m_position > endPos || m_position < 0) {
                    qDebug() << "Stopping";
                    m_state = PlayState::Stopped;
                } else {
                    emit sigPlayPos(m_position);
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            break;

        case PlayState::ToBePaused :
            qDebug() << "LOOP::ToBePaused";
            m_state = PlayState::Paused;
            emit sigPause();
            break;

        case PlayState::Paused :
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            break;

        case PlayState::Stopped :
            qDebug() << "LOOP::Stopped, exit on next iteration";
            break;
        }
    }
    qDebug() << "Exit loop";
    m_position = 0;
    emit sigStop();
}

void PlayController::setSong(std::shared_ptr<SongModel> model)
{
    stop();
    m_songModel = model;

    for (auto it : m_views) {
        it.second->setSong(model);
        it.second->setEnabled(true);
    }
    emit sigStop();
}

void PlayController::attach(const QString& viewName, QWidget* parent)
{
    m_views.at(viewName.toStdString())->attach(parent);
}

void PlayController::addView(std::shared_ptr<AbstractPlayView> view)
{
    QString name = view->getModelName();
    if (m_views.find(name.toStdString()) == m_views.end())
        m_views.emplace(name.toStdString(), view);

    if (m_songModel.get()) {
        view->setSong(m_songModel);
    }

    QObject::connect(this, &PlayController::sigPlay, view.get(), &AbstractPlayView::onPlay, Qt::DirectConnection);
    QObject::connect(this, &PlayController::sigPause, view.get(), &AbstractPlayView::onPause, Qt::QueuedConnection);
    QObject::connect(this, &PlayController::sigStop, view.get(), &AbstractPlayView::onStop, Qt::QueuedConnection);
    QObject::connect(this, &PlayController::sigPlayPos, view.get(), &AbstractPlayView::onPlayPos, Qt::QueuedConnection);
    QObject::connect(this, &PlayController::sigJmpPos, view.get(), &AbstractPlayView::onJmpPos, Qt::QueuedConnection);
    QObject::connect(this, &PlayController::sigTempo, view.get(), &AbstractPlayView::onTempo, Qt::QueuedConnection);

    QObject::connect(view.get(), &AbstractPlayView::sigPlayGenerated, this, &PlayController::start, Qt::QueuedConnection);
    QObject::connect(view.get(), &AbstractPlayView::sigPauseGenerated, this, &PlayController::pause, Qt::QueuedConnection);
    QObject::connect(view.get(), &AbstractPlayView::sigStopGenerated, this, &PlayController::stop, Qt::QueuedConnection);
    QObject::connect(view.get(), &AbstractPlayView::sigPosChanged, this, &PlayController::setPos, Qt::QueuedConnection);
    QObject::connect(view.get(), &AbstractPlayView::sigTempoChanged, this, &PlayController::setTempo, Qt::QueuedConnection);
}
