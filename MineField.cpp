#include "MineField.h"
#include "MineSweeper.h"

MineField::MineField(QWidget* parent)
    : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(&scene);
    logic = MineSweeper::instance();
}

MineField::~MineField()
{
    Q_FOREACH(auto item, scene.items())
        scene.removeItem(item);
}

void MineField::init()
{
    QFont f = font();
    f.setPixelSize(Tile::size() * 4 / 5);
    setFont(f);
}

void MineField::started()
{
    setEnabled(true);

    QSize size = logic->getTileSize();
    setFixedSize(size.width() * Tile::size(),
                 size.height() * Tile::size());

    Q_FOREACH(auto item, scene.items())
        scene.removeItem(item);
    auto tiles = logic->getTiles();
    scene.setSceneRect(0,
                       0,
                       size.width() * Tile::size(),
                       size.height() * Tile::size());
    Q_FOREACH(auto row, tiles)
    {
        Q_FOREACH(auto tile, row)
        {
            scene.addItem(tile.data());
            tile->setPos(tile->index().x() * Tile::size(),
                         tile->index().y() * Tile::size());
        }
    }
}

void MineField::success()
{
    setEnabled(false);
}

void MineField::explode()
{
    setEnabled(false);
}

void MineField::mouseMoveEvent(QMouseEvent* event)
{
    auto tile = dynamic_cast<Tile*>(scene.itemAt(event->pos(), QTransform()));
    if(!tile)
        return;
    if(event->buttons().testFlag(Qt::MidButton))
    {
        logic->moveHover(tile->index());
        viewport()->update();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MineField::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    if(event->button() == Qt::MidButton)
    {
        auto tile = dynamic_cast<Tile*>(scene.itemAt(event->pos(), QTransform()));
        logic->moveHover(tile->index());
    }
    emit press();
    viewport()->repaint();
}

void MineField::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    if(event->button() == Qt::MidButton)
    {
        auto tile = dynamic_cast<Tile*>(scene.itemAt(event->pos(), QTransform()));
        logic->setPressed(tile->index(), Qt::MidButton, false);
    }
    emit release();
    viewport()->repaint();
}
