#ifndef _KEYPAD_H
#define _KEYPAD_H

#include <Arduino.h>

class KeyPad
{
    public:
        // ids : from first to first + 15, rows first then columns
        KeyPad(uint8_t firstId);
        void updateState(uint8_t rowId, uint8_t colId, bool state);
    
    private:
        uint8_t firstId_;
        byte oldKeypadState1_;
        byte oldKeypadState2_;
};

#endif