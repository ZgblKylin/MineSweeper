#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <QtWidgets>

class Tile;
class MineSweeper;
class MineField : public QFrame
{
    Q_OBJECT

public:
    explicit MineField(QWidget* parent = 0);
    ~MineField();

    Q_SIGNAL void press();
    Q_SIGNAL void release();

    void init();
    void started();
    void success();
    void explode();

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    void fillTileRect(QPainter& painter, const QSharedPointer<Tile> tile, const QRectF& rect);
    void drawTileGrid(QPainter& painter, const QSharedPointer<Tile> tile, const QRectF& rect);
    void drawTileImage(QPainter& painter, const QSharedPointer<Tile> tile, const QRectF& rect);
    void drawTileBoarder(QPainter& painter, const QSharedPointer<Tile> tile, const QRectF& rect);
    void drawTileText(QPainter& painter, const QSharedPointer<Tile> tile, const QRectF& rect);
    void drawBorder(QPainter& painter);

    MineSweeper* logic;
    QGraphicsScene scene;
    Qt::MouseButton button = Qt::NoButton;
    QPoint pressPos;
};

#endif // MINEFIELD_H
