#include <Screen.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSans9pt7b.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//Set pins for I2C1
#define I2C1_SDA_PIN 17
#define I2C1_SCL_PIN 14

TwoWire twoWire(0);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &twoWire, OLED_RESET);

Screen::Screen(Board* board) : board_(board)
{

};

void Screen::setup()
{
  twoWire.setPins(I2C1_SDA_PIN, I2C1_SCL_PIN);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  Serial.println(F("SSD1306 allocation OK"));

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000);
  lastHeartbeatMillis_ = millis();
}

void Screen::update()
{
    printTrain(board_->getSelectedTrain());
    printHeartbeat();
    display.display();
}

void Screen::printTrain(Train* train)
{
    display.clearDisplay();

    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0,13);             // Start at top-left corner
    // display.println(F("Hello, world!"));
    // display.println(millis());

    display.print("Id: ");
    display.println(train->id_);
    display.print("Dir: ");
    display.println((train->train_direction_ == TrainDirection::FORWARD ? "FORW" : "BACK"));
    display.print("Spd: ");
    display.print(train->train_speed_);
    display.print(" / ");
    display.print(train->train_steps_number_);
}

void Screen::printHeartbeat()
{
    display.setTextSize(1);             // Normal 1:1 pixel scale
    display.setFont(&FreeSans9pt7b);
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(110,13);             // Start at top-left corner
    if (heartbeat_ % 4 == 0)
    {
        display.print("_");
    }
    else if (heartbeat_ % 4 == 2)
    {
        display.setCursor(110,0);
        display.print("_");
    }
    else if (heartbeat_ % 4 == 1)
    {
        display.print("/");
    }
    else {
        display.print("\\");
    }
    long currentMillis = millis();
    if (currentMillis - lastHeartbeatMillis_ > 5000)
    {
        heartbeat_++;
        lastHeartbeatMillis_ = currentMillis;
    }
}