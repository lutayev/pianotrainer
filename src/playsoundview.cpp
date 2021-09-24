#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QSpacerItem>
#include <QLabel>
#include <QDebug>

#include "playsoundview.h"

PlaySoundView::PlaySoundView(const QString& name, QWidget* parent) : AbstractPlayView(name, parent)
{
    m_cachedPosition = 0;
    m_lastEventIndex = 0;
    QVBoxLayout* btnLayout = new QVBoxLayout;
    QVBoxLayout* tempoLayout = new QVBoxLayout;
    QHBoxLayout* hLayout = new QHBoxLayout;
    QVBoxLayout* vLayout = new QVBoxLayout;

    m_btnStop = new QPushButton("Stop");
    m_btnPlay = new QPushButton("Pause");
    m_btnTempoReset = new QPushButton("Tempo Reset");
    m_sbPosition = new QSpinBox;
    m_sbPosition->setVisible(false);
    m_slTempo = new QSlider();
    m_slTempo->setRange(0, 200);
//    m_slTempo->setMaximumHeight(50);
    m_slPosition = new QSlider();
    m_slPosition->setOrientation(Qt::Horizontal);

    btnLayout->addWidget(m_btnStop);
    btnLayout->addWidget(m_btnPlay);
    btnLayout->addWidget(m_btnTempoReset);
    btnLayout->setContentsMargins(0, 0, 100, 0);

    tempoLayout->addWidget(new QLabel("Tempo"));
    tempoLayout->addWidget(m_slTempo);

    hLayout->addItem(btnLayout);
    hLayout->addWidget(m_sbPosition);
    hLayout->addItem(tempoLayout);


    vLayout->addItem(hLayout);
    vLayout->addWidget(m_slPosition);

    this->setLayout(vLayout);

    connect(m_btnStop, &QPushButton::clicked, this, &PlaySoundView::sigStopGenerated);

    connect(m_btnPlay, &QPushButton::clicked, [this]() {
        if (m_btnPlay->text() == "Play") {
            m_btnPlay->setText("Pause");
            m_pauseState = false;
        } else {
            m_btnPlay->setText("Play");
            m_pauseState = true;
        }
        emit sigPauseGenerated(m_pauseState);
    });

    connect(m_btnTempoReset, &QPushButton::clicked, [this]() {
        emit sigTempoChanged(1.0);
    });

    connect(m_sbPosition, &QSpinBox::editingFinished, this, [this]() {
        emit sigPosChanged(m_sbPosition->value());
    });

    connect(m_slPosition, &QSlider::sliderPressed, this, [this]() {
        emit sigPauseGenerated(true);
    });

    connect(m_slPosition, &QSlider::sliderReleased, this, [this]() {
        emit sigPosChanged(m_slPosition->value());
        emit sigPauseGenerated(m_pauseState);
    });

    connect(m_slTempo, &QSlider::sliderReleased, this, [this]() {
        float tempo = static_cast<float>(m_slTempo->value()) / 100.0;
        emit sigTempoChanged(tempo);
        emit sigPauseGenerated(m_pauseState);
    });
}

PlaySoundView::~PlaySoundView()
{
    qDebug() << "destructor";
    m_midiOut.disconnect();
}

void PlaySoundView::dispatchPlay()
{
    qDebug() << "Dispatch play";
    m_btnPlay->setText("Pause");
    if (!m_midiOut.isConnected())
        m_midiOut.connect("0");
    int max = m_song->getEndPos();
    m_slPosition->setRange(0, max);
    m_sbPosition->setRange(0, max);
}

void PlaySoundView::dispatchPause()
{
    qDebug() << "Dispatch pause";
    m_btnPlay->setText("Play");
    //m_lastIndex = 0;
    //m_lastIndex = 0;
}

void PlaySoundView::dispatchStop()
{
    qDebug() << "Dispatch Stop";
    m_lastEventIndex = 0;
    m_cachedPosition = 0;
    m_midiOut.disconnect();
    m_btnPlay->setText("Play");
    m_sbPosition->setValue(0);
    m_slPosition->setValue(0);
}

void PlaySoundView::dispatchPlayPos(qint32 pos)
{
    qDebug() << "Dispatch POS " + QString::number(pos);
    m_sbPosition->setValue(pos);
    m_slPosition->setValue(pos);

    //Imitate laggy processing
    //std::this_thread::sleep_for(std::chrono::milliseconds(100));

    qint32 lastEventTick = 0;
    while (m_lastEventIndex < m_events.size()) {
        QMidiEvent* event = m_events.at(m_lastEventIndex);
        lastEventTick = event->tick();

        if (lastEventTick <= pos) {
            //qDebug() << "Processing " + QString::number(m_lastEventIndex) + " of " + QString::number(m_events.size())
            //         << "Last tick " + QString::number(lastEventTick) + ", POS " + QString::number(pos);

            if (event->type() == QMidiEvent::Meta || event->type() == QMidiEvent::SysEx) {
                m_midiOut.sendEvent(*event);
                ++m_lastEventIndex;
                continue;
            }
            ++m_lastEventIndex;
            qint32 msg = event->message();
            m_midiOut.sendEvent(*event);
            //qDebug() << "Send event message " + QString::number(msg);
        } else {
            break;
        }
    }
}

void PlaySoundView::dispatchJmpPos(qint32 pos)
{
    m_midiOut.stopAll();
    for (qint32 i = 0; i < m_events.size(); ++i) {
        if (m_events.at(i)->tick() >= pos) {
            qDebug() << "Set last index" << i << m_lastEventIndex;
            m_lastEventIndex = i + 1;
            break;
            //qDebug() << "AAARGH" << m_events.at(i)->tick() << pos;
            //m_midiOut.sendEvent(*m_events.at(i));
        }
    }
}

void PlaySoundView::dispatchTempo(float tempo)
{
    m_slTempo->setValue(tempo * 100);
}
