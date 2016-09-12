#include "Tile.h"
#include "MineSweeper.h"

qreal TileSize = 32;

struct TileData
{
    QPoint index;                      // tile pos - QPoint(col, row)
    bool isMine = false;                        // has mine
    Tile::State state = Tile::Cover;            // tile state
    QMap<Qt::MouseButton, bool> isPressed = {   // mouse button pressed
                                                {Qt::LeftButton, false},
                                                {Qt::MidButton, false},
                                                {Qt::RightButton, false}};
    quint8 surroundingMines = 0;                              // surrounding mine counts
    QMap<Direction, QSharedPointer<Tile> > neighbours;         // neighbours
    MineSweeper* logic = MineSweeper::instance();
};

Tile::Tile()
{
    d = new TileData();
    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setAcceptHoverEvents(true);
}

Tile::~Tile()
{
    delete d;
}

qreal Tile::size()
{
    return TileSize;
}

void Tile::setSize(qreal newSize)
{
    TileSize = newSize;
}

QRectF Tile::boundingRect() const
{
    return QRectF(0, 0, Tile::size(), Tile::size());
}

void Tile::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);

    QPen pen = painter->pen();
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    fillTileRect(painter, option);
    drawTileGrid(painter, option);
    drawTileImage(painter, option);
    drawTileBoarder(painter, option);
    drawTileText(painter, option);
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

void Tile::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Q_FOREACH(auto pressed, d->isPressed.values())
    {
        if(pressed)
            return;
    }

    switch(event->button())
    {
    case Qt::LeftButton:
    case Qt::MidButton:
    case Qt::RightButton:
        d->logic->setPressed(index(), event->button(), true);
        break;
    default:
        break;
    }

    update();
    event->accept();
}

void Tile::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(!isPressed(event->button()))
        return;

    switch(event->button())
    {
    case Qt::LeftButton:
    case Qt::MidButton:
    case Qt::RightButton:
        if(isUnderMouse())
            d->logic->click(index(), event->button());
        d->logic->setPressed(index(), event->button(), false);
        break;
    default:
        break;
    }

    update();
}

void Tile::fillTileRect(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    QBrush brush;
    switch(state())
    {
    case Tile::Uncover:
    case Tile::Explode:
        brush = QBrush(option->palette.background());
        break;
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
        if(isUnderMouse())
            brush = QBrush(option->palette.color(QPalette::Active, QPalette::Midlight));
        else
            brush = QBrush(option->palette.color(QPalette::Active, QPalette::Button));
        break;
    }
    painter->fillRect(option->rect, brush);
}

void Tile::drawTileGrid(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    painter->save();
    QPen pen = painter->pen();
    pen.setWidth(1);
    pen.setColor(option->palette.background().color().darker());
    painter->setPen(pen);
    if(neighbour(Direction::Top))
        painter->drawLine(option->rect.topLeft(), option->rect.topRight());
    if(neighbour(Direction::Left))
        painter->drawLine(option->rect.topLeft(), option->rect.bottomLeft());
    painter->restore();
}

void Tile::drawTileImage(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    QRectF tagRect(option->rect.x() + option->rect.width() / 4,
                   option->rect.y() + option->rect.height() / 4,
                   option->rect.width() / 2,
                   option->rect.height() / 2);
    QRectF explosionRect = option->rect;
    QRectF mineRect(option->rect.x() + option->rect.width() / 10,
                    option->rect.y() + option->rect.height() / 10,
                    option->rect.width() * 4 / 5,
                    option->rect.height() * 4 / 5);
    switch(state())
    {
    case Tile::Flag:
        painter->drawImage(tagRect, QImage(":/image/flag"));
        break;
    case Tile::Tag:
        painter->drawImage(tagRect, QImage(":/image/tag"));
        break;
    case Tile::Explode:
        painter->drawImage(explosionRect, QImage(":/image/explosion"));
        break;
    case Tile::Uncover:
        if(isMine())
            painter->drawImage(mineRect, QImage(":/image/mine"));
        break;
    case Tile::Cover:
        if((d->logic->getState() != MineSweeper::State::Running) && (isMine()))
            painter->drawImage(mineRect, QImage(":/image/mine"));
        break;
    }
}

void Tile::drawTileBoarder(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    painter->save();
    QPen pen = painter->pen();
    switch(state())
    {
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
    {
        pen.setWidth(3);
        QColor light = option->palette.background().color().lighter(1000);
        QColor shadow = option->palette.background().color().darker(200);
        bool pressed = isPressed(Qt::LeftButton)
                       || isPressed(Qt::MidButton);

        // draw light side of top and left
        pen.setColor(pressed?shadow:light);
        painter->setPen(pen);
        painter->drawLine(option->rect.topLeft() + QPoint(1, 1),
                          option->rect.topRight() + QPoint(-1, 1));
        painter->drawLine(option->rect.topLeft() + QPoint(1, 1),
                          option->rect.bottomLeft() + QPoint(1, -1));

        // draw shadow side of bottom and right
        pen.setColor(pressed?light:shadow);
        painter->setPen(pen);
        painter->drawLine(option->rect.topRight() + QPoint(-1, 3),
                          option->rect.bottomRight() + QPoint(-1, -1));
        painter->drawLine(option->rect.bottomLeft() + QPoint(3, -1),
                          option->rect.bottomRight() + QPoint(-1, -1));
    }
        break;
    case Tile::Explode:
    case Tile::Uncover:
        break;
    }
    painter->restore();
}

void Tile::drawTileText(QPainter* painter, const QStyleOptionGraphicsItem* option)
{
    painter->save();
    QPen pen = painter->pen();
    switch(state())
    {
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
    case Tile::Explode:
        break;
    case Tile::Uncover:
        switch(surroundingMines())
        {
        case 1:
            pen.setColor(Qt::blue);
            break;
        case 2:
            pen.setColor(Qt::green);
            break;
        case 3:
            pen.setColor(Qt::red);
            break;
        case 4:
            pen.setColor(Qt::darkBlue);
            break;
        case 5:
            pen.setColor(Qt::darkRed);
            break;
        case 6:
            pen.setColor(Qt::darkGreen);
            break;
        case 7:
            pen.setColor(Qt::darkGray);
            break;
        case 8:
            pen.setColor(Qt::black);
            break;
        }
        if(surroundingMines() == 0)
            break;
        painter->setPen(pen);
        painter->drawText(option->rect, Qt::AlignCenter,
                          QString::number(surroundingMines()));
        break;
    }
    painter->restore();
}
