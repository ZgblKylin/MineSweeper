#include "Tile.h"

qreal TileSize = 32;

struct TileData
{
    QPoint index;                               // tile pos - QPoint(col, row)
    bool isMine = false;                        // has mine
    Tile::State state = Tile::Cover;            // tile state
    QMap<Qt::MouseButton, bool> isPressed = {   // mouse button pressed
        {Qt::LeftButton, false},
        {Qt::MidButton, false},
        {Qt::RightButton, false}};
    quint8 surroundingMines = 0;                              // surrounding mine counts
    QMap<Direction, QSharedPointer<Tile> > neighbours;         // neighbours
};

Tile::Tile()
{
    d = new TileData();
}

Tile::~Tile()
{
    delete d;
}

qreal Tile::size()
{
    return TileSize;
}

qreal Tile::setSize(qreal newSize)
{
    TileSize = newSize;
}

QRectF Tile::boundingRect() const
{
    return QRectF(0, 0, Tile::size(), Tile::size());
}

void Tile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->fillRect(1, 1, Tile::size() - 2, Tile::size() - 2, Qt::black);
}

QPoint Tile::index() const
{
    return d->index;
}

void Tile::setIndex(const QPoint& index)
{
    d->index = index;
}

bool Tile::isMine() const
{
    return d->isMine;
}

void Tile::setIsMine(bool isMine)
{
    d->isMine = isMine;
}

Tile::State Tile::state() const
{
    return d->state;
}

void Tile::setState(State state)
{
    d->state = state;
}

bool Tile::isPressed(Qt::MouseButton button) const
{
    return d->isPressed.value(button);
}

void Tile::setPressed(Qt::MouseButton button, bool pressed)
{
    d->isPressed[button] = pressed;
}

quint8 Tile::surroundingMines() const
{
    return d->surroundingMines;
}

void Tile::setSurroundingMines(quint8 mines)
{
    d->surroundingMines = mines;
}

QSharedPointer<Tile> Tile::neighbour(Direction pos) const
{
    return d->neighbours.value(pos);
}

void Tile::setNeighbour(Direction pos, QSharedPointer<Tile> neighbour)
{
    d->neighbours[pos] = neighbour;
}
