#include <Board.h>

Train* Board::getSelectedTrain()
{
    if (!trains.empty())
    {
        return trains.at(selected_train).get();
    }
    return NULL;
}