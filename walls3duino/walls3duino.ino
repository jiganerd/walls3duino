#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Game.hpp"

// max framerate w/ fullscreen black/white memset() flashing test (w/ console output) is just under 12 FPS

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Game game(display.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);

// time it took to render the last frame, in milliseconds
unsigned long frameTimeMs = 1000; // (something reasonable before the first frame is rendered)

//Create variables for each button on the Joystick Shield to assign the pin numbers
char button0=3, button1=4, button2=5, button3=6;
char sel=2;

void setup()
{
  Serial.begin(9600);

  joypadSetup();

  // allow time for developer to attach the serial monitor
  //delay(5000);

  Serial.print("free RAM: ");
  Serial.println(freeRam());

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // address should be 0x3D for a standard 128x64 display
  {
    Serial.println(F("SSD1306 allocation failed"));
    while(1) {}
  }

  Serial.println(F("SSD1306 allocation success"));

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
}

void loop()
{
  joypadRead();
  game.ProcessFrame();
  display.display();
  updateFrameRate();
}

void joypadSetup(void)
{
  pinMode(sel, INPUT);
  digitalWrite(sel, HIGH);  // enable the pull-up resistor
  
  pinMode(button0, INPUT);
  digitalWrite(button0, HIGH);

  pinMode(button1, INPUT);
  digitalWrite(button1, HIGH);
  
  pinMode(button2, INPUT);
  digitalWrite(button2, HIGH);

  pinMode(button3, INPUT);
  digitalWrite(button3, HIGH);
}

void joypadRead(void)
{
  // range of 0 to 1024
  uint8_t joyX = analogRead(0);
  uint8_t joyY = analogRead(1);

  // 0 for pressed
  uint8_t joySel = digitalRead(sel);
  uint8_t rotateLeft = digitalRead(button0);
  uint8_t moveBackward = digitalRead(button1);
  uint8_t moveForward = digitalRead(button2);
  uint8_t rotateRight = digitalRead(button3);
  uint8_t strafeLeft = 0;
  uint8_t strafeRight = 0;

  //double moveSpeed = frameTimeMs * 300;
  //double rotSpeed = frameTimeMs * M_PI * 2;
  double moveSpeed = 10.0f;
  double rotSpeed = M_PI/16;
  
  if (moveForward == true)
      game.MoveCamera(moveSpeed);
  
  if (moveBackward == true)
      game.MoveCamera(-moveSpeed);
  
  if (rotateLeft == true)
      game.RotateCamera(rotSpeed);
  
  if (rotateRight == true)
      game.RotateCamera(-rotSpeed);
  
  if (strafeLeft == true)
      game.StrafeCamera(-moveSpeed);
  
  if (strafeRight == true)
      game.StrafeCamera(moveSpeed);
}

int freeRam() 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void updateFrameRate()
{
  static unsigned long last = millis();

  unsigned long now = millis();
  frameTimeMs = now - last;
  last = now;

  Serial.print("Frame time (ms): ");
  Serial.println(frameTimeMs);
}
