#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_SSD1306_mod.h"
#include "Game.hpp"

// TODO:
// to be able to load bigger maps, switch to model of global vertices and connections
//   instead of repeating vertices (very big refactor)
// use lookup tables for trig. functions
// could attempt enabling larger maps by keeping BSP tree in flash instead of RAM, in the same format as is currently in RAM, 
//   if flash reading speed would allow for it
// could attempt to use a "compressed" array with each item representing a "from x to x" segment that has been drawn,
//   instead of flat "height buffer, for checking if the entire screen has been drawn or not (similar to Doom engine), in
//   case it speeds up checking of whether or not the entire screen has been drawn

// NOTE: max framerate w/ fullscreen/full pixel buffer black/white memset() flashing test (w/ console output)
// is just under 12 FPS

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306_mod display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Game* pGame;

// time it took to render the last frame, in milliseconds
unsigned long frameTimeMs = 1000; // (something reasonable before the first frame is rendered)

// pin numbers for buttons on the joystick shield
char button0 = 3, button1 = 4, button2 = 5, button3 = 6;
char sel = 2;

void onColRenderered();

void setup()
{
  Serial.begin(9600);

  joypadSetup();

  // allow time for developer to attach the serial monitor
  //delay(5000);

  Serial.println(F(""));
  Serial.print(F("free RAM before display init: "));
  Serial.println(freeRam());

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) // address should be 0x3D for a standard 128x64 display
  {
    Serial.println(F("SSD1306 allocation failed"));
    while (1) {}
  }

  Serial.println(F("SSD1306 allocation success"));

  Serial.print(F("free RAM after display init: "));
  Serial.println(freeRam());

  pGame = new Game(display.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, onColRenderered);
}

void loop()
{
  joypadRead();

  display.startDisplay();
  pGame->ProcessFrame();
  display.endDisplay();

  //Serial.print(F("free RAM in loop: "));
  //Serial.println(freeRam());

  updateFrameRate();
}

void onColRenderered()
{
  display.displayColumn();
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

  double moveSpeed = 0.05f * frameTimeMs;
  double rotSpeed = 0.0005f * M_PI * frameTimeMs;

  if (moveForward == true)
      pGame->MoveCamera(moveSpeed);
  
  if (moveBackward == true)
      pGame->MoveCamera(-moveSpeed);
  
  if (rotateLeft == true)
      pGame->RotateCamera(-rotSpeed);
  
  if (rotateRight == true)
      pGame->RotateCamera(rotSpeed);
  
  if (strafeLeft == true)
      pGame->StrafeCamera(-moveSpeed);
  
  if (strafeRight == true)
      pGame->StrafeCamera(moveSpeed);
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

  Serial.print(F("Frame time (ms): "));
  Serial.println(frameTimeMs);
}
