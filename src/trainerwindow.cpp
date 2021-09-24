#include <QFileDialog>

#include "trainerwindow.h"
#include "ui_trainerwindow.h"

TrainerWindow::TrainerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainerWindow)
{
    ui->setupUi(this);


    std::shared_ptr<AbstractPlayView> audioView = std::make_shared<PlaySoundView>(m_soundViewName, this);
    std::shared_ptr<AbstractPlayView> pianoView = std::make_shared<PlayPianoView>(m_keyboardViewName, this);

    ui->gbControl->layout()->addWidget(audioView.get());
    ui->gbKeyboard->layout()->addWidget(pianoView.get());

    m_controller = new PlayController(nullptr, audioView);
    m_controller->addView(pianoView);
    audioView->setEnabled(false);

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(loadSong()));

    connect(ui->cbDetachKeyboard, &QCheckBox::stateChanged, [this]() {
        QWidget* parent = (ui->cbDetachKeyboard->isChecked() ? nullptr : ui->gbKeyboard);
        m_controller->attach(m_keyboardViewName, parent);
    });


    connect(ui->cbDetachControls, &QCheckBox::stateChanged, [this]() {
        QWidget* parent = (ui->cbDetachControls->isChecked() ? nullptr : ui->gbControl);
        m_controller->attach(m_soundViewName, parent);
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
