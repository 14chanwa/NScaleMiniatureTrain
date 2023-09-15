#include <RF24Module.h>
#include <Arduino.h>

#include <SPI.h>
#include "RF24.h"


#define CE_PIN 15
#define CSN_PIN 5
// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);


// an identifying device destination
// Let these addresses be used for the pair
uint8_t address[][6] = { "1Node", "2Node" };
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// to use different addresses on a pair of radios, we need a variable to

// uniquely identify which address this radio will use to transmit
bool radioNumber = 1;  // 0 uses address[0] to transmit, 1 uses address[1] to transmit

// For this example, we'll be using a payload containing
// a string & an integer number that will be incremented
// on every successful transmission.
// Make a data structure to store the entire payload of different datatypes
DataPackage payload;

RF24Module::RF24Module()
{

}


void RF24Module::setup()
{
    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
    }

    Serial.print(F("radioNumber = "));
    Serial.println((int)radioNumber);

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

    // to use ACK payloads, we need to enable dynamic payload lengths (for all nodes)
    radio.enableDynamicPayloads();  // ACK payloads are dynamically sized

    // Acknowledgement packets have no payloads by default. We need to enable
    // this feature for all nodes (TX & RX) to use ACK payloads.
    radio.enableAckPayload();

    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]);  // always uses pipe 0

    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]);  // using pipe 1

    // additional setup specific to the node's role
    radio.stopListening();                 // put radio in TX mode
}


static RF24Module* rf24ModuleSingleton = nullptr;

RF24Module* RF24Module::getRF24Module()
{
    if(rf24ModuleSingleton == nullptr){
        rf24ModuleSingleton = new RF24Module();
    }
    return rf24ModuleSingleton;
}

bool RF24Module::send(DataPackage dp)
{
    // copy payload to be sent
    payload = dp;

    // This device is a TX node
    unsigned long start_timer = micros();                  // start the timer
    bool report = radio.write(&payload, sizeof(payload));  // transmit & save the report
    unsigned long end_timer = micros();                    // end the timer

    if (report) {
      Serial.print(F("Transmission successful! "));  // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer);  // print the timer result
      Serial.println(F(" us."));
      uint8_t pipe;
      if (radio.available(&pipe)) {  // is there an ACK payload? grab the pipe number that received it
        DataPackage received;
        radio.read(&received, sizeof(received));  // get incoming ACK payload
        Serial.print(F(" Recieved "));
        Serial.print(radio.getDynamicPayloadSize());  // print incoming payload size
        Serial.print(F(" bytes on pipe "));
        Serial.println(pipe);  // print pipe number that received the ACK
      } else {
        Serial.println(F(" Recieved: an empty ACK packet"));  // empty ACK packet received
      }
    } else {
      Serial.println(F("Transmission failed or timed out"));  // payload was not delivered
    }
    return report;
}