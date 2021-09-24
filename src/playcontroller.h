#ifndef PLAYCONTROLLER_H
#define PLAYCONTROLLER_H

#include <QString>

#include <atomic>
#include <thread>
#include <unordered_map>
#include <memory>

#include <src/QMidi/src/QMidiFile.h>
#include <src/QMidi/src/QMidiIn.h>
#include <src/QMidi/src/QMidiOut.h>

#include "songmodel.h"
#include "abstractplayview.h"
#include "playsoundview.h"

class PlayController : public QObject
{
    Q_OBJECT

    enum class PlayState : uint8_t{
        Playing,
        Paused,
        Stopped,

        ToBePlaying,
        ToBePaused,
    };

public:
    //Контроллер высылает сигналы (каждый n-ный тик).
    //Чтобы не было просадок фпс, лагов и тормозов, вьюхи могут состояние "занят" и режим "ленивой" обработки сообщений и в таком случае игнорировать сигналы, пока не обработается текущий
    //Вьюхи получают сигнал, бегут по модели с текущего индекса по элементам до первого, который еще не "наступил".
    //Если время наступило (с учетом "смещения" в будущее - индекс смещается вперед, создается объект и падает, пока не "упадет".
    //Интенсивность падения вычисляется ("тик события минус смещение": спавн, сам тик события - падение.
    //Разница между событием нажатия и событием отмены - длина объекта.

    //Вьюхи могут генерить сигналы паузы, стопа, перемотки, на которые подписывается контроллер и после изменения своего состояния отправляет обратные же сигналы всем вьюхам
    PlayController(std::shared_ptr<SongModel> songModel, std::shared_ptr<AbstractPlayView> view);
    void addView(std::shared_ptr<AbstractPlayView> view);
    void setSong(std::shared_ptr<SongModel> model);
    void attach(const QString& viewName, QWidget* parent);

public slots:
    bool start();
    bool stop();
    void pause(bool pause);
    void setPos(qint32 pos);
    void setTempo(float tempo);

private:
    void updateStateLoop();

    //State
    std::atomic<qint32>             m_position{0};
    std::atomic<PlayState>          m_state{PlayState::Stopped};
    std::atomic<float>              m_tempo{1.0};

    //Model and views
    std::shared_ptr<SongModel>      m_songModel {nullptr};
    std::unordered_map<std::string, std::shared_ptr<AbstractPlayView>>      m_views;

signals:
    void sigPlay();
    void sigPause();
    void sigStop();
    void sigPlayPos(qint32 pos);
    void sigJmpPos(qint32 pos);
    void sigTempo(float tempo);
};

#endif // PLAYCONTROLLER_H
