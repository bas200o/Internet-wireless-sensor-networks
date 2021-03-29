#include <Arduino.h>

//define buttons
#define BTNLEFT D4
#define BTNRIGHT D5

//define LEDS left
#define LRED D1
#define LYELLOW D2
#define LGREEN D3

//define LEDS right
#define RRED D0
#define RYELLOW D7
#define RGREEN D6

#define YELLOWTIME 2000

#define TIMERTIME 5000

int timer = TIMERTIME;

bool leftActive = false;
bool rightActive = false;

void clearAllLEDs()
{
  digitalWrite(RRED, LOW);
  digitalWrite(RYELLOW, LOW);
  digitalWrite(RGREEN, LOW);
  digitalWrite(LRED, LOW);
  digitalWrite(LYELLOW, LOW);
  digitalWrite(LGREEN, LOW);
}

void SwitchToLeft()
{
  if (!leftActive)
  {
    leftActive = true;
    rightActive = false;
    clearAllLEDs();
    digitalWrite(LRED, HIGH);
    digitalWrite(RYELLOW, HIGH);
    delay(YELLOWTIME);
    digitalWrite(RYELLOW, LOW);
    digitalWrite(RRED, HIGH);
    digitalWrite(LRED, LOW);
    digitalWrite(LGREEN, HIGH);
    timer = TIMERTIME;
  }
}

void switchToRight()
{
  if (!rightActive)
  {
    leftActive = false;
    rightActive = true;
    clearAllLEDs();
    digitalWrite(LYELLOW, HIGH);
    digitalWrite(RRED, HIGH);
    delay(YELLOWTIME);
    digitalWrite(LYELLOW, LOW);
    digitalWrite(LRED, HIGH);
    digitalWrite(RGREEN, HIGH);
    digitalWrite(RRED, LOW);
    timer = TIMERTIME;
  }
}

void switchToRed()
{
  leftActive = false;
  rightActive = false;
  clearAllLEDs();
  digitalWrite(RRED, HIGH);
  digitalWrite(LRED, HIGH);
}

void waitState()
{
  while (timer > 1)
  {
    timer--;
    delay(1);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(D5, INPUT);
  pinMode(D4, INPUT);
  pinMode(LRED, OUTPUT);
  pinMode(LYELLOW, OUTPUT);
  pinMode(LGREEN, OUTPUT);
  pinMode(RRED, OUTPUT);
  pinMode(RYELLOW, OUTPUT);
  pinMode(RGREEN, OUTPUT);

  clearAllLEDs();
  switchToRed();
}

void loop()
{
  if (digitalRead(BTNLEFT) == false)
  {
    waitState();
    SwitchToLeft();
  }

  if (digitalRead(BTNRIGHT) == false)
  {
    waitState();
    switchToRight();
  }

  timer--;
  delay(1);
}
