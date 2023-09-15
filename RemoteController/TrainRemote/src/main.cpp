#include <Arduino.h>
#include <WiFi.h>
#include <Screen.h>
#include <RF24Module.h>
#include <Commanders.hpp>
#include <KeyPad.h>

#define LED_PWM_CHANNEL 0U
#define LED_BUILTIN_HIGH_LEVEL 30U

#define ENCODER_DT 36U
#define ENCODER_CLK 35U
#define ENCODER_SW 39U

#define KEYPAD_R1 4U
#define KEYPAD_R2 13U
#define KEYPAD_R3 27U
#define KEYPAD_R4 33U

#define SR_LOAD 25U
#define SR_CLOCK 26U
#define SR_DATA 32U

Board board;
Screen screen(&board);
RF24Module* rf24Module(RF24Module::getRF24Module());

ButtonsCommanderEncoder EncoderRelative;
ButtonsCommanderPush Push_Encoder;
KeyPad keypad(0);
void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData);

byte serialRegister1;
byte serialRegister2;

void task_update_screen(void* parameters) {
  for (;;)
  {
    screen.update();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void task_blink(void* parameters)
{
  int delay = 1000;

  // ledcAttachPin(LED_BUILTIN, LED_PWM_CHANNEL);
  // ledcSetup(LED_PWM_CHANNEL, 1000, 8);
  pinMode(LED_BUILTIN, OUTPUT);

  for(;;)
  {
    // ledcWrite(LED_PWM_CHANNEL, LED_BUILTIN_HIGH_LEVEL);  // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_BUILTIN, HIGH);
    // Serial.println("tick");
    vTaskDelay(delay / portTICK_PERIOD_MS);                      // wait for a second
    // ledcWrite(LED_PWM_CHANNEL, 0U);   // turn the LED off by making the voltage LOW
    digitalWrite(LED_BUILTIN, LOW);
    // Serial.println("tock");
    vTaskDelay(delay / portTICK_PERIOD_MS);                      // wait for a second
  }
}

void readSR(uint8_t rowId)
{

  uint8_t rowPin;
  switch(rowId)
    {
      case 0:
        rowPin = KEYPAD_R1;
        break;
      case 1:
        rowPin = KEYPAD_R2;
        break;
      case 2:
        rowPin = KEYPAD_R3;
        break;
      case 3:
        rowPin = KEYPAD_R4;
        break;
      default:
        break;
    };

  // enable row
  digitalWrite(rowPin, HIGH);  

  digitalWrite(SR_LOAD, LOW);
  delayMicroseconds(5);
  digitalWrite(SR_LOAD, HIGH);
  delayMicroseconds(5);

  // Get data from 74HC165
  digitalWrite(SR_CLOCK, HIGH);
  serialRegister1 = shiftIn(SR_DATA, SR_CLOCK, LSBFIRST);
  serialRegister2 = shiftIn(SR_DATA, SR_CLOCK, LSBFIRST);

  bool col1 = bitRead(serialRegister1, 5);
  bool col2 = bitRead(serialRegister1, 4);
  bool col3 = bitRead(serialRegister1, 3);
  bool col4 = bitRead(serialRegister1, 2);

  keypad.updateState(rowId, 0, col1);
  keypad.updateState(rowId, 1, col2);
  keypad.updateState(rowId, 2, col3);
  keypad.updateState(rowId, 3, col4);

  // disable row
  digitalWrite(rowPin, LOW);
}

void task_read_sr(void* parameters)
{
  uint8_t i = 0;
  // alternate reading rows
  for(;;)
  {
    if (i > 3)
    {
      i = 0;
    }
    readSR(i);
    i++;
  }

  vTaskDelay(10 / portTICK_PERIOD_MS);
}

void setup() {
  Serial.begin(115200);

  std::shared_ptr<Train > train0(new Train(47));
  board.trains.push_back(train0);

  std::shared_ptr<Train > train1(new Train(3));
  board.trains.push_back(train1);

  std::shared_ptr<Train > train2(new Train(74));
  board.trains.push_back(train2);
  
  screen.setup();
  rf24Module->setup();
  delay(1000);

  // WiFi.mode(WIFI_AP_STA);
  // delay(1000);

  // Commanders
  Commanders::begin(ReceiveEvent, LED_BUILTIN);
  EncoderRelative.begin(30, ENCODER_CLK, ENCODER_DT);
  Push_Encoder.begin(51, ENCODER_SW);

  // Keypad
  pinMode(KEYPAD_R1, OUTPUT);
  pinMode(KEYPAD_R2, OUTPUT);
  pinMode(KEYPAD_R3, OUTPUT);
  pinMode(KEYPAD_R4, OUTPUT);

  digitalWrite(KEYPAD_R1, LOW);
  digitalWrite(KEYPAD_R2, LOW);
  digitalWrite(KEYPAD_R3, LOW);
  digitalWrite(KEYPAD_R4, LOW);

  // Setup 74HC165 connections
  pinMode(SR_LOAD, OUTPUT);
  pinMode(SR_CLOCK, OUTPUT);
  pinMode(SR_DATA, INPUT);

  xTaskCreate(
    task_update_screen,
    "task_update_screen",
    10000,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    task_blink,
    "task_blink",
    10000,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    task_read_sr,
    "task_read_sr",
    10000,
    NULL,
    1,
    NULL
  );
}


int increment = 10;

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
  Serial.println("Received event: " + String(inId) + " " + String(inEventType) + " " + String(inEventData));

  // // Power on/off
  // if (inId == 100) {
  //   if (DCCpp::IsPowerOnMain || DCCpp::IsPowerOnProg) 
  //     DCCpp::powerOff();
  //   else
  //     DCCpp::powerOn();

  //   update_screen_time();
  //   return;
  // }

  // // Panic stop
  // if (inId == 105) {
  //   for (int index = 0; index < locoNumber; index++) {
  //     locos[index].stop();
  //   }
  //   update_screen_time();
  //   return;
  // }

  // // Cycle loco
  // if (inId == 110) {
  //   locoIndex +=1;
  //   locoIndex = locoIndex % locoNumber;
  //   update_screen_time();
  //   return;
  // }
  
  // // Toggle auto
  // if (inId == 115) {
  //   return;
  // }
  
  if (inId == 30) {
    // Encoder turned
    
    if (inEventData > 0) {
      // Knob turned CW
      rf24Module->send(
        board.getSelectedTrain()->incrementSpeed(increment)
      );
    } else if (inEventData < 0) {
      // Knob turned CCW
      rf24Module->send(
        board.getSelectedTrain()->incrementSpeed(-increment)
      );
    }    
  } else if (inId == 51) {
    // Encoder button pressed
    
    // Stop the loco without changing direction
    rf24Module->send(
      board.getSelectedTrain()->softStop()
    );
  } else if (inId < 16) {
    // Keypad pressed

    // select train
    if (inId == 3) {
      board.selectTrain(0);
    } else if (inId == 7) {
      board.selectTrain(1);
    } else if (inId == 11) {
      board.selectTrain(2);
    } else if (inId == 15) {
      board.selectTrain(3);
    }

    else if (inId == 12) {
      rf24Module->send(board.powerOn());
    } else if (inId == 13) {
      rf24Module->send(board.powerOff());
    }
  }
}


void loop() {
  Commanders::loop();
  vTaskDelay(1 / portTICK_PERIOD_MS);

  // buttons are located at positions 

  // rf24Module->send(
  //   board.getSelectedTrain()->incrementSpeed(increment)
  // );
  // if (board.getSelectedTrain()->train_speed_ == 127)
  // {
  //   increment = -increment;
  // }
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.getSelectedTrain()->softStop()
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.emergencyStop()
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.getSelectedTrain()->hardStop()
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.powerOff()
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.powerOn()
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // rf24Module->send(
  //   board.getSelectedTrain()->toggleFunction(TrainFunction::F0, true)
  // );
  // vTaskDelay(1000 / portTICK_PERIOD_MS);

  // // WiFi.scanNetworks will return the number of networks found
  // int n = WiFi.scanNetworks();
  // Serial.println("scan done");
  // if (n == 0) {
  //     Serial.println("no networks found");
  // } else {
  //   Serial.print(n);
  //   Serial.println(" networks found");
  //   for (int i = 0; i < n; ++i) {
  //     // Print SSID and RSSI for each network found
  //     Serial.print(i + 1);
  //     Serial.print(": ");
  //     Serial.print(WiFi.SSID(i));
  //     Serial.print(" (");
  //     Serial.print(WiFi.RSSI(i));
  //     Serial.print(")");
  //     Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
  //     delay(10);
  //   }
  // }
  // vTaskDelay(1000 / portTICK_PERIOD_MS);
}