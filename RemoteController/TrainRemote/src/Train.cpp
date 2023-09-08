#include <Train.h>

Train::Train(uint8_t id) : id_(id)
{
    train_direction_ = TrainDirection::FORWARD;
    train_steps_number_ = 128;
    train_speed_ = 0;
};


DataPackage Train::hardStop()
{
    train_speed_ = 0;

    DataPackage dp;
    dp.command_type = CommandType::TRAIN_HARDSTOP;
    dp.target_id = id_;
    return dp;
}

DataPackage Train::softStop()
{
    return setSpeed(0);
}

DataPackage Train::setSpeed(uint8_t speed, TrainDirection direction)
{
    train_direction_ = direction;
    // Truncate speed to max steps number
    train_speed_ = speed >= train_steps_number_ ? train_steps_number_ - 1 : speed;

    DataPackage dp;
    dp.command_type = CommandType::TRAIN_COMMAND;
    dp.target_id = id_;
    dp.train_direction = train_direction_;
    dp.train_speed = train_speed_;
    return dp;
}

DataPackage Train::incrementSpeed(int speed_increment)
{
    int newSpeedInt = (train_direction_ == TrainDirection::BACKWARD ? -1 : 1) * train_speed_ + speed_increment;
    TrainDirection newDirection = newSpeedInt < 0 ? TrainDirection::BACKWARD : TrainDirection::FORWARD;
    uint8_t newSpeed = abs(newSpeedInt) > CHAR_MAX ? CHAR_MAX : (uint8_t)abs(newSpeedInt);

    return setSpeed(newSpeed, newDirection);
}

DataPackage Train::toggleFunction(TrainFunction function)
{
    DataPackage dp;
    dp.command_type = CommandType::TRAIN_FUNCTION;
    dp.target_id = id_;
    dp.train_function = function;
    return dp;
}