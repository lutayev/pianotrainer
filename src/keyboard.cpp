#include "keyboard.h"

Keyboard::Keyboard(QWidget* parent) : QWidget(parent)
{
    this->setLayout(new QHBoxLayout);
    this->setFixedWidth(1000);
    for (uint8_t i = m_rangeMin; i <= m_rangeMax; ++i) {
        QLabel* key = new QLabel(QString::number(i));
        key->setStyleSheet(m_defaultStyle);
        key->setFixedHeight(50);
        m_keys[i] = key;
        this->layout()->addWidget(key);
    }
}

void Keyboard::setRange(std::pair<uint8_t, uint8_t> range)
{
    for (uint8_t i = m_rangeMin; i <= m_rangeMax; ++i) {
        if (i < range.first || i > range.second) {
            m_keys[i]->setVisible(false);
        }
        else m_keys[i]->setVisible(true);
    }
}

bool Keyboard::isPressed(uint8_t key) const noexcept
{
    return m_keys.at(key)->styleSheet() == m_pressedStyle;
}

void Keyboard::press(uint8_t key, Keyboard::KeyState state)
{
    switch(state) {
    case Pressed:
        m_keys[key]->setStyleSheet(m_pressedStyle);
        break;
    case Released:
        m_keys[key]->setStyleSheet(m_defaultStyle);
        break;
    case Pending1:
        m_keys[key]->setStyleSheet(m_pendingStyle1);
        break;
    case Pending2:
        m_keys[key]->setStyleSheet(m_pendingStyle2);
        break;
    case Pending3:
        m_keys[key]->setStyleSheet(m_pendingStyle3);
        break;
    case Pending4:
        m_keys[key]->setStyleSheet(m_pendingStyle4);
        break;
    case Pending5:
        m_keys[key]->setStyleSheet(m_pendingStyle5);
        break;
    }
}

void Keyboard::reset()
{
    for (int i = m_rangeMin; i < m_rangeMax; ++i) {
        m_keys[i]->setStyleSheet(m_defaultStyle);
    }
}
