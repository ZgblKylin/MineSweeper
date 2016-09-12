#include "MineSweeper.h"
#include <QtWidgets>
#include <utility>

Q_GLOBAL_STATIC(MineSweeper, mc)

MineSweeper::MineSweeper()
{
    setObjectName(QStringLiteral("mineSweep"));

    settings = new QSettings(QDir(qApp->applicationDirPath()).absoluteFilePath("MineSweep.ini"),
                             QSettings::IniFormat, dynamic_cast<QObject*>(this));

    // function for loading ranklist with given level
    auto loadRank = [this](Difficulty level)
    {
        QList<QVariantList> list;
        settings->beginReadArray(QStringLiteral("Rank"));
        for(int i=0;i<10;++i)
        {
            settings->setArrayIndex(i);
            QVariantList value;
            int r = settings->value(QStringLiteral("rank"), i+1).toInt();
            QString name = settings->value(QStringLiteral("name"),
                                           QStringLiteral("anoymous"))
                           .toString();
            qreal time = settings->value(QStringLiteral("time"),
                                         999).toReal();
            value << r << name << time;
            list.append(value);
        }
        settings->endArray();
        ranklist[level] = list;
    };

    // load simple ranklist
    settings->beginGroup(QStringLiteral("Simple"));
    loadRank(Difficulty::Simple);
    settings->endGroup();

    // load normal ranklist
    settings->beginGroup(QStringLiteral("Normal"));
    loadRank(Difficulty::Normal);
    settings->endGroup();

    // load hard ranklist
    settings->beginGroup(QStringLiteral("Hard"));
    loadRank(Difficulty::Hard);
    settings->endGroup();
}

MineSweeper::~MineSweeper()
{
    // function for saving ranklist with given level
    auto saveRank = [this](Difficulty level)
    {
        QList<QVariantList> list = ranklist[level];
        settings->beginWriteArray(QStringLiteral("Rank"));
        for(int i=0;i<10;++i)
        {
            settings->setArrayIndex(i);
            QVariantList value = list.at(i);
            settings->setValue(QStringLiteral("rank"), value.at(0).toInt());
            settings->setValue(QStringLiteral("name"), value.at(1).toString());
            settings->setValue(QStringLiteral("time"), value.at(2).toReal());
        }
        settings->endArray();
    };

    // save simple ranklist
    settings->beginGroup(QStringLiteral("Simple"));
    saveRank(Difficulty::Simple);
    settings->endGroup();

    // save normal ranklist
    settings->beginGroup(QStringLiteral("Normal"));
    saveRank(Difficulty::Normal);
    settings->endGroup();

    // save hard ranklist
    settings->beginGroup(QStringLiteral("Hard"));
    saveRank(Difficulty::Hard);
    settings->endGroup();

    // save ranklist
    settings->sync();
}

MineSweeper* MineSweeper::instance()
{
    return mc;
}

void MineSweeper::init(QMainWindow* mainWindow, QSize minimumSize)
{
    QSize size = qApp->desktop()->availableGeometry(mainWindow).size() * 0.9;
    size -= minimumSize;

    screenHorizontal = (size.width() >= size.height());
    if(!screenHorizontal)
        std::swap(columnRange, rowRange);
}

bool MineSweeper::isScreenHorizontal() const
{
    return screenHorizontal;
}

const QVector<QVector<QSharedPointer<Tile> > > MineSweeper::getTiles() const
{
    return tiles;
}

MineSweeper::Difficulty MineSweeper::getDifficulty() const
{
    return difficulty;
}

MineSweeper::State MineSweeper::getState() const
{
    return state;
}

QSize MineSweeper::getTileSize() const
{
    return tileSize;
}

int MineSweeper::getMaxMineCount() const
{
    return maxMineCount;
}

int MineSweeper::getMineCount() const
{
    return mineCount;
}

int MineSweeper::getRank() const
{
    return rank;
}

QList<QVariantList> MineSweeper::getRank(MineSweeper::Difficulty lvl) const
{
    return ranklist[lvl];
}

const QPoint MineSweeper::getColumnRange() const
{
    return columnRange;
}

const QPoint MineSweeper::getRowRange() const
{
    return rowRange;
}

qreal MineSweeper::getTime() const
{
    static qreal time;
    if(state == MineSweeper::State::Running)
        time = timer.elapsed() / static_cast<qreal>(1000);
    return time;
}

void MineSweeper::startGame(MineSweeper::Difficulty lvl, QSize size, int mines)
{
    difficulty = lvl;
    state = MineSweeper::State::Running;

    // init columns, rows, mines. columns is larger
    int col = 0;
    int row = 0;
    switch(difficulty)
    {
    case MineSweeper::Difficulty::Simple:
        col = 10;
        row = 10;
        maxMineCount = 10;
        break;
    case MineSweeper::Difficulty::Normal:
        col = 16;
        row = 16;
        maxMineCount = 40;
        break;
    case MineSweeper::Difficulty::Hard:
        col = 30;
        row = 16;
        maxMineCount = 99;
        break;
    case MineSweeper::Difficulty::Custom:
        col = size.width();
        row = size.height();
        maxMineCount = mines;
        break;
    }
    if(col < row)
        std::swap(col, row);

    // if screen is vertical, swap columns and rows
    if(!screenHorizontal)
        std::swap(col, row);
    tileSize = QSize(col, row);

    // initialize tiles
    tiles.clear();
    tiles.resize(row);
    for(int r=0;r<row;++r)
    {
        tiles[r] = QVector<QSharedPointer<Tile> >(col);
        for(int c=0;c<col;++c)
        {
            tiles[r][c] = QSharedPointer<Tile>::create();
            tiles.at(r).at(c)->setIndex(QPoint(c, r));
        }
    }

    // initialize tile neighbours
    for(int r=0;r<row;++r)
    {
        for(int c=0;c<col;++c)
        {
            QSharedPointer<Tile> tile = tiles.at(r).at(c);
            for(int i=0;i<8;++i)
            {
                Direction direction = static_cast<Direction>(i);
                int nc = c + Directions.value(direction).x();
                int nr = r + Directions.value(direction).y();
                if(((nc >= 0) && (nc < col))
                   && ((nr >= 0) && (nr < row)))
                {
                    QSharedPointer<Tile> neighbour = tiles.at(nr).at(nc);
                    tile->setNeighbour(direction, neighbour);
                }
            }
        }
    }

    // initialize mines
    mineCount = maxMineCount;
    while(mineCount > 0)
    {
        int r = qrand()%row;
        int c = qrand()%col;
        QSharedPointer<Tile> tile = tiles.at(r).at(c);
        if(tile->isMine())
            continue;
        tile->setIsMine(true);
        --mineCount;
    }
    mineCount = maxMineCount;

    // initialize tile surrounding mine count
    for(int r=0;r<row;++r)
    {
        for(int c=0;c<col;++c)
        {
            QSharedPointer<Tile> tile = tiles.at(r).at(c);
            quint8 count = tile->surroundingMines();
            for(int i=0;i<8;++i)
            {
                Direction direction = static_cast<Direction>(i);
                int nc = c + Directions.value(direction).x();
                int nr = r + Directions.value(direction).y();
                if(((nc >= 0) && (nc < col))
                   && ((nr >= 0) && (nr < row)))
                {
                    QSharedPointer<Tile> neighbour = tiles.at(nr).at(nc);
                    count += (neighbour->isMine()?1:0);
                }
            }
            tile->setSurroundingMines(count);
        }
    }

    emit update();
    timer.restart();
}

bool MineSweeper::isPressed(const QPoint& index, Qt::MouseButton button) const
{
    QSharedPointer<Tile> tile = tiles.at(index.y()).at(index.x());
    return tile->isPressed(button);
}

void MineSweeper::setPressed(const QPoint& index, Qt::MouseButton button, bool pressed)
{
    QSharedPointer<Tile> tile = tiles.at(index.y()).at(index.x());

    switch(button)
    {
    case Qt::MidButton:
        for(int i=0;i<8;++i)
        {
            Direction direction = static_cast<Direction>(i);
            QSharedPointer<Tile> neighbour = tile->neighbour(direction);
            if(neighbour)
                neighbour->setPressed(button, pressed);
        }
        break;
    case Qt::LeftButton:
    case Qt::RightButton:
    default:
        break;
    }
    tile->setPressed(button, pressed);
}

void MineSweeper::click(const QPoint& index, Qt::MouseButton button)
{
    QSharedPointer<Tile> tile = tiles.at(index.y()).at(index.x());
    if(tile->isPressed(button))
    {
        QSharedPointer<Tile> tile = tiles.at(index.y()).at(index.x());
        switch(button)
        {
        case Qt::LeftButton:
            leftClick(tile);
            break;
        case Qt::MidButton:
            midClick(tile);
            break;
        case Qt::RightButton:
            rightClick(tile);
            break;
        default:
            break;
        }
    }
}

void MineSweeper::moveHover(const QPoint& index)
{
    Q_FOREACH(auto row, tiles)
    {
        Q_FOREACH(auto tile, row)
            tile->setPressed(Qt::MidButton, false);
    }
    setPressed(index, Qt::MidButton, true);
}

void MineSweeper::leftClick(QSharedPointer<Tile> tile)
{
    if(state != MineSweeper::State::Running)
        return;

    bool exploded = uncover(tile);
    if(exploded)
    {
        state = MineSweeper::State::Fail;
        calcRank();
        emit explode();
    }
    else
    {
        checkSuccess();
    }

    emit update();
}

void MineSweeper::midClick(QSharedPointer<Tile> tile)
{
    if(state != MineSweeper::State::Running)
        return;

    if(tile->state() != Tile::Uncover)
        return;

    int mineCount = tile->surroundingMines();
    int count = 0;

    for(int i=0;i<8;++i)
    {
        Direction direction = static_cast<Direction>(i);
        QSharedPointer<Tile> neighbour = tile->neighbour(direction);
        if(!neighbour)
            continue;
        if(neighbour->state() == Tile::Flag)
            ++count;
    }

    if(count != mineCount)
        return;

    bool exploded = uncover(tile);
    for(int i=0;i<8;++i)
    {
        Direction direction = static_cast<Direction>(i);
        QSharedPointer<Tile> neighbour = tile->neighbour(direction);
        if(!neighbour)
            continue;
        if(neighbour->state() == Tile::Cover)
            exploded = exploded || uncover(neighbour);
    }

    if(exploded)
    {
        state = MineSweeper::State::Fail;
        calcRank();
        emit explode();
    }
    else
    {
        checkSuccess();
    }
}

void MineSweeper::rightClick(QSharedPointer<Tile> tile)
{
    if(state != MineSweeper::State::Running)
        return;

    switch(tile->state())
    {
    case Tile::Cover:
        tile->setState(Tile::Flag);
        mineCount -= 1;
        break;
    case Tile::Flag:
        tile->setState(Tile::Tag);
        mineCount += 1;
        break;
    case Tile::Tag:
        tile->setState(Tile::Cover);
        break;
    case Tile::Explode:
    case Tile::Uncover:
        break;
    }
    checkSuccess();

    emit update();
}

bool MineSweeper::uncover(QSharedPointer<Tile> tile)
{
    // already uncovered
    if(tile->state() != Tile::Cover)
        return false;

    // uncover tile
    tile->setState(Tile::Uncover);

    // detect mine
    if(tile->isMine())
    {
        tile->setState(Tile::Explode);
        return true;
    }

    // if surrounding mine count is not zero, do not uncover neighbours
    if(tile->surroundingMines() != 0)
        return false;

    bool exploded = false;
    for(int i=0;i<8;++i)
    {
        Direction direction = static_cast<Direction>(i);
        QSharedPointer<Tile> neighbour = tile->neighbour(direction);
        if(neighbour)
            exploded = (exploded || uncover(neighbour));
    }
    return exploded;
}

void MineSweeper::calcRank()
{
    qreal time = getTime();
}

void MineSweeper::checkSuccess()
{
    bool onlyMineLeft = true;
    Q_FOREACH(auto row, tiles)
    {
        Q_FOREACH(auto tile, row)
        {
            if((tile->state() != Tile::Uncover) && (!tile->isMine()))
                onlyMineLeft = false;
        }
    }

    if(onlyMineLeft)
    {
        state = MineSweeper::State::Success;
        calcRank();
        emit success();
    }
}
