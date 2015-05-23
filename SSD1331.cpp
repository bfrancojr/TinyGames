/*
SSD1331.cpp

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Written by Ben Rose for TinyCircuits.

The latest version of this library can be found at https://tiny-circuits.com/
*/

#include "SSD1331.h"
#include <avr/pgmspace.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>
#include <Wire.h>

void SSD1331::writeGPIO(uint8_t regAddr, uint8_t regData)
{
  uint8_t oldTWBR = TWBR;
  TWBR = 0;
  Wire.beginTransmission(0x20);
  Wire.write(regAddr);
  Wire.write(regData);
  Wire.endTransmission();
  TWBR = oldTWBR;
}

void SSD1331::startCommand(void)
{
  writeGPIO(GPIO_RegData, GPIO_CMD_START);
}

void SSD1331::startData(void)
{
  writeGPIO(GPIO_RegData, GPIO_DATA_START);
}

void SSD1331::endTransfer(void)
{
  writeGPIO(GPIO_RegData, GPIO_TRANSFER_END);
}

uint8_t SSD1331::getButtons(void)
{
  Wire.beginTransmission(0x20);
  Wire.write(GPIO_RegData);
  Wire.endTransmission();
  Wire.requestFrom(0x20, 1);
  uint8_t b = Wire.read();
  //buttons are active low and MSBs, so flip+shift
  return ((~b) >> 4) & 0x0F;
}

void SSD1331::goTo(uint8_t x, uint8_t y)
{
  if (x > xMax || y > yMax)
  {
    return;
  }
  setX(x, xMax);
  setY(y, yMax);
}

void SSD1331::setX(uint8_t x, uint8_t end)
{
  if (x > xMax)
  {
    x = xMax;
  }
  if (end > xMax)
  {
    end = xMax;
  }
  startCommand();
  SPI.transfer(0x15); //set column
  SPI.transfer(x);
  SPI.transfer(end);
  endTransfer();
}

void SSD1331::setY(uint8_t y, uint8_t end)
{
  if (y > yMax)
  {
    y = yMax;
  }
  if (end > yMax)
  {
    end = yMax;
  }
  startCommand();
  SPI.transfer(0x75); //set row
  SPI.transfer(y);
  SPI.transfer(end);
  endTransfer();
}

void SSD1331::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t f, uint8_t color)
{
  uint8_t r = (color & 0x03) << 4; //two bits
  uint8_t g = (color & 0x1C) << 1; //three bits
  uint8_t b = (color & 0xE0) >> 2; //three bits
  if (r & 0x10)
  {
    r |= 0x0F; // carry lsb
  }
  if (g & 0x08)
  {
    g |= 0x07; // carry lsb
  }
  if (b & 0x08)
  {
    b |= 0x07; // carry lsb
  }
  drawRect(x, y, w, h, f, r, g, b);
}

void SSD1331::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t f, uint8_t r, uint8_t g, uint8_t b)
{
  if (x > xMax || y > yMax)
  {
    return;
  }
  uint8_t x2 = x + w - 1;
  uint8_t y2 = y + h - 1;
  if (x2 > xMax)
  {
    x2 = xMax;
  }
  if (y2 > yMax)
  {
    y2 = yMax;
  }

  uint8_t fill = (f != 0);

  startCommand();
  SPI.transfer(0x26); // set fill
  SPI.transfer(fill);

  SPI.transfer(0x22); //draw rectangle
  SPI.transfer(x);
  SPI.transfer(y);
  SPI.transfer(x2);
  SPI.transfer(y2);
  // outline
  SPI.transfer(b);
  SPI.transfer(g);
  SPI.transfer(r);
  // fill
  SPI.transfer(b);
  SPI.transfer(g);
  SPI.transfer(r);
  endTransfer();
}

void SSD1331::clearWindow(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  if (x > xMax || y > yMax)
  {
    return;
  }
  uint8_t x2 = x + w - 1;
  uint8_t y2 = y + h - 1;
  if (x2 > xMax)
  {
    x2 = xMax;
  }
  if (y2 > yMax)
  {
    y2 = yMax;
  }

  startCommand();
  SPI.transfer(0x25); // clear window
  SPI.transfer(x);
  SPI.transfer(y);
  SPI.transfer(x2);
  SPI.transfer(y2);
  endTransfer();
}

void SSD1331::cls()
{
  clearWindow(0, 0, xMax + 1, yMax + 1);
  setCursor(0, 0);
}

void SSD1331::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{
  drawLine(x0, y0, x1, y1, (color & 0xE0) >> 2, (color & 0x1c) << 1, (color & 0x03) << 4);
}

void SSD1331::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t r, uint8_t g, uint8_t b)
{
  if (x0 > xMax)
  {
    x0 = xMax;
  }
  if (y0 > yMax)
  {
    y0 = yMax;
  }
  if (x1 > xMax)
  {
    x1 = xMax;
  }
  if (y1 > yMax)
  {
    y1 = yMax;
  }
  startCommand();
  SPI.transfer(0x21); // draw line
  SPI.transfer(x0);
  SPI.transfer(y0);
  SPI.transfer(x1);
  SPI.transfer(y1);
  SPI.transfer(b);
  SPI.transfer(g);
  SPI.transfer(r);
  endTransfer();
}


void SSD1331::drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  if (x > xMax || y > yMax)
  {
    return;
  }
  goTo(x, y);
  writePixel(color);
}

void SSD1331::writePixel(uint8_t color)
{
  startData();
  SPI.transfer(color);
  endTransfer();
}

void SSD1331::writeBuffer(uint8_t *buffer, int count)
{
  byte temp;
  SPDR = buffer[0];
  for (int j = 1; j < count; j++)
  {
    temp = buffer[j];
    while (!(SPSR & _BV(SPIF)));
    SPDR = temp;
  }
  while (!(SPSR & _BV(SPIF)));
}

void SSD1331::setBrightness(uint8_t brightness)
{
  if (brightness > 15)
  {
    brightness = 15;
  }
  startCommand();
  SPI.transfer(0x87); // set master current
  SPI.transfer(brightness);
  endTransfer();
}

void SSD1331::on(void)
{
  startCommand();
  SPI.transfer(0xAF); // display on
  endTransfer();
}
void SSD1331::off(void)
{
  startCommand();
  SPI.transfer(0xAE); // display off
  endTransfer();
}

void SSD1331::setBitDepth(uint8_t b)
{
  _bitDepth = b;
  writeRemap();
}

void SSD1331::setFlip(uint8_t f)
{
  _flipDisplay = f;
  writeRemap();
}

void SSD1331::writeRemap(void)
{
  uint8_t remap = 0x04;
  if (_flipDisplay)
  {
    remap |= 0x32;
  }
  else
  {
    remap |= 0x20;
  }
  if (_bitDepth)
  {
    remap |= (1 << 6);
  }
  startCommand();
  SPI.transfer(0xA0); // set remap
  SPI.transfer(remap);
  endTransfer();
}

void SSD1331::begin(void)
{
  //setup GPIO, reset SSD1331
  writeGPIO(GPIO_RegData, ~GPIO_RES); // reset low, CS/other pins high
  writeGPIO(GPIO_RegDir, ~GPIO_RES); // set reset to output
  delay(5);
  writeGPIO(GPIO_RegDir, ~(GPIO_CS | GPIO_DC)); //reset to input, CS/DC output
  writeGPIO(GPIO_RegPullUp, GPIO_BTN1 | GPIO_BTN2 | GPIO_BTN3 | GPIO_BTN4); // button pullup enable
  //init SPI
  SPI.begin();
  SPI.setDataMode(SPI_MODE0); // wrong mode, works because we're only writing. compatible with SD cards.
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  //datasheet SSD1331 init sequence
  uint8_t init[32] = { 0xAE, 0xA1, 0x00, 0xA2, 0x00, 0xA4, 0xA8, 0x3F,
                       0xAD, 0x8E, 0xB0, 0x0B, 0xB1, 0x31, 0xB3, 0xF0, 0x8A, 0x64, 0x8B,
                       0x78, 0x8C, 0x64, 0xBB, 0x3A, 0xBE, 0x3E, 0x81, 0x91, 0x82, 0x50, 0x83, 0x7D
                     };
  off();
  startCommand();
  for (uint8_t i = 0; i < 32; i++)
    SPI.transfer(init[i]);
  endTransfer();
  // use libarary functions for remaining init
  setBrightness(5);
  writeRemap();
  clearWindow(0, 0, 96, 64);
  delay(2);
  on();
}

SSD1331::SSD1331(uint8_t addr)
{
  _addr = addr;
  _cursorX = 0;
  _cursorY = 0;
  _fontHeight = 0;
  _fontFirstCh = 0;
  _fontLastCh = 0;
  _fontDescriptor = 0;
  _fontBitmap = 0;
  _fontColor = 0xFF;
  _fontBGcolor = 0x00;
  _bitDepth = 0;
  _flipDisplay = 0;
}

void SSD1331::setCursor(uint8_t x, uint8_t y)
{
  _cursorX = x;
  _cursorY = y;
}

void SSD1331::setFont(const FONT_INFO& fontInfo)
{
  _fontHeight = fontInfo.height;
  _fontFirstCh = fontInfo.startCh;
  _fontLastCh = fontInfo.endCh;
  _fontDescriptor = fontInfo.charDesc;
  _fontBitmap = fontInfo.bitmap;
}

void SSD1331::fontColor(uint8_t f, uint8_t g)
{
  _fontColor = f;
  _fontBGcolor = g;
}

size_t SSD1331::write(uint8_t ch)
{
  if (!_fontFirstCh)
  {
    return 1;
  }
  if (ch < _fontFirstCh || ch > _fontLastCh)
  {
    return 1;
  }
  if (_cursorX > xMax || _cursorY > yMax)
  {
    return 1;
  }
  uint8_t chWidth = pgm_read_byte(&_fontDescriptor[ch - _fontFirstCh].width);
  uint8_t bytesPerRow = chWidth / 8;
  if (chWidth > bytesPerRow * 8)
  {
    bytesPerRow++;
  }
  uint16_t offset = pgm_read_word(&_fontDescriptor[ch - _fontFirstCh].offset) + (bytesPerRow * _fontHeight) - 1;

  setX(_cursorX, _cursorX + chWidth + 1);
  setY(_cursorY, _cursorY + _fontHeight);

  startData();
  for (uint8_t y = 0; y < _fontHeight && y + _cursorY < yMax + 1; y++)
  {
    SPDR = _fontBGcolor;
    for (uint8_t byte = 0; byte < bytesPerRow; byte++)
    {
      uint8_t data = pgm_read_byte(_fontBitmap + offset - y - ((bytesPerRow - byte - 1) * _fontHeight));
      uint8_t bits = byte * 8;
      for (uint8_t i = 0; i < 8 && (bits + i) < chWidth && (bits + i + _cursorX) < xMax; i++)
      {
        while (!(SPSR & _BV(SPIF)));
        if (data & (0x80 >> i))
        {
          SPDR = _fontColor;
        }
        else
        {
          SPDR = _fontBGcolor;
        }
      }
    }
    while (!(SPSR & _BV(SPIF)));
    if ((_cursorX + chWidth) < xMax)
    {
      SPI.transfer(_fontBGcolor);
    }
  }
  endTransfer();
  _cursorX += (chWidth + 1);
  return 1;
}
