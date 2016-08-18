#include "MineField.h"
#include "MineSweeper.h"

MineField::MineField(QWidget* parent)
    : QFrame(parent)
    , hoverPos(QPoint(-1, -1))
{
    mc = MineSweeper::instance();
    setMouseTracking(true);
}

MineField::~MineField()
{
}

void MineField::init()
{
    QFont f = font();
    f.setPixelSize(mc->TileSize * 4 / 5);
    setFont(f);
}

void MineField::started()
{
    QSize size = mc->getTileSize();
    setFixedSize(size.width() * mc->TileSize,
                 size.height() * mc->TileSize);
}

void MineField::success()
{
}

void MineField::explode()
{
}

void MineField::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QFont font = painter.font();
    font.setBold(true);
    painter.setFont(font);

    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);

    QVector<QVector<Tile> > tiles = mc->getTiles();


    for(auto row=tiles.cbegin();row!=tiles.cend();++row)
    {
        for(auto tile=row->cbegin();tile!=row->cend();++tile)
        {
            QRectF rect(tile->pos.x() * mc->TileSize,
                        tile->pos.y() * mc->TileSize,
                        mc->TileSize,
                        mc->TileSize);

            fillTileRect(painter, tile, rect);
            drawTileGrid(painter, tile, rect);
            drawTileImage(painter, tile, rect);
            drawTileBoarder(painter, tile, rect);
            drawTileText(painter, tile, rect);
        }
    }
}

void MineField::mouseMoveEvent(QMouseEvent* event)
{
    hoverPos = event->pos();
    update();
}

void MineField::mousePressEvent(QMouseEvent* event)
{
    if(button != Qt::NoButton)
        return;
    button = event->button();

    emit press();

    pressPos = event->pos();
    switch(button)
    {
    case Qt::LeftButton:
    case Qt::MidButton:
    case Qt::RightButton:
        mc->setPressed(QPoint(pressPos.x() / mc->TileSize,
                              pressPos.y() / mc->TileSize),
                       button, true);
        break;
    default:
        break;
    }

    update();
}

void MineField::mouseReleaseEvent(QMouseEvent* event)
{
    if(button != event->button())
        return;

    emit release();

    switch(button)
    {
    case Qt::LeftButton:
    case Qt::MidButton:
    case Qt::RightButton:
        mc->setPressed(QPoint(pressPos.x() / mc->TileSize,
                              pressPos.y() / mc->TileSize),
                       button, false);
        break;
    default:
        break;
    }
    button = Qt::NoButton;

    update();
}

void MineField::fillTileRect(QPainter& painter, const Tile* const tile, const QRectF& rect)
{
    QBrush brush;
    switch(tile->state)
    {
    case Tile::Uncover:
    case Tile::Explode:
        brush = QBrush(palette().background());
        break;
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
        if(rect.contains(hoverPos))
            brush = QBrush(palette().color(QPalette::Active, QPalette::Midlight));
        else
            brush = QBrush(palette().color(QPalette::Active, QPalette::Button));
        break;
    }
    painter.fillRect(rect, brush);
}

void MineField::drawTileGrid(QPainter& painter, const Tile* const tile, const QRectF& rect)
{
    painter.save();
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(palette().background().color().darker());
    painter.setPen(pen);
    if(tile->neighbours[1])
        painter.drawLine(rect.topLeft(), rect.topRight());
    if(tile->neighbours[3])
        painter.drawLine(rect.topLeft(), rect.bottomLeft());
    painter.restore();
}

void MineField::drawTileImage(QPainter& painter, const Tile* const tile, const QRectF& rect)
{
    QRectF tagRect(rect.x() + rect.width() / 4,
                   rect.y() + rect.height() / 4,
                   rect.width() / 2,
                   rect.height() / 2);
    QRectF explosionRect = rect;
    QRectF mineRect(rect.x() + rect.width() / 10,
                    rect.y() + rect.height() / 10,
                    rect.width() * 4 / 5,
                    rect.height() * 4 / 5);
    switch(tile->state)
    {
    case Tile::Flag:
        painter.drawImage(tagRect, QImage(":/image/flag"));
        break;
    case Tile::Tag:
        painter.drawImage(tagRect, QImage(":/image/tag"));
        break;
    case Tile::Explode:
        painter.drawImage(explosionRect, QImage(":/image/explosion"));
        break;
    case Tile::Uncover:
        if(tile->isMine)
            painter.drawImage(mineRect, QImage(":/image/mine"));
        break;
    case Tile::Cover:
        if((mc->getState() != MineSweeper::State::Running) && (tile->isMine))
            painter.drawImage(mineRect, QImage(":/image/mine"));
        break;
    }
}

void MineField::drawTileBoarder(QPainter& painter, const Tile* const tile, const QRectF& rect)
{
    painter.save();
    QPen pen = painter.pen();
    switch(tile->state)
    {
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
    {
        pen.setWidth(3);
        QColor light = palette().background().color().lighter(1000);
        QColor shadow = palette().background().color().darker(200);
        bool pressed = tile->pressed.value(Qt::LeftButton)
                       || tile->pressed.value(Qt::MidButton);

        // draw light side of top and left
        pen.setColor(pressed?shadow:light);
        painter.setPen(pen);
        painter.drawLine(rect.topLeft() + QPoint(1, 1),
                         rect.topRight() + QPoint(-1, 1));
        painter.drawLine(rect.topLeft() + QPoint(1, 1),
                         rect.bottomLeft() + QPoint(1, -1));

        // draw shadow side of bottom and right
        pen.setColor(pressed?light:shadow);
        painter.setPen(pen);
        painter.drawLine(rect.topRight() + QPoint(-1, 3),
                         rect.bottomRight() + QPoint(-1, -1));
        painter.drawLine(rect.bottomLeft() + QPoint(3, -1),
                         rect.bottomRight() + QPoint(-1, -1));
    }
        break;
    case Tile::Explode:
    case Tile::Uncover:
        break;
    }
    painter.restore();
}

void MineField::drawTileText(QPainter& painter, const Tile* const tile, const QRectF& rect)
{
    painter.save();
    QPen pen = painter.pen();
    switch(tile->state)
    {
    case Tile::Cover:
    case Tile::Flag:
    case Tile::Tag:
    case Tile::Explode:
        break;
    case Tile::Uncover:
        switch(tile->count)
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
        if(tile->count == 0)
            break;
        painter.setPen(pen);
        painter.drawText(rect, Qt::AlignCenter,
                         QString::number(tile->count));
        break;
    }
    painter.restore();
}

void MineField::drawBorder(QPainter& painter)
{
    painter.save();
//    painter.fillRect(rect(), QBrush(Qt::white));
    QPen pen = painter.pen();
    pen.setColor(palette().color(QPalette::Normal, QPalette::Base));
    painter.setPen(pen);
    painter.drawLine(0, 0, width(), 0);
    painter.drawLine(width(), 0, width(), height());
    painter.drawLine(width(), height(), 0, height());
    painter.drawLine(0, height(), 0, 0);
    painter.restore();
}
