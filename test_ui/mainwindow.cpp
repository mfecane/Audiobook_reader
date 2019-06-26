#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_player = new Player(this);
    connect(m_player, SIGNAL(positionChanged(int)), this, SLOT(positionChanged(int)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_playButton_clicked() {
    m_player->play();
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
    slider->setRange(0, m_player->duration());
    slider->setValue(pos);

}

void MainWindow::on_stopButton_clicked() {
    m_player->stop();
}

void MainWindow::on_pauseButton_clicked() {
    m_player->pause();
}

void MainWindow::on_setButton_clicked(){
    m_player->setTempo( 2.0f);
}
