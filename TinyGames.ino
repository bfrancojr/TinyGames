
// Game Selector

#include <SPI.h>
#include <Wire.h>

#include "TinyGames.h"
#include <EEPROM.h>

#define CODEBENDER 0

static const gameEntry games[] = {
  { "Pong",		pong,		pongLoop },
  { "Flappy Birds",	flappy,		flappyLoop },
  { "Snake",		snake,		snakeLoop }
};

#define NUM_GAMES (sizeof(games) / sizeof(gameEntry))

// Globals
SSD1331 display = SSD1331(0);
JoyStick joystick;
byte lineBuffer[148];

static char gameSelected = -1;
static char currentGame = 0;

void setup()
{
  Wire.begin();
  display.begin();
  menu();
}

void splash(const char *text, byte col)
{
  display.cls();
  display.setFont(liberationSans_12ptFontInfo);
  display.fontColor(BLUE, BLACK);
  display.setCursor(col, 20);
  display.print(text);
  delay(1000);
}

void displayMenu(const char *title, byte col, const char *text)
{
  display.cls();
  display.setFont(liberationSans_12ptFontInfo);
  display.fontColor(WHITE, BLACK);
  display.setCursor(col, 1);
  display.print(title);
  display.setFont(liberationSans_8ptFontInfo);
  display.setCursor(1, 21);
  display.fontColor(GREEN, BLACK);
  display.print(F("Up/Down: Change"));
  display.setCursor(1, 34);
  display.print(F("Rigth/Left: Select"));
  display.fontColor(YELLOW, BLACK);
  display.setCursor(1, 50);
  display.print(F("--> "));
  display.print(text);
}

static void menu()
{
  displayMenu("Tiny Games", 5, games[currentGame].name);
}

void gameOver()
{
  blink(1, 25, "GAME OVER", YELLOW);
  delay(1000);
}

void loop()
{
  getJoystick();

  if (gameSelected == -1)
  {
    int LY = joystick.LY;
    int LX = joystick.LX;
    int RY = joystick.RY;
    int RX = joystick.RX;

    if (abs(LY) > 200 || abs(RY) > 200)
    {
      char prevGame = currentGame;
      currentGame += (LY > 0) ? -1 : 1;

      if (currentGame < 0) {
        currentGame = NUM_GAMES - 1;
      }
      if (currentGame >= NUM_GAMES) {
        currentGame = 0;
      }

      if (prevGame != currentGame) {
        menu();
      }

      do {
        delay(50);
        getJoystick();
      } while (abs(LY - joystick.LY) < 20 && abs(RY - joystick.RY) < 20);
      delay(200);
      return;
    }

    if (abs(LX) > 200 || abs(RX) > 200)
    {
      delay(200);
      gameSelected = currentGame;
      games[gameSelected].setup();
    }
    return;
  }

  if (joystick.LB && joystick.RB)
  {
    gameSelected = -1;
    currentGame = 0;
    menu();
    do
    {
      delay(10);
      getJoystick();
    }
    while (joystick.LB || joystick.RB);
    return;
  }

  if (games[gameSelected].loop()) {
    gameSelected = -1;
    currentGame = 0;
    menu();
  }
}

void getJoystick()
{
  Wire.requestFrom(0x22, 6);
  int data[4];
  for (int i = 0; i < 4; i++)
  {
    data[i] = Wire.read();
  }
  byte lsb = Wire.read();
  byte buttons = ~Wire.read();
  joystick.LB = (buttons & 4) != 0;
  joystick.RB = (buttons & 8) != 0;
  for (int i = 0; i < 4; i++)
  {
    data[i] <<= 2;
    data[i]  |= ((lsb >> (i * 2)) & 3);
    data[i]  -= 511;
  }
  joystick.RX =  data[0];
  joystick.RY = -data[1];
  joystick.LX = -data[2];
  joystick.LY =  data[3];
}

void displayInt(int x, int y, int val)
{
  char buf[6];

  display.setCursor(x, y);
  display.print(itoa(val, buf, 10));
}

void blink(int x, int y, const char *text, int color)
{
  for (int i = 0; i < 2; i++)
  {
    display.setCursor(x, y);
    display.fontColor(BLACK, color);
    display.print(text);
    delay(150);
    display.setCursor(x, y);
    display.fontColor(color, BLACK);
    display.print(text);
    delay(150);
  }
  display.fontColor(GREEN, BLACK);
}

void blinkInt(int x, int y, int val, int color)
{
  char buf[6];

  blink(x, y, itoa(val, buf, 10), color);
}

byte readEEPROMByte(int address)
{
  byte value;

#if CODEBENDER  
  value = EEPROM.read(address);
#else
  EEPROM.get(address, value);
#endif
  return value;
}

void writeEEPROMByte(int address, byte value)
{
#if CODEBENDER  
  EEPROM.write(address, value);
#else
  EEPROM.put(address, value);
#endif
}

int readEEPROMInt(int address)
{
  return readEEPROMByte(address) + 256 * readEEPROMByte(address+1);
}

void writeEEPROMInt(int address, int value)
{
  writeEEPROMByte(address, value % 256);
  writeEEPROMByte(address+1, value / 256);
}

int readHighScore()
{
  static byte MAGIC_NUMBER = 0xa5;
  int highScore;
  byte ec = readEEPROMByte(HSC_ADDRESS);
  
  if (ec == MAGIC_NUMBER) {
    highScore = readEEPROMInt(HSV_ADDRESS);
  } else {
    highScore = 0;
    writeEEPROMInt(HSV_ADDRESS, highScore);
    writeEEPROMByte(HSC_ADDRESS, MAGIC_NUMBER);
  }
  return highScore;
}

void writeHighScore(int highScore)
{
  writeEEPROMInt(HSV_ADDRESS, highScore);
}

