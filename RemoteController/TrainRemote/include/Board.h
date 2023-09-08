#ifndef BOARD_H
#define BOARD_H
#include <Arduino.h>
#include <vector>
#include <memory>

#include <Train.h>
#include <Point.h>

class Board
{
    public:
        Train* getSelectedTrain();

        uint8_t selected_train;
        std::vector<std::shared_ptr<Train> > trains;
        std::vector<std::shared_ptr<Point> > points;
};

#endif