#ifndef PLAYPIANOVIEW_H
#define PLAYPIANOVIEW_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

#include <unordered_map>

#include "abstractplayview.h"
#include "keyboard.h"


class PlayPianoView : public AbstractPlayView
{
    Q_OBJECT

public:
    explicit PlayPianoView(const QString& name = "PianoView", QWidget *parent = nullptr);
    ~PlayPianoView() override;


    // PlayView interface
protected:
    void dispatchPlay() override;
    void dispatchPause() override;
    void dispatchStop() override;
    void dispatchPlayPos(qint32 pos) override;
    void dispatchJmpPos(qint32 pos) override;
    void dispatchTempo(float tempo) override;

private:
    void reset();

    Keyboard*    m_keyboard;
    qint32 m_lastIndex{0};
    QLabel*     m_lblLength;

//    std::unordered_map<uint8_t, QWidget*> m_keys;
//    QString m_defaultStyle{"QLabel { background-color : white; color : black; }"};
//    QString m_pressedStyle{"QLabel { background-color : red; color : blue; }"};
//    QString m_pendingStyle1{"QLabel { background-color : #e3e1e1; color : black; }"};
//    QString m_pendingStyle2{"QLabel { background-color : #cfcccc; color : black; }"};
//    QString m_pendingStyle3{"QLabel { background-color : #bab8b8; color : black; }"};
//    QString m_pendingStyle4{"QLabel { background-color : #a6a2a2; color : black; }"};
//    QString m_pendingStyle5{"QLabel { background-color : #858282; color : black; }"};
};

#endif // PLAYPIANOVIEW_H
