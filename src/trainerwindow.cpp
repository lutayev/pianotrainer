#include <QFileDialog>

#include "trainerwindow.h"
#include "ui_trainerwindow.h"

TrainerWindow::TrainerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainerWindow)
{
    ui->setupUi(this);


    //Async listener without GUI
    std::shared_ptr<AbstractPlayView> soundListener = std::make_shared<PlaySoundListener>(m_soundListenerName, this, true);

    //Gui listeners
    std::shared_ptr<AbstractPlayView> controlsListener = std::make_shared<PlaySoundView>(m_controlsListenerName, this, false);
    std::shared_ptr<AbstractPlayView> keyboardListener = std::make_shared<PlayPianoView>(m_keyboardListenerName, this, false);

    ui->gbControl->layout()->addWidget(controlsListener.get()->getWidget());
    ui->gbKeyboard->layout()->addWidget(keyboardListener.get()->getWidget());

    m_controller = new PlayController(nullptr, soundListener);

    m_controller->addView(controlsListener);
    m_controller->addView(keyboardListener);
    controlsListener->getWidget()->setEnabled(false);

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(loadSong()));

    connect(ui->cbDetachKeyboard, &QCheckBox::stateChanged, [this]() {
        QWidget* parent = (ui->cbDetachKeyboard->isChecked() ? nullptr : ui->gbKeyboard);
        m_controller->attach(m_keyboardListenerName, parent);
    });


    connect(ui->cbDetachControls, &QCheckBox::stateChanged, [this]() {
        QWidget* parent = (ui->cbDetachControls->isChecked() ? nullptr : ui->gbControl);
        m_controller->attach(m_controlsListenerName, parent);
    });
}

TrainerWindow::~TrainerWindow()
{
    delete ui;
}

void TrainerWindow::loadSong()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select file"), QDir::currentPath(), tr("Midi files (*.mid *.midi)"));
    if (!file.size()) {
        return;
    }
    ui->lblFile->setText(file);
    std::shared_ptr<QMidiFile> midi = std::make_shared<QMidiFile>();
    midi->load(file);
    std::shared_ptr<SongModel> song = std::make_shared<SongModel>(midi);
    m_controller->setSong(song);
}
