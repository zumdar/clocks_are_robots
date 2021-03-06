//-----------------------------------------------------
// Engineer: William Kennedy, Alex Liu, Erin-Grace Walden, Solomon Martin
// Overview:
//    When this device is turned on it listens to the conductor play
//    plays a song with the conductor. It also displays
//    what pitches it is playing with a servo and leds
//    and displays the timing with a metronome like penduluum
// Design Name:   clocks are robots too
// File Name:     roboclock.ino
//
// Inputs:
//    Start Switch: switch to start playing the song
//    Stop switch: stops the song
//    Volume: controls the volume
//    Tempo: controls the tempo of the song
//    Octave Select: control the octave of the song
// Outputs:
//    Speaker: Used to play the song
//
//
// History:       4 February 2021 - William - File created
//                1 March 2021 - Solomon - Tempo and Windowing Functionality Added
//
//-----------------------------------------------------

// TODO: create array for servo positions based on note (similar to the notes array)
//create lighting array
// TODO: integrate stepper motor w/ main code
// create pendluluum rotation based on tempo
// put in code for buttons
// put in code for octave switching

#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include <math.h>
#include <Stepper.h>




// OUTPUT SETUP (servos, leds, speaker)
#define LED_PIN 12
#define LED_COUNT 7
#define BRIGHTNESS 255
#define SPEAKER_PIN 9
//#define TEST_INPUT 6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare NeoPixel strip object:
Servo metronomeServo;  // create servo object to control a metronom servo

//INPUT SETUP
int octavePin = A2;
int tempoPin = A1; // envelope or comparator input
int audioInput = A0;
#define threshold 200 // Conventions are bad, this is only temporary. Replace w/ threshold input
#define START_BUTTON 8

//SOUND SETUP
#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255
int octave = 4; //Default 4 but should change

//Music Notes based on Octave--
float C = 16.3516*pow(2,octave);
float D = 18.35405*pow(2,octave);
float E = 20.60172*pow(2,octave);
float F = 21.82676*pow(2,octave);
float G = 24.49971*pow(2,octave);
float A = 27.5*pow(2,octave);
float B = 30.86771*pow(2,octave);
float high_C = 32.70320*pow(2,octave);
float rest = 0;

// servo rotations based on notes
#define Cserv 0
#define Dserv 29
#define Eserv 57
#define Fserv 86
#define Gserv 114
#define Aserv 143
#define Bserv 171
#define high_Cserv 200

// led index based on notes
#define Cled 0
#define Dled 1
#define Eled 2
#define Fled 3
#define Gled 4
#define Aled 5
#define Bled 6
#define high_Cled 0

// led color based on notes

uint32_t Ccolor = strip.ColorHSV(0);
uint32_t Dcolor = strip.ColorHSV(9362);
uint32_t Ecolor = strip.ColorHSV(18725);
uint32_t Fcolor = strip.ColorHSV(28087);
uint32_t Gcolor = strip.ColorHSV(37449);
uint32_t Acolor = strip.ColorHSV(46811);
uint32_t Bcolor = strip.ColorHSV(56174);
uint32_t high_Ccolor = strip.ColorHSV(65534);


// playOutput Globals
bool startSong = 0;
bool skipProcessing = 0;
bool testOutput = 0;
bool music = 1;
bool lights = 1;
bool motors = 1;

int pitchPos = 0;    // variable to store the pitch servo position
int metronomePos = 0;    // variable to store the metronome penduluum servo position
int metronomePosMapped;
double tempo;

//Row Row Row Your Boat
int songLength = 54;
//double notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
float* notes[] = {&C, &rest, &C, &rest, &C, &rest, &D, &rest, &E, &rest, &E, &rest, &D, &rest, &E, &rest, &F, &rest, &G, &rest, &high_C, &rest, &high_C, &rest, &high_C, &rest, &G, &rest, &G, &rest, &G, &rest, &E, &rest, &E, &rest, &E, &rest, &C, &rest, &C, &rest, &C, &rest, &G, &rest, &F, &rest, &E, &rest, &D, &rest, &C, &rest};
int beats[] = {2, 1, 2, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 5, 1};

//int songLength = 28;
//int notes[] = {C, C, C, D, E, E, D, E, F, G, high_C, high_C, high_C, G, G, G, E, E, E, C, C, C, G, F, E, D, C, rest};
//int beats[] = {3,3,2,1,3,2,1,2,1,6,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,5,1};
int song_tempo = 250;

int servoNotes[] = {Cserv, Cserv, Cserv, Cserv, Cserv, Cserv, Dserv, Dserv, Eserv, Eserv, Eserv, Eserv, Dserv, Dserv, Eserv, Eserv, Fserv, Fserv, Gserv, Gserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, Gserv, Gserv, Gserv, Gserv, Gserv, Gserv, Eserv, Eserv, Eserv, Eserv, Eserv, Eserv, Cserv, Cserv, Cserv, Cserv, Cserv, Cserv, Gserv, Gserv, Fserv, Fserv, Eserv, Eserv, Dserv, Dserv, Cserv, Cserv};
int ledIndexes[] = {Cled, Cled, Cled, Cled, Cled, Cled, Dled, Dled, Eled, Eled, Eled, Eled, Dled, Dled, Eled, Eled, Fled, Fled, Gled, Gled, high_Cled, high_Cled, high_Cled, high_Cled, high_Cled, high_Cled, Gled, Gled, Gled, Gled, Gled, Gled, Eled, Eled, Eled, Eled, Eled, Eled, Cled, Cled, Cled, Cled, Cled, Cled, Gled, Gled, Fled, Fled, Eled, Eled, Dled, Dled, Cled, Cled};
uint32_t ledColors[] = {Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Dcolor, Dcolor, Ecolor, Ecolor, Ecolor, Ecolor, Dcolor, Dcolor, Ecolor, Ecolor, Fcolor, Fcolor, Gcolor, Gcolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, Gcolor, Gcolor, Gcolor, Gcolor, Gcolor, Gcolor, Ecolor, Ecolor, Ecolor, Ecolor, Ecolor, Ecolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Gcolor, Gcolor, Fcolor, Fcolor, Ecolor, Ecolor, Dcolor, Dcolor, Ccolor, Ccolor};

//Test mode
int mode = LOW;
int modeSecond = LOW;

//Function Prototypes
void buttonISR(int);
double getTempo(float);
int* DFTind(int, float);
void solidColor(uint32_t);
void playOutput(void);

//Stepper Motor
int degree = 1.8;
int currAngle = 0;
Stepper myStepper(200, 4, 5, 6, 7);

void setup() {
  myStepper.setSpeed(60);
  pinMode(octavePin, INPUT);
  pinMode(tempoPin, INPUT);
  pinMode(audioInput, INPUT);
  pinMode(START_BUTTON, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  
  metronomeServo.attach(10);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  //debug only
  Serial.begin(9600);
  Serial.print("Setup Complete\n");
}

void loop() {

  while (true) { // Do not continue until we see a button press
    Serial.println("Waiting for button press");
    solidColor(strip.ColorHSV(56000, 255,   255)); // Green
    mode = !digitalRead(START_BUTTON);
    if (mode == HIGH) {
      long buttonPressStart = millis();
      while (millis() - buttonPressStart < 3000) {}; // Functionally the same as delay(3000), but we could define some actions within loop if necessesary
      modeSecond = !digitalRead(START_BUTTON);
      break;
    }
  }

  if (mode == HIGH && modeSecond == HIGH) {
    // test mode
    Serial.println("Test mode");
    mode = LOW;
    modeSecond = LOW;

    while (true) { // Do not continue until we see a button press
      Serial.println("Waiting for input/output");
      solidColor(strip.ColorHSV(10922, 255,   255));
      mode = !digitalRead(START_BUTTON);
      if (mode == HIGH) {
        long buttonPressStart = millis();
        while (millis() - buttonPressStart < 3000) {}; // Functionally the same as delay(3000), but we could define some actions within loop if necessesary
        modeSecond = !digitalRead(START_BUTTON);
        break;
      }
    }
    if (mode == HIGH && modeSecond == HIGH) {
      //Output Test
      Serial.println("Output Test");
      ///----------------------------------
      ///TEST PROCEDURE FOR LEDS AND SERVOS
      //------------------------------------
      solidColor(strip.ColorHSV(32000,   255,   255)); // Red
      delay(200);
      solidColor(strip.ColorHSV(56000, 255,   255)); // Green
      delay(200);
      solidColor(strip.ColorHSV(10000,   255, 255)); // Blue
      delay(200);
      tempo = 0.125; // Hardcoded tempo for
      startSong = 1;
      playOutput();
      for (int angle : servoNotes) {
        int difference = angle - currAngle;
        Serial.println("clockwise");
        myStepper.step(difference / degree);
        delay(50);
        currAngle = angle;
      }
    } else if (mode == HIGH && modeSecond == LOW) {
      Serial.println("Input Test");
      long startInputTest = millis();
      int brightness;
      while (true) {
        if (!digitalRead(START_BUTTON)) {
          break;
        }
        strip.clear();
        brightness = round((analogRead(tempoPin) - 360)*255.0/20.0);
        strip.setPixelColor(0, brightness);
        strip.setPixelColor(1, brightness);
        strip.setPixelColor(2, brightness);
        strip.setPixelColor(3, brightness);
        strip.setPixelColor(4, brightness);
        strip.setPixelColor(5, brightness);
        strip.setPixelColor(6, brightness);
        strip.setPixelColor(7, brightness);
        strip.setPixelColor(8, brightness);
        strip.show();
        Serial.println(analogRead(tempoPin));
        while(millis() - startInputTest < 500) {};
      }
    }
  } else if (mode == HIGH && modeSecond == LOW) {
    // normal play mode
    Serial.println("Play mode");
    while (digitalRead(START_BUTTON)) { // Loop until next press
      Serial.println("Waiting for octave select.");
      myStepper.step(5);
      octave = 1024 - analogRead(octavePin); //TODO: Map 1024 input values to 6 discrete octave values (0-5 for now)
      if (octave >= 0 && octave < 170) {
        strip.clear();
        strip.setPixelColor(0, Ccolor);
        strip.show();
        octave = 0;
      } else if (octave >= 170 && octave < 340) {
        strip.clear();
        strip.setPixelColor(1, Ccolor);
        strip.show();
        octave = 1;
      } else if (octave >= 340 && octave < 510) {
        strip.clear();
        strip.setPixelColor(2, Ccolor);
        strip.show();
        octave = 2;
      } else if (octave >= 510 && octave < 680) {
        strip.clear();
        strip.setPixelColor(3, Ccolor);
        strip.show();
        octave = 3;
      } else if (octave >= 680 && octave < 850) {
        strip.clear();
        strip.setPixelColor(4, Ccolor);
        strip.show();
        octave = 4;
      } else if (octave >= 850 && octave <= 1024) {
        strip.clear();
        strip.setPixelColor(5, Ccolor);
        strip.show();
        octave = 5;
      }
      Serial.println(octave);
    }
    if (octave == 1) {
        int difference = Cserv - Dserv;
        myStepper.step(difference/degree);
      } else if (octave == 2) {
        int difference = Cserv - Eserv;
        myStepper.step(difference/degree);
      } else if (octave == 3) {
        int difference = Cserv - Fserv;
        myStepper.step(difference/degree);
      } else if (octave == 4) {
        int difference = Cserv - Gserv;
        myStepper.step(difference/degree);
      } else if (octave == 5) {
        int difference = Cserv - Aserv;
        myStepper.step(difference/degree);
      }
    currAngle = 0;
    redefineNotes();
    strip.clear();

    //play mode
    int tempoRaw;
    int sampleRate = 2500; // Limits highest input octave to 5, by nyquist sampling of high_C
    int motor_speed;
    ///----------------------------------
    ///TEMPO DETECTION & DELAY SYNC
    //------------------------------------

    tempo = getTempo(1.0 / sampleRate);
    int DFTsize = 256; //Hopefully multiple of two helps speed
    int* FFTindices = DFTind(DFTsize, 1.0 / sampleRate);
    int windowLength = 256;
    int window[windowLength];
    float v[8][3];
    float fftValues[8];
    int strongFreq[6] = {0, 0, 0, 0, 0, 0}; // We want [E, *, D, *, C, C];

    bool detected = false;
    unsigned long currTime;
    int cnt = 0;

    int state = 0; //E is 3, D is 2, C is 1
    int input; // Input to state machine

    while (!detected && !skipProcessing) {
      if (digitalRead(START_BUTTON)) {
        buttonISR(1);
      }
      for (int i = 0; i < windowLength; i++) {
        currTime = micros();
        window[i] = analogRead(audioInput); // collect window
        while (micros() - currTime < long(1000000.0 / sampleRate)) {} // delay by sample period
      }

      // Goertzel calculation of FFT values
      for (int* k = FFTindices; k < FFTindices + 4; k++) {
        int f = k - FFTindices; //index based on frequency
        for (int n = 0; n < DFTsize; n++) {
          if (n == 0) {
            v[f][2] = window[0];
          } else if (n == 1) {
            v[f][1] = v[f][2];
            v[f][2] = 2 * cos(2.0 * 3.14 * (*k) / DFTsize) * v[f][1] + window[1];
          } else {
            v[f][0] = v[f][1];
            v[f][1] = v[f][2];
            v[f][2] = 2 * cos(2.0 * 3.14 * (*k) / DFTsize) * v[f][1] - v[f][0] + window[n];
          }
        }
        fftValues[f] = pow(v[f][2], 2) + pow(v[f][1], 2) - 2 * cos(-2.0 * 3.14 * (*k) / DFTsize) * v[f][2] * v[f][1];
      }

      // Associate the most recent window w/ a note given frequency values
      for (int i = 1; i < sizeof(strongFreq) / sizeof(int); i++) { //shifting previous values back
        strongFreq[i - 1] = strongFreq[i];
      }
      // Determine strongest frequency
      float maximum = fftValues[0]; //Initializing max at first value
      int maxInd = 0;
      for (int i = 1; i < 4; i++) {
        if (fftValues[i] > maximum) {
          maximum = fftValues[i];
          maxInd = i;
        }
      }

      // State Machine Implementation for Pattern Detection
      input = strongFreq[sizeof(strongFreq) / sizeof(int) - 1]; //most recent value
      if (state == 0) { //start
        if (input == 3) {
          state = 1;
        }
      } else if (state == 1) { // E
        if (input == 1) {
          state = 2;
        } else if (input == 3) {
          // stay at E
        } else {
          state = 0; // Back to start
        }
      } else if (state == 2) { // First Rest
        if (input == 2) {
          state = 3;
        } else if (input == 1) {
          // stay at rest
        } else {
          state = 0; //reset
        }
      } else if (state == 3) { // D
        if (input == 1) {
          state = 4;
        } else if (input == 2) {
          // stay at D
        } else {
          state = 0; //reset
        }
      } else if (state == 4) { // Second Rest
        detected = true;
      }

    }

    if (skipProcessing) { //Skip determined by buttonISR in either of the tempo detection and time sync loops
      if (startSong) {
        strip.clear();
        strip.setPixelColor(0, 255, 0, 255);
        strip.show();
        while (!digitalRead(START_BUTTON)) {}; //Show status lights and then play on next press
        playOutput();
      }
      skipProcessing = false;
      return; //Go back to init state
    }

    while (analogRead(tempoPin) > threshold) { } //let C finish
    //  Serial.println("Waiting for C to finish");
    while (analogRead(tempoPin) < threshold) { } //let rest finish
    //  Serial.println("Final rest finished, now PLAY!!");
    // while (true) {
    //   Serial.println(analogRead(audioInput));
    // }
    //TODO: Verify playing on time w/ speaker
    //------------------------------------
    //playing THE SONG
    //------------------------------------

    startSong = 1;
    while (true)
    {
      if (!startSong) {
        break;
      } else {
        playOutput();
      }
    }

  }
}

void buttonISR(int code = 0) { // Not really an ISR. Saving commented code for more complex test routines
  if (code == 0) {
    startSong = 0;
  } else if (code == 1) {
    skipProcessing = true;
    long startPress = millis();
    while (digitalRead(START_BUTTON)) {}
    if (millis() - START_BUTTON >= 3000) {
      startSong = true;
    }
  }
  // long currTime = millis();
  // int riseCount = 0;
  // int currLevel = 0;
  // int pastLevel = digitalRead(START_BUTTON);
  // while (millis() - currTime < 20) { // detect # of rising edges over 20 ms
  //   currLevel = digitalRead(START_BUTTON);
  //   if (currLevel && !pastLevel) {
  //     riseCount++;
  //   }
  //   pastLevel = currLevel;
  // }
  // switch (riseCount) { //TODO take another look at the redundancy by startSong and testOutput
  // case 0: // Play song in loop
  //   music = 1;
  //   lights = 1;
  //   motors = 1;
  //   startSong = 1;
  //   testOutput = 0;
  //   break;
  // case 1: // All test
  //   music = 1;
  //   lights = 1;
  //   motors = 1;
  //   startSong = 0;
  //   testOutput = 1;
  //   break;
  // case 2:
  //   music = 1;
  //   lights = 0;
  //   motors = 0;
  //   startSong = 0;
  //   testOutput = 1;
  //   break;
  // case 3:
  //   music = 0;
  //   lights = 1;
  //   motors = 0;
  //   startSong = 0;
  //   testOutput = 1;
  //   break;
  // case 4:
  //   music = 0;
  //   lights = 0;
  //   motors = 1;
  //   startSong = 0;
  //   testOutput = 1;
  //   break;
  // }
}


void solidColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show(); // must call this everytime you want the LEDs to update
}

void playOutput() {

  unsigned int mic = 0;
  int i_note_index = 0;
  int duration;
  long metronomeTimer;
  int metronomeMovement = 0;

  while (i_note_index < songLength) {
    if (!digitalRead(START_BUTTON)) {
      buttonISR(0);
    }
    if (!startSong) {
      break;
    }

    Serial.println(tempo);


    mic = analogRead(audioInput);
    Serial.println(mic);

    //play the song
    duration = beats[i_note_index] * tempo * 1000; // in ms
    if (music) {
      tone(SPEAKER_PIN, *notes[i_note_index]*2, duration); // TODO: define w/ Team what octave we should play in
    }

    // metronome servo positioning penduluum

    //new metronome code
    if (motors) {
//      //71 beats in song
//      if (i_note_index == 0) {
//        metronomeTimer = millis();
//        metronomeMovement = 0;
//      }
//      if (i_note_index < 25) {
//        metronomeMovement += 1;
//      } else {
//        metronomeMovement -= 1;
//      }
//      if (metronomeTimer > tempo*100) {
//        Serial.print("Angle: ");
//        Serial.println(metronomeMovement);
//        metronomeServo.write(metronomeMovement);
//        metronomeTimer = millis();
//      }

      //old metronome code
      
      metronomePos = i_note_index;
      if (metronomePos >= 27) {
        metronomePosMapped = map(metronomePos, 28, 53, 100, 0);
      }
      else {
        metronomePosMapped = map(metronomePos, 0, 27, 0, 100);
      }
      //    Serial.print("metronome position: ");
      //    Serial.println(metronomePosMapped);
      metronomeServo.write(metronomePosMapped);

      //Stepper Motor
      int angle = servoNotes[i_note_index];
      int difference = angle - currAngle;
      myStepper.step(difference / degree);
      currAngle = angle;
    }
    //led strip output
    if (lights) {
      strip.clear();
      strip.setPixelColor(ledIndexes[i_note_index], ledColors[i_note_index]);         //  Set pixel's color (in RAM)
      strip.show();
      //    Serial.print("Note playing: ");
      //    Serial.println(notes[i_note_index]);
    }

    long currTime = millis();
    while (millis() - currTime < duration) {
      if (!digitalRead(START_BUTTON)) {
        buttonISR();
        break;
      }
//      //71 beats in song
//      if (i_note_index == 0) {
//        metronomeTimer = millis();
//        metronomeMovement = 0;
//      }
//      if (i_note_index < 25) {
//        metronomeMovement += 1;
//      } else {
//        metronomeMovement -= 1;
//      }
//      if (millis() - metronomeTimer > tempo*1000) {
//        Serial.print("Rest Length: ");
//        Serial.println(tempo*1000);
//        Serial.print("Angle: ");
//        Serial.println(metronomeMovement);
//        metronomeServo.write(metronomeMovement);
//        metronomeTimer = millis();
//      }
    }

    //increment the note counter
    i_note_index += 2; //as delay covers for the rests, we can skip them here
    if (i_note_index >= songLength)
    {
      i_note_index = 0;
      metronomePos = 0;
    }
  }
}

double getTempo(float samplePeriod) {
  int restCount = 0; //The number of samples so far that count as a rest
  int numRests = 0; // The number of entire rests we have recorded
  unsigned int rests[20];
  unsigned long restSum = 0;
  float newData; //TODO: Change back to int
  bool startCount = false;
  long cnt = 0;
  long currTime;
  // Moving Average
  int windowSize = 10;
  int windowSum = 0;
  int window[windowSize];
  int i = 0;
  // Outlier Detection
  int mini = 1024;
  int maxi = 0;
  int data;
  float thresh;
  int outlierRegion = 30;
  int trueTotal;

  samplePeriod = 1 / 5000.0;

  currTime = micros(); // Determine threshold by noise
  while (micros() - currTime < long(10000000 * 3 * samplePeriod)) {
    data = analogRead(tempoPin);
    if (data > maxi) {
      maxi = data;
    }
    if (data < mini) {
      mini = data;
    }
  }
  thresh = mini + 1.5 * (maxi - mini);
  Serial.println(thresh);
  while (true) {
    numRests = 0;
    outlierRegion = 30;
    while (numRests < 20 && !skipProcessing) {
      if (digitalRead(START_BUTTON)) {
        buttonISR(1);
        if (startSong) {
          return 0.3; // Hardcoded tempo in the case of early termination
        }
      }
      currTime = micros();

      newData = analogRead(tempoPin);
      // Moving average implementation
      if (i < windowSize) {
        window[i] = newData;
        windowSum += window[i];
        i++;
      } else {
        windowSum -= window[0];
        for (int j = 1; j < windowSize; j++) {
          window[j - 1] = window[j];
        }
      }
      window[windowSize - 1] = newData;
      windowSum = windowSum + window[windowSize - 1];
      newData = windowSum / (2 * (float)windowSize);

      cnt++;

      // Decision block based on level of averaged input
      if (newData <= thresh) { //if (newData < threshold)
        if (startCount) {
          restCount++;
          if (restCount == 1) {
          }
        }
      } else if (restCount) { // rising edge in envelope // else if (restCount) // && (newData - oldData >= 15)
        if (restCount > outlierRegion) {
          rests[numRests] = restCount;
          numRests++;
        }
        restCount = 0;
      } else {
        startCount = true;
      }

      while (micros() - currTime < long(samplePeriod * 1000000)) {} // delay statement to make sure we're sampling as expected
    }

    // Find max of collected rest periods, and eliminate outliers determined by some percentage of the max
    outlierRegion = rests[0];
    for (int i = 1; i < 20; i++) {
      if (rests[i] > outlierRegion) {
        outlierRegion = rests[i];
      }
    }
    outlierRegion = ceil(outlierRegion * 0.3);
    trueTotal = 0;
    restSum = 0;
    for (int i = 1; i < 20; i++) {
      if (rests[i] > outlierRegion) {
        restSum += rests[i];
        Serial.println(rests[i]);
        trueTotal++;
      }
    }

    if (trueTotal >= 5) {
      break;
    }
  }

  Serial.println(restSum);

  double tempo = samplePeriod * (restSum) / (float)trueTotal; //avg num of rest_samples * sample_period = tempo in sec
  Serial.print("Tempo: ");
  Serial.println(tempo, 5);
  delay(1000);
  return tempo;
}

int* DFTind(int DFTsize, float samplePeriod) {
  float freq[8] = {C, D, E, F, G, A, B, high_C};
  float f = 1.0 / (DFTsize * samplePeriod);
  float bestF[8] = {f, f, f, f, f, f, f, f};
  static int bestInd[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Make sure the indices aren't deallocated

  for (int k = 1; k <= DFTsize; k++) {
    f = k / (DFTsize * samplePeriod);
    for (int i = 0; i < 8; i++) {
      if (abs(freq[i] - f) < abs(freq[i] - bestF[i])) {
        bestF[i] = f;
        bestInd[i] = k;
      }
    }
  }

  return &bestInd[0];
}

void redefineNotes() {
  C = 16.3516*pow(2,octave);
  D = 18.35405*pow(2,octave);
  E = 20.60172*pow(2,octave);
  F = 21.82676*pow(2,octave);
  G = 24.49971*pow(2,octave);
  A = 27.5*pow(2,octave);
  B = 30.86771*pow(2,octave);
  high_C = 32.70320*pow(2,octave);
  rest = 0;
}
