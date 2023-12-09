#ifndef ANTRUNNER_H
#define ANTRUNNER_H

#include <QThread>
#include <QPushButton>
#include <thread>
#include <QDebug>

#include "AntMover.h"

using Field = std::vector<std::vector<QPushButton*>>;

const QString whiteCellStyle = "QPushButton { background-color: #FFFFFF }";
const QString blackCellStyle = "QPushButton { background-color: #000000 }";

class AntRunner : public QThread {
    Q_OBJECT

public:

    AntRunner(Field &f, int speedStepMs, Route startDireciton, std::mutex &mutex)
        : isStart(false)
        , field(f)
        , speedStepMs(speedStepMs)
        , mutex(mutex)
    {
        antMover = createAntMover(startDireciton);
    }

    void run() override {
        isStart = true;
        while (isStart) {
            int x = antMover->getPosX();
            int y = antMover->getPosY();

            emit prevPosAnt(x, y);

            qDebug() << x << y << "why";

            mutex.lock();
            if (field[x][y]->styleSheet() == whiteCellStyle) {
                field[x][y]->setStyleSheet(blackCellStyle);
                qDebug() << "stepRight";
                if (!antMover->stepRight()) {
                    qDebug() << "stepRight failed";
                    emit antBreaked();
                    mutex.unlock();
                    break;
                }
                qDebug() << "stepRight ok";
            } else {
                field[x][y]->setStyleSheet(whiteCellStyle);
                qDebug() << "stepLeft";
                if (!antMover->stepLeft()) {
                    qDebug() << "stepLeft failed";
                    emit antBreaked();
                    mutex.unlock();
                    break;
                }
                qDebug() << "stepLeft ok";
            }
            mutex.unlock();

            x = antMover->getPosX();
            y = antMover->getPosY();

            emit nextPosAnt(x, y);

            std::this_thread::sleep_for(std::chrono::milliseconds(speedStepMs));
        }
    }

    void stop() {
        isStart = false;
    }

    bool isRunning() const {
        return isStart;
    }

    Route getDirection() const {
        return antMover->getDirection();
    }

    void changeField(Field &f, Route antDirection) {
        field = f;
        delete antMover;
        antMover = createAntMover(antDirection);
        emit nextPosAnt(antMover->getPosX(), antMover->getPosY());
    }

    AntMover *createAntMover(Route antDirection) {
        int posX = field.size() / 2;
        int posY = field.size() / 2;
        int border = field.size() - 1;
        return new AntMover(antDirection, posX, posY, border);
    }

    int getPosX() const {
        return antMover->getPosX();
    }

    int getPosY() const {
        return antMover->getPosY();
    }

    void setDirection(Route direction) {
        antMover->setDirection(direction);
    }

    void setAntSpeedStep(int ms) {
        speedStepMs = ms;
    }

signals:
    void prevPosAnt(int x, int y);
    void nextPosAnt(int x, int y);
    void antBreaked();

private:
    bool isStart;
    Field &field;
    int speedStepMs;
    AntMover *antMover;
    std::mutex &mutex;
};

#endif // ANTRUNNER_H
