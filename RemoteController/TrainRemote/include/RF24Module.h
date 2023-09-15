#ifndef RF24_MODULE_H
#define RF24_MODULE_H

#include <DataPackage.h>

class RF24Module
{
    private:
        RF24Module();
    public:
        void setup();
        static RF24Module* getRF24Module();

        bool send(DataPackage dp);
};

#endif