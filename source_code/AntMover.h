#ifndef ANTMOVER_H
#define ANTMOVER_H

#include <mutex>

enum Route {
    kRight,
    kDown,
    kLeft,
    kUp
};

class AntMover {
public:
    AntMover(Route direction, int posX, int posY, int border)
        : direction(direction)
        , posX(posX)
        , posY(posY)
        , border(border)
    {}

    bool stepRight() {
        std::lock_guard<std::mutex> lock(mutex);

        bool stepDone = false;

        switch (direction) {
            case kRight:
                if (posX < border) {
                    ++posX;
                    direction = kDown;
                    stepDone = true;
                }
                break;

            case kDown:
                if (posY > 0) {
                    --posY;
                    direction = kLeft;
                    stepDone = true;
                }
                break;

            case kLeft:
                if (posX > 0) {
                    --posX;
                    direction = kUp;
                    stepDone = true;
                }
                break;

            case kUp:
                if (posY < border) {
                    ++posY;
                    direction = kRight;
                    stepDone = true;
                }
                break;
        }

        return stepDone;
    }

    bool stepLeft() {
        std::lock_guard<std::mutex> lock(mutex);

        bool stepDone = false;

        switch (direction) {
            case kRight:
                if (posX > 0) {
                    --posX;
                    direction = kUp;
                    stepDone = true;
                }
                break;

            case kUp:
                if (posY > 0) {
                    --posY;
                    direction = kLeft;
                    stepDone = true;
                }
                break;

            case kLeft:
                if (posX < border) {
                    ++posX;
                    direction = kDown;
                    stepDone = true;
                }
                break;

            case kDown:
                if (posY < border) {
                    ++posY;
                    direction = kRight;
                    stepDone = true;
                }
                break;
        }

        return stepDone;
    }

    void setDirection(Route direction) {
        std::lock_guard<std::mutex> lock(mutex);

        this->direction = direction;
    }

    Route getDirection() const {
        std::lock_guard<std::mutex> lock(mutex);

        return direction;
    }

    int getPosX() const {
        std::lock_guard<std::mutex> lock(mutex);

        return posX;
    }

    int getPosY() const {
        std::lock_guard<std::mutex> lock(mutex);

        return posY;
    }

private:
    Route direction;
    int posX;
    int posY;
    int border;
    mutable std::mutex mutex;
};

#endif // ANTMOVER_H
