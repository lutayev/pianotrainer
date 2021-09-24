#include <QtWidgets/QApplication>
//#include "pianotrainer.h"
#include "trainerwindow.h"

int main(int argc, char** argv) {

    QApplication app(argc, argv);
    //PianoTrainer PianoApp;
    TrainerWindow wnd;
    wnd.show();

    return app.exec();
}
