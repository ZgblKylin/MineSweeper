#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CustomDialog.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    initUi();
//    initField();

    mc = MineSweeper::instance();
    connect(mc, &MineSweeper::success,
            this, &MainWindow::success);
    connect(mc, &MineSweeper::explode,
            this, &MainWindow::explode);
    connect(mc, &MineSweeper::update,
            this, &MainWindow::update);

    connect(&timer, &QTimer::timeout,
            this, &MainWindow::timeout);
    timer.setInterval(10);

    ui->setupUi(this);

    QPoint colRange = mc->getColumnRange();
    QPoint rowRange = mc->getRowRange();
    customDialog = new CustomDialog(colRange, rowRange, this);

    mc->init(this, QSize(ui->mainLayout->contentsMargins().left()
                         + ui->mainLayout->contentsMargins().right()
                         + 4,
                         height() - 2));

    ui->buttonRestart->setFixedSize(ui->timeLayout->sizeHint().height() + 12,
                                    ui->timeLayout->sizeHint().height() + 12);
    ui->buttonRestart->setIconSize(ui->buttonRestart->size());

    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionHelp->setShortcuts(QKeySequence::HelpContents);

    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowCloseButtonHint | Qt::WindowShadeButtonHint);

    QMargins margins = ui->centralwidget->layout()->contentsMargins();
    baseSize = QSize(margins.left() + margins.right() + ui->frame->lineWidth() * 2,
                     height());
    QSizeF maxFieldSize = qApp->desktop()->availableGeometry().size() - baseSize;
    qreal maxTileWidth = maxFieldSize.width() / colRange.y() - 1;
    qreal maxTileHeight = maxFieldSize.height() / rowRange.y() - 1;
    Tile::setSize(std::min(maxTileWidth, maxTileHeight));
    baseSize = QSize();

    ui->mineField->init();
    startGame(MineSweeper::Difficulty::Simple, false);

    timer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent* e)
{
    if(!baseSize.isValid())
    {
        baseSize = e->size() - ui->mineField->size();
        setFixedSize(e->size());
    }
    QMainWindow::resizeEvent(e);

    QRect rect = frameGeometry();
    rect.setSize(minimumSizeHint());
    rect.moveCenter(qApp->desktop()->availableGeometry(this).center());
    move(rect.topLeft());
}

void MainWindow::on_actionRestart_triggered()
{
    startGame(mc->getDifficulty());
}

void MainWindow::on_actionSimple_triggered()
{
    startGame(MineSweeper::Difficulty::Simple);
}

void MainWindow::on_actionNormal_triggered()
{
    startGame(MineSweeper::Difficulty::Normal);
}

void MainWindow::on_actionHard_triggered()
{
    startGame(MineSweeper::Difficulty::Hard);
}

void MainWindow::on_actionCustom_triggered()
{
    switch(customDialog->exec())
    {
    case QDialog::Accepted:
        tileSize = customDialog->getTileSize();
        maxMineCount = customDialog->getMineCount();
        startGame(MineSweeper::Difficulty::Custom);
        break;
    case QDialog::Rejected:
        break;
    }
}

void MainWindow::on_actionRank_triggered()
{
    ;
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionHelp_triggered()
{
    ;
}

void MainWindow::on_actionAbout_triggered()
{
    qApp->aboutQt();
}

void MainWindow::on_buttonRestart_clicked()
{
    on_actionRestart_triggered();
}

void MainWindow::on_mineField_press()
{
    if(finished)
        return;
    ui->buttonRestart->setIcon(QIcon(":/image/hushed"));
}

void MainWindow::on_mineField_release()
{
    if(finished)
        return;
    ui->buttonRestart->setIcon(QIcon(":/image/smile"));
}

void MainWindow::success()
{
    finished = true;
    ui->buttonRestart->setIcon(QIcon(":/image/cool"));

    ui->mineField->success();
}

void MainWindow::explode()
{
    finished = true;
    ui->buttonRestart->setIcon(QIcon(":/image/confounded"));

    ui->mineField->explode();
}

void MainWindow::update()
{
    ui->mineNum->setValue(mc->getMineCount());
    ui->mineField->update();
    QMainWindow::update();
}

void MainWindow::startGame(MineSweeper::Difficulty difficulty, bool resize)
{
    finished = false;
    ui->buttonRestart->setIcon(QIcon(":/image/smile"));

    mc->startGame(difficulty, tileSize, maxMineCount);

    tileSize = mc->getTileSize();
    maxMineCount = mc->getMaxMineCount();

    ui->mineField->started();

    if(resize)
        setFixedSize(baseSize + ui->mineField->size());

    QRect rect = frameGeometry();
    rect.moveCenter(qApp->desktop()->availableGeometry(this).center());
    move(rect.topLeft());

    update();
}

void MainWindow::timeout()
{
    ui->timeNum->setValue(mc->getTime());
}
