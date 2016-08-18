#include "MineSweeper.h"
#include <QtWidgets>
#include <utility>

Q_GLOBAL_STATIC(MineSweeper, mc);

const qreal MineSweeper::TileSize = 32;

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

const QVector<QVector<Tile> > MineSweeper::getTiles() const
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

QList<QVariantList> MineSweeper::getRank(Difficulty lvl) const
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
    if(state == State::Running)
        time = timer.elapsed() / static_cast<qreal>(1000);
    return time;
}

void MineSweeper::startGame(Difficulty lvl, QSize size, int mines)
{
    difficulty = lvl;
    state = State::Running;

    // init columns, rows, mines. columns is larger
    int col = 0;
    int row = 0;
    switch(difficulty)
    {
    case Difficulty::Simple:
        col = 10;
        row = 10;
        maxMineCount = 10;
        break;
    case Difficulty::Normal:
        col = 16;
        row = 16;
        maxMineCount = 40;
        break;
    case Difficulty::Hard:
        col = 30;
        row = 16;
        maxMineCount = 99;
        break;
    case Difficulty::Custom:
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
    tiles.resize(row);
    for(int r=0;r<row;++r)
    {
        tiles[r] = QVector<Tile>(col, Tile());
        for(int c=0;c<col;++c)
            tiles[r][c].pos = QPoint(c, r);
    }

    // initialize tile neighbours
    for(int r=0;r<row;++r)
    {
        for(int c=0;c<col;++c)
        {
            Tile* tile = &tiles[r][c];
            for(int i=0;i<8;++i)
            {
                int nc = c + Direction[i].x();
                int nr = r + Direction[i].y();
                if(((nc >= 0) && (nc < col))
                   && ((nr >= 0) && (nr < row)))
                {
                    Tile* neighbour = &tiles[nr][nc];
                    tile->neighbours[i] = neighbour;
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
        Tile* tile = &tiles[r][c];
        if(tile->isMine)
            continue;
        tile->isMine = true;
        --mineCount;
    }
    mineCount = maxMineCount;

    // initialize tile surrounding mine count
    for(int r=0;r<row;++r)
    {
        for(int c=0;c<col;++c)
        {
            Tile* tile = &tiles[r][c];
            for(int i=0;i<8;++i)
            {
                int nc = c + Direction[i].x();
                int nr = r + Direction[i].y();
                if(((nc >= 0) && (nc < col))
                   && ((nr >= 0) && (nr < row)))
                {
                    Tile* neighbour = &tiles[nr][nc];
                    tile->count += (neighbour->isMine?1:0);
                }
            }
        }
    }

    emit update();
    timer.restart();
}

void MineSweeper::setPressed(const QPoint& pos, Qt::MouseButton button, bool pressed)
{
    Tile* tile = &tiles[pos.y()][pos.x()];

    switch(button)
    {
    case Qt::LeftButton:
        break;
    case Qt::MidButton:
        for(Tile* neighbour : tile->neighbours)
        {
            if(neighbour)
                neighbour->pressed[button] = pressed;
        }
        break;
    case Qt::RightButton:
        break;
    default:
        break;
    }

    if(tile->pressed.value(button) && !pressed)
    {
        Tile* tile = &tiles[pos.y()][pos.x()];
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
    tile->pressed[button] = pressed;
}

void MineSweeper::leftClick(Tile* tile)
{
    if(state != State::Running)
        return;

    bool exploded = uncover(tile);
    if(exploded)
    {
        state = State::Fail;
        calcRank();
        emit explode();
    }
    else
    {
        checkSuccess();
    }

    emit update();
}

void MineSweeper::midClick(Tile* tile)
{
    if(state != State::Running)
        return;

    if(tile->state != Tile::Uncover)
        return;

    int mineCount = tile->count;
    int count = 0;

    for(Tile* neighbour : tile->neighbours)
    {
        if(!neighbour)
            continue;
        if(neighbour->state == Tile::Flag)
            ++count;
    }

    if(count != mineCount)
        return;

    bool exploded = uncover(tile);
    for(Tile* neighbour : tile->neighbours)
    {
        if(!neighbour)
            continue;
        if(neighbour->state == Tile::Cover)
            exploded = exploded || uncover(neighbour);
    }

    if(exploded)
    {
        state = State::Fail;
        calcRank();
        emit explode();
    }
    else
    {
        checkSuccess();
    }
}

void MineSweeper::rightClick(Tile* tile)
{
    if(state != State::Running)
        return;

    switch(tile->state)
    {
    case Tile::Cover:
        tile->state = Tile::Flag;
        mineCount -= 1;
        break;
    case Tile::Flag:
        tile->state = Tile::Tag;
        mineCount += 1;
        break;
    case Tile::Tag:
        tile->state = Tile::Cover;
        break;
    case Tile::Explode:
    case Tile::Uncover:
        break;
    }
    checkSuccess();

    emit update();
}

bool MineSweeper::uncover(Tile* tile)
{
    // already uncovered
    if(tile->state != Tile::Cover)
        return false;

    // uncover tile
    tile->state = Tile::Uncover;

    // detect mine
    if(tile->isMine)
    {
        tile->state = Tile::Explode;
        return true;
    }

    // if surrounding mine count is not zero, do not uncover neighbours
    if(tile->count != 0)
        return false;

    bool exploded = false;
    for(Tile* neighbour : tile->neighbours)
    {
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
            if((tile.state != Tile::Uncover) && (!tile.isMine))
                onlyMineLeft = false;
        }
    }

    if(onlyMineLeft)
    {
        state = State::Success;
        calcRank();
        emit success();
    }
}
