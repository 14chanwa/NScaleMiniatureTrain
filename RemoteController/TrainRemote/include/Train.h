#ifndef TRAIN_H
#define TRAIN_H
#include <Arduino.h>
#include <DataPackage.h>


class Train
{
    public:
        Train(uint8_t id);
        DataPackage hardStop();
        DataPackage softStop();
        // speed_increment can be negative
        DataPackage incrementSpeed(int speed_increment);
        DataPackage setSpeed(uint8_t speed, TrainDirection direction = TrainDirection::FORWARD);
        DataPackage toggleFunction(TrainFunction function, bool activated);

        uint8_t id_;
        TrainDirection train_direction_;
        uint8_t train_steps_number_;
        uint8_t train_speed_;
};

#endif