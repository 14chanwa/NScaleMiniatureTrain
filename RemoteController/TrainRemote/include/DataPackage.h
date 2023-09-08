#ifndef DATA_PACKAGE_H
#define DATA_PACKAGE_H
#include <Arduino.h>


enum TrainDirection : bool
{
    BACKWARD = false,
    FORWARD = true
};

enum TrainFunction : uint8_t
{
    F0 = 0,
    F1 = 1,
    F2 = 2,
    F3 = 3
};

enum PointDirection : bool
{
    RIGHT = false,
    LEFT = true
};

// command types :
// 0 : power off
// 1 : power on
// 2 : emergency stop
// 3 : train hardstop
// 4 : train command
// 5 : train function
// 6 : point toggle
enum CommandType : uint8_t
{
    POWER_OFF = 0,
    POWER_ON = 1,
    EMERGENCY_STOP = 2,
    TRAIN_HARDSTOP = 3,
    TRAIN_COMMAND = 4,
    TRAIN_FUNCTION = 5,
    POINT_TOGGLE = 6
};

struct DataPackage
{
    CommandType command_type;
    uint8_t target_id;
    TrainDirection train_direction;
    uint8_t train_steps_number;
    uint8_t train_speed;
    TrainFunction train_function;
    PointDirection point_direction;
};

#endif