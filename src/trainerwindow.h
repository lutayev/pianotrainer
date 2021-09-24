#ifndef TRAINERWINDOW_H
#define TRAINERWINDOW_H

#include <QWidget>
#include <QApplication>
#include "playcontroller.h"
#include "playpianoview.h"
#include "playsoundview.h"

namespace Ui {
class TrainerWindow;
}

class TrainerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TrainerWindow(QWidget *parent = 0);
    ~TrainerWindow();

private slots:
    void loadSong();

private:
    Ui::TrainerWindow *ui;

    PlayController* m_controller;
    QString         m_keyboardViewName {"Keyboard"};
    QString         m_soundViewName {"Sound"};
    QString         m_controlsViewName {"Controls"};
};

#endif // TRAINERWINDOW_H
