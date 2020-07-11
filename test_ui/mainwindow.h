#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "player.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_playButton_clicked();

    void on_fwdButton_clicked();

    void on_backButton_clicked();

    void positionChanged(int pos);

    void on_stopButton_clicked();

    void on_pauseButton_clicked();

    void on_setButton_clicked();

    void on_setButton2_clicked();

    void on_setButton3_clicked();

    void on_progressSlider_sliderReleased();

private:
    Ui::MainWindow *ui;
    Player* m_player;
    int m_position;
};

#endif // MAINWINDOW_H
