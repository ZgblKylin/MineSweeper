#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QtCore/QtCore>
#include "Tile.h"

class QMainWindow;
class MineSweeperPrivate;
class MineSweeper : public QObject
{
    Q_OBJECT

public:
    enum class Difficulty {
        Simple = 0,
        Normal,
        Hard,
        Custom
    };
    enum class State {
        Running = 0,
        Success,
        Fail
    };

    MineSweeper();
    ~MineSweeper();

    static MineSweeper* instance();

    Q_SIGNAL void success();
    Q_SIGNAL void explode();
    Q_SIGNAL void update();

    void init(QMainWindow* mainWindow, QSize minimumSize);

    bool isScreenHorizontal() const;
    const QVector<QVector<QSharedPointer<Tile> > > getTiles() const;
    Difficulty getDifficulty() const;
    State getState() const;
    QSize getTileSize() const;
    int getMaxMineCount() const;
    int getMineCount() const;
    int getRank() const;
    QList<QVariantList> getRank(Difficulty difficulty) const;
    const QPoint getColumnRange() const;
    const QPoint getRowRange() const;
    qreal getTime() const;

    void startGame(Difficulty difficulty = Difficulty::Simple, QSize size = QSize(), int mines = 0);
    void setPressed(const QPoint& pos, Qt::MouseButton button, bool pressed);

private:
    void leftClick(QSharedPointer<Tile> tile);
    void midClick(QSharedPointer<Tile> tile);
    void rightClick(QSharedPointer<Tile> tile);
    bool uncover(QSharedPointer<Tile> tile);
    void calcRank();
    void checkSuccess();

    bool screenHorizontal = true;
    QVector<QVector<QSharedPointer<Tile> > > tiles;
    MineSweeper::Difficulty difficulty = MineSweeper::Difficulty::Simple;
    MineSweeper::State state = MineSweeper::State::Running;
    QSize tileSize;
    int maxMineCount = 0;
    int mineCount = 0;
    QSettings* settings;
    int rank;
    QMap<MineSweeper::Difficulty, QList<QVariantList> > ranklist;
    QPoint columnRange = QPoint(10, 30);
    QPoint rowRange = QPoint(10, 24);
    QElapsedTimer timer;
};

#endif // MINESWEEPER_H
