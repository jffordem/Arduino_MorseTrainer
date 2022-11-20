/*  Morse code blinker.
 *  
 *  Hardware:
 *  Hardware is optional.  If you don't have it, then you can't control the WPM.
 *  
 *  Current controls are:
 *  Left encoder - adjust WPM
 *  Left button - repeat current word
 *  Right encoder - adjust dit duration
 *  Right button - pause
 *  
 *  Set the intial string below.  But for more fun open up the Serial terminal
 *  and enter new strings.  Note that currently it has a limit of 128 chars.
 *  
 *  The encoder wheel adjusts the blink speed but isn't calibrated to any particular
 *  words per minute or anything.
 *  
 *  Caution.  Making the program too big "bricks" the Arduino.  (See the unbricking sketch!)
 *  Going over about 50-60% of capacity seems to cause this. It's possible that some mismatch
 *  in expected specs vs. real specs could be causing this.  Or the boot loader.  Who knows.
 *  
 *  Thinking about adding a small speaker and switching to audio.  Not sure if active or passive
 *  speakers would be better.  I'm guessing the active ones are easier to use, but are shrill.
 *  
 *  Example: active buzzer - https://www.instructables.com/ACTIVE-BUZZER-WITH-ARDUINO-UNO-R3/
 *  
 *  Passive requires more hardware.  The speaker and a 100 ohm resistor.  Evidently there's a 
 *  tone() function to oscillate the signal.  However, there is also known iterference with PWM pins 3 & 11.
 *  
 *  Example; passive buzzer - https://create.arduino.cc/projecthub/SURYATEJA/use-a-buzzer-module-piezo-speaker-using-arduino-uno-89df45
 *  
 *  const int buzzerPin = 9; // board-dependent, we assume, needs to be PWM?
 *  void setup() { pinMode(buzzerPin, OUTPUT); }
 *  void loop() {
 *    tone(buzzerPin, 550);
 *    delay(500);
 *    noTone(buzzerPin, 550);
 *    delay(500);
 *  }
 */

#include <Scheduler.hpp>
#include <Clock.hpp>
#include <Led.hpp>
#include <Mapper.hpp>
#include <EncoderWheel.hpp>
#include <ButtonHandler.hpp>

#include "ElizaWords.hpp"

#include "MorseStream.hpp"
#include "MorseClock.hpp"
#include "Randomizer.hpp"

// BE SURE TO PICK THE RIGHT LAYOUT
// #include <BreadboardConfig.hpp>
#include <LeonardoConfig.hpp>

#define DEBUG

int charSpeed = 12; // Character speed in WPM, using 'PARIS' as the standard word.
int farnsworthSpeed = 10; // Spacing between letters and words.  Usually lower than character speed.
bool ledState = LOW;
int kochLevel = 0;

MainSchedule schedule;
RandomWords words(kochLevel);
// RepeatStream alphabet("a a a  b b b  c c c  d d d  e e e  f f f  g g g  h h h  i i i  j j j  k k k  l l l  m m m  n n n  o o o  p p p  q q q  r r r  s s s  t t t  u u u  v v v  w w w  x x x  y y y  z z z  ");

MorseStream message(words);
MorseClock morse(schedule, charSpeed, farnsworthSpeed, message, ledState);
DigitalLED led(schedule, ledState, Config.DefaultLed);

// EncoderControl<int> farnsworthControl(schedule, Config.Left.Encoder, farnsworthSpeed, 20, 20);
EncoderControl<int> kochControl(schedule, Config.Left.Encoder, kochLevel, KochLevels >> 1, KochLevels-1);
ButtonHandler replayButton(schedule, Config.Right.Button, &onReplayButtonPressed);
EncoderControl<int> speedControl(schedule, Config.Right.Encoder, charSpeed, 20, 20);
ToggleButton pauseButton(schedule, Config.Left.Button, morse);
Randomizer<int,int> randomizer(schedule, charSpeed, farnsworthSpeed);
Mapper<int,int> farnsworthLevel(schedule, charSpeed, farnsworthSpeed, 1, 20, 1, 15);

void onReplayButtonPressed() {
  // randomWords.replay();
}

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif
  randomSeed(analogRead(A0));
}

#ifdef DEBUG
Timer debugTimer(200);
#endif

void loop() {
#ifdef DEBUG
  if (debugTimer.expired()) {
    debugTimer.reset(200);
    // words.print();
    // words.next();
    // message.println();
    // message.next();
    Serial.print("KOCH LEVEL: ");
    Serial.print(kochLevel, DEC);
    Serial.print(" WPM: ");
    Serial.println(charSpeed, DEC);
  }
  schedule.poll();
#else
  schedule.poll();
#endif
}