/*
  Toy-Ikea-MicrowaveController
  Toy Ikea Microwave Controller

  Microware Controller (Arduino Based)
  for Ikea Kitchen DUKTIG
  LEDBERG LED spotlight
  SKaccess by Simon Kong
  
  www.skaccess.com
  ATTiny841

  ------------------------------------------------------------
  - Features
  increment timer by 5 sec from 0 to 60 sec
  increment timer by 10 sec from 1min to 5 min
  increment timer by 30 sec from 5min onwards to 51min
  press the rotary knob to start/pause countdown
  light will be on when countdown is active
  low tone buzzer noise when counting down to simulate the microwave being on
  opening the door while countdown is on will pause countdown
  when the door is open, light will be on
  when the countdown finishes, will display "End", if the door is not open,
    it will do the End melody again every 2 min, untill the door is open.
    this is to remind the user that there is still food in the microwave.
  
  V 1.0 - 30/06/2019 - Initial Release
  ------------------------------------------------------------
  - PCB Board Used
  https://www.skstore.com.au/electronics/pcb/Toy-Ikea-MicrowaveController

  https://github.com/SkullKill/Toy-Ikea-MicrowaveController-PCB

  more pictures in the wiki
  https://github.com/SkullKill/Toy-Ikea-MicrowaveController-PCB/wiki
  
  ------------------------------------------------------------
  - Code Used
  https://github.com/SkullKill/Toy-Ikea-MicrowaveController
  
  ------------------------------------------------------------
  - Video
  https://youtu.be/BQZ9P_Z0Fzc

  ------------------------------------------------------------
  - Product Used (included)
  15mm Aluminium Knob
  10mm Spacer/Standoff and M3 Screws
  
  ------------------------------------------------------------
  - Product Used (not included)
  
  Ikea DUKTIG Play kitchen, birch
  Article Number : 403.199.73
  https://www.ikea.com/au/en/catalog/products/40319973/

  Ikea LEDBERG LED spotlight, white, 4 pack
  Article Number : 103.517.33
  https://www.ikea.com/au/en/catalog/products/10351733/
  Note: the cable with the Dark Lines is the -ve/gnd cable. 
  But better to double check with a multimeter.

  magnetic door sensor (reed switch) [could also use a micro switch]
  3mm Acrylic sheet

  50mm L Angle Bracket
  https://www.bunnings.com.au/zenith-50mm-zinc-plated-angle-brackets-4-pack_p2760232

  various screws and bolts 
  4G X 12mm timber screws
  washer 5/32" / M4
  Bolts & Nuts 3/16" X 15mm / M5
  
  ------------------------------------------------------------
  Library Used
  
  https://github.com/SpenceKonde/ATTinyCore
  https://miguelpynto.github.io/ShiftDisplay/
  https://www.pjrc.com/teensy/td_libs_Encoder.html

  ------------------------------------------------------------
  Sketch uses 6850 bytes (90%) of program storage space. Maximum is 7552 bytes.
  Global variables use 226 bytes (44%) of dynamic memory, leaving 286 bytes for local variables. Maximum is 512 bytes.

*/

#include <ShiftDisplay.h>
#include "lib\pitches.h"
#include "lib\Encoder.h"

// uncomment to enable debug output, but there might not be enough memory on the ATtiny841
//#define DEBUG

#define BEEPER_PIN  8
#define LIGHT_PIN 9
#define DOOR_PIN 10

// shift register pins, for LCD display
#define LATCH_PIN 5
#define CLOCK_PIN 6
#define DATA_PIN 4

// initial welcome message
#define SCROLL_TIME 500

// encoder pins
#define ENCODER_A 3
#define ENCODER_B 7
#define ENCODER_BUTTON 0

#define DEBOUNCE_DELAY 50

unsigned long encoderButtonLastDebounceTime = 0;  // the last time the output pin was toggled
bool encoderButtonState = HIGH;
bool encoderButtonLastState = HIGH;

bool doorState = LOW;
bool lightState = LOW;

bool countdownEnable = false;  // will countdown when on
bool countdownCycle = false;  // will clear once timer reaches 0
unsigned long countdownStart;
bool countdownClear = true;  // will clear once 0, and door opens
unsigned long countdownFinished;

bool disableKnob = false;
int8_t positionKnob = 0;
int16_t durationKnob = 0; // timer duration in sec


// notes in the melody: when timer finishes
//int melody[] = {
//  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
//};
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
//int noteDurations[] = {
//  4, 8, 8, 4, 4, 4, 4, 4
//};
int noteDurations[] = {
  10, 10, 10
};


ShiftDisplay display(LATCH_PIN, CLOCK_PIN, DATA_PIN, COMMON_CATHODE, 4);
Encoder knob(ENCODER_B, ENCODER_A);

/*
 * Read knob changes, and increase/decrease timer accordingly
 */
void processKnob()
{
  if (disableKnob)
  {
    return;
  }
  int8_t positionKnobNew;
  positionKnobNew = knob.read();
  bool positive = true;
  int8_t stepts = positionKnobNew;
  // if change detected 
  if (positionKnobNew != positionKnob)
  {
    tone(BEEPER_PIN, NOTE_G3, 20);
    if (positionKnobNew < positionKnob)
    {
      positive = false;
      // offset if negative, else it will not reduce the correct value off the timer
      if (positionKnobNew >= 0)
      {
        stepts = stepts + 1;
      }
    }
    // if negative, bring everything back to 0. we do not want -ve timer values
    if (positionKnobNew < 0)
    {
      positionKnobNew = 0;
      knob.write(0);
      durationKnob = 0;
      stepts = 0;
    }
    // increment timer by 5 sec from 0 to 10 sec
    else if  (0 <= stepts && stepts <= 10)
    {
      if (positive)
      {
        durationKnob = durationKnob + 1;
      }
      else
      {
        durationKnob = durationKnob - 1;
      }
    }
    // increment timer by 10 sec from 1min to 5 min
    else if  (11 <= stepts)
    {
      positionKnobNew = 10;
      knob.write(10);
      durationKnob = 10;
      stepts = 10;
    }

#ifdef DEBUG
    Serial.print("Knob = ");
    Serial.print(positionKnobNew);
    Serial.println();
    Serial.print("TimeSec = ");
    Serial.print(durationKnob);
    Serial.println();
#endif

    positionKnob = positionKnobNew;
  }
}

/*
 * convert sec to MM:SS for use on the LCD Display
 */
int16_t secToMinSec(int16_t duration)
{
  int8_t minutes = duration / 60;
  int8_t seconds = duration % 60;
  return ((minutes*100))+(seconds);
}

/*
 * will display END if timer has finished, and door has not been open yet
 * else, will display timer value
 */
void processDisplay()
{
  if (!countdownClear && durationKnob == 0)
  {
    display.set("END");
  }
  else
  {
    display.set(secToMinSec(durationKnob));
    display.setDot(1, true);
  }
  display.show(); // show stored Display Value
}

/*
 * read and debounce Encoder Button
 */
void processEncoderButton()
{
  // read the state of the switch into a local variable:
  bool encoderButtonRead = digitalRead(ENCODER_BUTTON);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (encoderButtonRead != encoderButtonLastState)
  {
    encoderButtonLastDebounceTime = millis();
#ifdef DEBUG
    Serial.print("Encoder Button changed, millis");
    Serial.print(encoderButtonLastDebounceTime);
    Serial.println();
#endif
  }
  if ((millis() - encoderButtonLastDebounceTime) > DEBOUNCE_DELAY)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (encoderButtonRead != encoderButtonState)
    {
      encoderButtonState = encoderButtonRead;
      if (encoderButtonState == LOW)
      {
#ifdef DEBUG
        Serial.print("Encoder Button Pressed, toogle countdown");
        Serial.println();
#endif
        // toogle start/stop countdown
        tone(BEEPER_PIN, NOTE_C4, 20);
        if (countdownEnable)
        {
          countdownEnable = false;
        }
        else
        {
          // if door open, do not start countdown
          if (doorState)
          {
            tone(BEEPER_PIN, NOTE_DS8, 40);

          }
          // if door closed, safe to start countdown
          else
          {
            countdownEnable = true;
            countdownStart = millis();
          }
        }
      }
    }
  }
  encoderButtonLastState = encoderButtonRead;
}

/*
 * Read and do a crude 5ms debounce . relying more on the hardware debounce circuit.
 */
void processDoorSwitch()
{
  bool doorSwitchRead = digitalRead(DOOR_PIN);
  // if change detected, do a crude 5ms debounce
  if (doorSwitchRead != doorState)
  {
    delay(5);
    doorSwitchRead = digitalRead(DOOR_PIN);
    if (doorSwitchRead != doorState)
    {
      doorState = doorSwitchRead;
      // if door open
      if (doorState)
      {
        // turon on light
        lightState = HIGH;
        tone(BEEPER_PIN, NOTE_C4, 20);
        // pause timer is it was on
        if (countdownEnable)
        {
          countdownEnable = false;
        }
        // clear the "END" message if timer was already 0
        if (!countdownClear)
        {
          countdownClear = true;
        }
      }
      // if door close
      else
      {
        // turn off light
        lightState = LOW;
        tone(BEEPER_PIN, NOTE_C4, 20);
      }
    }
  }
}

/*
 * check and process all inputs
 */
void processInputs()
{
  processEncoderButton();
  processDoorSwitch();
}

/*
 * end timer melody
 */
void playMelody()
{
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    display.show();
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(BEEPER_PIN, melody[thisNote], noteDuration);
    display.show();
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

/*
 * check if countdown is enable, and do countdown
 */
void processCounddown()
{
  // if countdown is enable
  if (countdownEnable)
  {
    // if countdown has reached 0
    if (durationKnob == 0)
    {
      // turn off countdown, and clear/reset all values
      countdownEnable = false;
      if (countdownCycle)
      {
        disableKnob = false;
        knob.write(0);
        countdownCycle = false;
        durationKnob = 0;
        positionKnob = 0;
        countdownFinished = millis();
        // lights off
        lightState = LOW;
        playMelody();
        // turn off the repeat nag
        countdownClear = true;
      }
      else
      {
        // if already 0, pressing it make it go into sleep mode?
        // sleep mode?
        // not implemented since we are not running on battery
      }
    }
    // if timer is not 0, do the countdown 
    else
    {
      countdownCycle = true;
      disableKnob = true;
      countdownClear = false;
      // simulate the microwave on noise
      tone(BEEPER_PIN, NOTE_G2, 5);
      // lights on
      lightState = HIGH;
      // decrement timer ever 1 sec
      if ((millis() - countdownStart) > 1000)
      {
        countdownStart = millis();
        durationKnob = durationKnob - 1;
      }
    }

  }
  else
  {
    // if timer has finished it's cycle, but the door has not been open since then
    // it will play the END melody again every 2 min, to remind user that there is still food in it
    if (!countdownClear)
    {
      if ((millis() - countdownFinished) > (120000))
      {
        countdownFinished = millis();
        playMelody();
      }
    }
  }
}

/*
 * turn on line if required
 */
void processLight()
{
  if (lightState)
  {
    digitalWrite(LIGHT_PIN, HIGH);
  }
  else
  {
    digitalWrite(LIGHT_PIN, LOW);
  }
}

void setup() 
{
#ifdef DEBUG
  Serial.begin(9600);
//  Serial.println("TwoKnobs Encoder Test:");
#endif
  // put your setup code here, to run once:
  //encoder = new ClickEncoder(ENCODER_A, ENCODER_B, ENCODER_BUTTON, 4, HIGH);
  //encoder->setAccelerationEnabled(true);
  pinMode(ENCODER_BUTTON, INPUT);
  pinMode(DOOR_PIN, INPUT);
  pinMode(BEEPER_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // display version V1.00
  display.set("V100");
  display.setDot(1, true);
  display.show(SCROLL_TIME);
  
  display.show("Hi", SCROLL_TIME, ALIGN_CENTER);
  display.show("   S", SCROLL_TIME);
  display.show("  SO", SCROLL_TIME);
  display.show(" SOP", SCROLL_TIME);
  display.show("SOPH", SCROLL_TIME);
  display.show("OPHI", SCROLL_TIME);
  display.show("PHIE", SCROLL_TIME);
  display.show("HIE", SCROLL_TIME);
  display.show("IE", SCROLL_TIME);
  display.show("E", SCROLL_TIME);

  for (int i = 3; i > 0; i--) {
    display.show(i, 400, ALIGN_CENTER); // store number and show it for 400ms
    display.setDot(1, true); // add dot to stored number
    display.show(400); // show number with dot for 400ms
  }
  //display.set("GO"); // store "GO"
  tone(BEEPER_PIN, NOTE_C4, 400);
}


void loop() {
  
  // put your main code here, to run repeatedly:
  
  processKnob();
  processDisplay();
  processInputs();
  processCounddown();
  processLight();
}
