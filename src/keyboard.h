#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <unordered_map>


class Keyboard : public QWidget{
public:
    enum KeyState {Pressed, Released, Pending1, Pending2, Pending3, Pending4, Pending5};

    Keyboard(QWidget* parent = 0);
    void setRange(std::pair<uint8_t, uint8_t> range);
    bool isPressed(uint8_t key) const noexcept;
    void press(uint8_t key, KeyState state);
    void reset();

private:

    uint8_t m_rangeMin {0};
    uint8_t m_rangeMax {128};
    std::unordered_map<uint8_t, QLabel*> m_keys;
    QString m_defaultStyle{"QLabel { background-color : white; color : black; }"};
    QString m_pressedStyle{"QLabel { background-color : blue; color : blue; }"};
    QString m_pendingStyle1{"QLabel { background-color : #e3e1e1; color : black; }"};
    QString m_pendingStyle2{"QLabel { background-color : #cfcccc; color : black; }"};
    QString m_pendingStyle3{"QLabel { background-color : #bab8b8; color : black; }"};
    QString m_pendingStyle4{"QLabel { background-color : #a6a2a2; color : black; }"};
    QString m_pendingStyle5{"QLabel { background-color : #858282; color : black; }"};
};

#endif // KEYBOARD_H
