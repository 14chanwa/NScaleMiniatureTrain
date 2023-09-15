#include <Board.h>

Train* Board::getSelectedTrain()
{
    if (!trains.empty())
    {
        return trains.at(selected_train).get();
    }
    return NULL;
}


void Board::selectTrain(uint8_t trainIndex)
{
    if (trainIndex < trains.size())
    {
        selected_train = trainIndex;
    }
}

DataPackage Board::powerOn()
{
    DataPackage dp;
    dp.command_type = CommandType::POWER_ON;
    return dp;
}

DataPackage Board::powerOff()
{
    DataPackage dp;
    dp.command_type = CommandType::POWER_OFF;
    return dp;
}

DataPackage Board::emergencyStop()
{
    DataPackage dp;
    dp.command_type = CommandType::EMERGENCY_STOP;
    return dp;
}