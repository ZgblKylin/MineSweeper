#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QtWidgets>
#include "MineSweeper.h"

namespace Ui {
class MainWindow;
}

class CustomDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent* e);

    Q_SLOT void on_actionRestart_triggered();
    Q_SLOT void on_actionSimple_triggered();
    Q_SLOT void on_actionNormal_triggered();
    Q_SLOT void on_actionHard_triggered();
    Q_SLOT void on_actionCustom_triggered();
    Q_SLOT void on_actionRank_triggered();
    Q_SLOT void on_actionQuit_triggered();
    Q_SLOT void on_actionHelp_triggered();
    Q_SLOT void on_actionAbout_triggered();
    Q_SLOT void on_buttonRestart_clicked();

    Q_SLOT void on_mineField_press();
    Q_SLOT void on_mineField_release();

    Q_SLOT void success();
    Q_SLOT void explode();
    Q_SLOT void update();

private:
    void startGame(MineSweeper::Difficulty difficulty, bool resize = true);

    Q_SLOT void timeout();

    Ui::MainWindow* ui;
    MineSweeper* mc;
    CustomDialog* customDialog;

    QSize tileSize;
    int maxMineCount = 0;

    QSize initWindowSize;
    QSize initFieldSize;

    bool finished = false;
    QTimer timer;
};

#endif // MAINWINDOW_H
