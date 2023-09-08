#ifndef SCREEN_H
#define SCREEN_H
#include <Arduino.h>
#include <Board.h>

class Screen
{
    public:
        Screen(Board* board);
        void setup();
        void update();

    private:
        Board* board_;
        void printTrain(Train* train);

        char heartbeat_;
        long lastHeartbeatMillis_;
        void printHeartbeat();
};

#endif