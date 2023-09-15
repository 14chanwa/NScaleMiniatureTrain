#include <KeyPad.h>
#include <Commanders.hpp>

// ids : from first to first + 15, rows first then columns
KeyPad::KeyPad(uint8_t firstId) : firstId_(firstId)
{

}


void KeyPad::updateState(uint8_t rowId, uint8_t colId, bool state)
{
    bool oldValue;
    if (rowId < 2)
    {
        oldValue = bitRead(oldKeypadState1_, rowId * 4 + colId);
    }
    else
    {
        oldValue = bitRead(oldKeypadState2_, (rowId-2) * 4 + colId);
    }

    if (oldValue != state)
    {
        // only toggle on pressed (no unpress)
        if (state)
        {
            Commanders::RaiseEvent(firstId_ + rowId * 4 + colId, COMMANDERS_EVENT_TOGGLE, state);
        }

        // save last state
        if (rowId < 2)
        {
            bitWrite(oldKeypadState1_, rowId * 4 + colId, state);
        }
        else
        {
            bitWrite(oldKeypadState2_, (rowId-2) * 4 + colId, state);
        }
    }

    

}