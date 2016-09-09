#ifndef TILE_H
#define TILE_H

#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>

enum class Direction {
    TopLeft = 0,
    Top,
    TopRight,
    Left,
    Right,
    BottomLeft,
    Bottom,
    BottomRight
};

const static QMap<Direction, QPoint> Directions {
    {Direction::TopLeft, QPoint(-1, -1)},
    {Direction::Top, QPoint(0, -1)},
    {Direction::TopRight, QPoint(1, -1)},
    {Direction::Left, QPoint(-1, 0)},
    {Direction::Right, QPoint(1, 0)},
    {Direction::BottomLeft, QPoint(-1, 1)},
    {Direction::Bottom, QPoint(0, 1)},
    {Direction::BottomRight, QPoint(1, 1)}
};

struct TileData;
class Tile : public QGraphicsItem
{
public:
    Tile();
    ~Tile();

    enum State {
        Cover = 0,
        Flag,
        Tag,
        Explode,
        Uncover
    };

    static qreal size();
    static qreal setSize(qreal newSize);

    QRectF boundingRect() const override final;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override final;

    QPoint index() const;
    void setIndex(const QPoint& index);

    bool isMine() const;
    void setIsMine(bool isMine = true);

    State state() const;
    void setState(State state);

    bool isPressed(Qt::MouseButton button) const;
    void setPressed(Qt::MouseButton button, bool pressed);

    quint8 surroundingMines() const;
    void setSurroundingMines(quint8 mines);

    QSharedPointer<Tile> neighbour(Direction pos) const;
    void setNeighbour(Direction pos, QSharedPointer<Tile> neighbour);

private:
    TileData *d = nullptr;
};

#endif // TILE_H
