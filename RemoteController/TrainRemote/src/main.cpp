#include <Arduino.h>
#include <Screen.h>

Board board;
Screen screen(&board);

void setup() {
  Serial.begin(115200);

  std::shared_ptr<Train > train1(new Train(0));
  board.trains.push_back(train1);
  
  screen.setup();
}

int increment = 10;

void loop() {
  board.getSelectedTrain()->incrementSpeed(increment);
  screen.update();
  if (board.getSelectedTrain()->train_speed_ == 127)
  {
    increment = -increment;
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
}