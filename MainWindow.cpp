#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CustomDialog.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    initUi();
    initLogic();
    initField();
    startGame(MineSweeper::Difficulty::Simple, false);
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
    rect.setSize(size());
    rect.moveCenter(qApp->desktop()->availableGeometry(this).center());
    move(rect.topLeft());
}

void MainWindow::on_actionRestart_triggered()
{
    startGame(logic->getDifficulty());
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
    ui->mineNum->display(logic->getMineCount());
    ui->mineField->update();
    QMainWindow::update();
}

void MainWindow::initUi()
{
    ui->setupUi(this);

    ui->mineNum->setFixedHeight(ui->mineLabel->height() * 2);
    ui->timeNum->setFixedHeight(ui->timeLabel->height() * 2);
    ui->timeNum->setFixedWidth(ui->timeLabel->width() * 2);
    ui->mineNum->setFixedWidth(ui->timeNum->width());

    ui->buttonRestart->setFixedSize(ui->timeLayout->sizeHint().height() + 12,
                                    ui->timeLayout->sizeHint().height() + 12);
    ui->buttonRestart->setIconSize(ui->buttonRestart->size());

    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionHelp->setShortcuts(QKeySequence::HelpContents);

    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowCloseButtonHint | Qt::WindowShadeButtonHint);
}

void MainWindow::initLogic()
{
    logic = MineSweeper::instance();
    connect(logic, &MineSweeper::success,
            this, &MainWindow::success);
    connect(logic, &MineSweeper::explode,
            this, &MainWindow::explode);
    connect(logic, &MineSweeper::update,
            this, &MainWindow::update);

    customDialog = new CustomDialog(logic->getColumnRange(), logic->getRowRange(), this);

    logic->init(this, QSize(ui->mainLayout->contentsMargins().left()
                            + ui->mainLayout->contentsMargins().right()
                            + 4,
                            height() - 2));

    connect(&timer, &QTimer::timeout,
            this, &MainWindow::timeout);
    timer.setInterval(10);
    timer.start();
}

void MainWindow::initField()
{
    QMargins margins = ui->centralwidget->layout()->contentsMargins();
    baseSize = QSize(margins.left() + margins.right() + ui->frame->lineWidth() * 2,
                     height());
    QSize baseWindowSize = frameGeometry().size() - size() + baseSize;
    QSizeF maxFieldSize = qApp->desktop()->availableGeometry().size() - baseWindowSize;
    qreal maxTileWidth = maxFieldSize.width() / logic->getColumnRange().y() - 1;
    qreal maxTileHeight = maxFieldSize.height() / logic->getRowRange().y() - 1;
    Tile::setSize(std::min(maxTileWidth, maxTileHeight));
    baseSize = QSize();

    ui->mineField->init();
}

void MainWindow::startGame(MineSweeper::Difficulty difficulty, bool resize)
{
    finished = false;
    ui->buttonRestart->setIcon(QIcon(":/image/smile"));

    logic->startGame(difficulty, tileSize, maxMineCount);

    tileSize = logic->getTileSize();
    maxMineCount = logic->getMaxMineCount();

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
    ui->timeNum->display(QStringLiteral("%1").arg(logic->getTime(),
                                                  0,
                                                  'f',
                                                  3,
                                                  QLatin1Char(' ')));
}
