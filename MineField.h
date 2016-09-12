#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <QtWidgets>

class Tile;
class MineSweeper;
class MineField : public QGraphicsView
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
    void mouseMoveEvent(QMouseEvent* event) override final;
    void mousePressEvent(QMouseEvent* event) override final;
    void mouseReleaseEvent(QMouseEvent* event) override final;

private:
    MineSweeper* logic;
    QGraphicsScene scene;
    Qt::MouseButton button = Qt::NoButton;
    QPoint pressPos;
};

#endif // MINEFIELD_H
