#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_player = new Player(this);
    m_player->setFile("file.mp3");
    connect(m_player, SIGNAL(positionChanged(int)), this, SLOT(positionChanged(int)));
}

MainWindow::~MainWindow() {
    //    delete ui;
}

void MainWindow::on_playButton_clicked() {
    m_player->start();
}

void MainWindow::on_stopButton_clicked() {
    m_player->stop();
}

void MainWindow::on_fwdButton_clicked() {
    m_player->fwd();
}

void MainWindow::on_backButton_clicked() {
    m_player->back();
}

void MainWindow::positionChanged(int pos) {
    QLabel* label = findChild<QLabel*> ("posLabel");
    QString s = "Position: " + QString::number(pos) + " of " + QString::number(m_player->duration());
    label->setText(s);

    QSlider *slider = findChild<QSlider*> ("progressSlider");
    slider->setValue(100 * ((float)pos / m_player->duration()));
}

void MainWindow::on_pauseButton_clicked() {
    m_player->pause();
}

void MainWindow::on_setButton_clicked() {
    m_player->setTempo( 1.0f);
}

void MainWindow::on_setButton2_clicked() {
    m_player->setTempo( 0.5f);
}

void MainWindow::on_setButton3_clicked() {
    m_player->setTempo( 2.0f);
}

void MainWindow::on_progressSlider_sliderReleased() {
    QSlider *slider = findChild<QSlider*> ("progressSlider");
    int value = slider->value();
    if(value >= 0 && value < 100) {
        int pos = m_player->duration() * ((float)value / 100);
        m_player->setPosition(pos);
    }
}
