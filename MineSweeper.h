#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QtCore>

enum class Direct {
    TopLeft = 0,
    Top,
    TopRight,
    Left,
    Right,
    BottomLeft,
    Bottom,
    BottomRight
};

const QPoint Direction[8] = {
    QPoint(-1, -1), // TopLeft
    QPoint( 0, -1), // Top
    QPoint( 1, -1), // TopRight
    QPoint(-1,  0), // Left
    QPoint( 1,  0), // Right
    QPoint(-1,  1), // BottomLeft
    QPoint( 0,  1), // Bottom
    QPoint( 1,  1)  // BottomRight
};

#pragma pack(push, 1)
struct Tile
{
    Tile()
    {
        pressed[Qt::LeftButton] = false;
        pressed[Qt::MidButton] = false;
        pressed[Qt::RightButton] = false;
    }
    ~Tile() {}
    Tile(const Tile& other) = default;
    Tile(Tile&& other) = default;
    Tile& operator=(const Tile& other) = default;
    Tile& operator=(Tile&& other) = default;

    enum State {
        Cover = 0,
        Flag,
        Tag,
        Explode,
        Uncover
    };

    QPoint pos;                             // tile pos - QPoint(col, row)
    bool isMine = false;                      // has mine
    State state = Cover;                    // tile state
    QMap<Qt::MouseButton, bool> pressed;    // mouse button pressed
    int count = 0;                          // surrounding mine counts
    Tile* neighbours[8] = {nullptr};        // neighbours
};
#pragma pack(pop)
Q_DECLARE_TYPEINFO(Tile, Q_PRIMITIVE_TYPE);

class QMainWindow;
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

    static const qreal TileSize;
    static MineSweeper* instance();

    Q_SIGNAL void success();
    Q_SIGNAL void explode();
    Q_SIGNAL void update();

    void init(QMainWindow* mainWindow, QSize minimumSize);

    bool isScreenHorizontal() const;
    const QVector<QVector<Tile> > getTiles() const;
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
    void leftClick(Tile* tile);
    void midClick(Tile* tile);
    void rightClick(Tile* tile);
    bool uncover(Tile* tile);
    void calcRank();
    void checkSuccess();

    bool screenHorizontal = true;
    QVector<QVector<Tile> > tiles;
    Difficulty difficulty = Difficulty::Simple;
    State state = State::Running;
    QSize tileSize;
    int maxMineCount = 0;
    int mineCount = 0;
    QSettings* settings;
    int rank;
    QMap<Difficulty, QList<QVariantList> > ranklist;
    QPoint columnRange = QPoint(10, 30);
    QPoint rowRange = QPoint(10, 24);
    QElapsedTimer timer;
};

#endif // MINESWEEPER_H
