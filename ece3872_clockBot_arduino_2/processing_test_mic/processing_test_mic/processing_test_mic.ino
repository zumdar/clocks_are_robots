//-----------------------------------------------------
// Engineer: William Kennedy, Alex Liu, Erin-Grace Walden, Solomon Martin
// Overview:
//    When this device is turned on it listens to the conductor play
//    plays a song with the conductor. It also displays
//    what pitches it is playing with a servo and leds
//    and displays the timing with a metronome like penduluum
// Design Name:   clocks are robots too
// File Name:     processing_test.ino
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
// History:      25 March 2021 - Solomon - File Created
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
#include <VariableTimedAction.h>
#include <stdio.h>
#include <math.h>



// OUTPUT SETUP (servos, leds, speaker)
#define LED_PIN 12
#define LED_COUNT 7
#define BRIGHTNESS 255
#define SPEAKER_PIN 9
#define TEST_INPUT 6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare NeoPixel strip object:
Servo pitchServo;  // create servo object to control a pitch servo
Servo metronomeServo;  // create servo object to control a metronom servo

//INPUT SETUP
int octavePin = A2;
int tempoPin = A1; // envelope or comparator input
int audioInput = A0;
#define threshold 380 // Conventions are bad, this is only temporary. Replace w/ threshold input
#define START_BUTTON 8
#define TEST_TEMPO 0.4 // beat time in ms

//SOUND SETUP
#define TempoCal 512
#define TempoPotMax 1023
#define PwmMax 255
#define rest 0
#define octave 4

//Music Notes based on Octave--
#define C 16.3516*pow(2,octave)
#define D 18.35405*pow(2,octave)
#define E 20.60172*pow(2,octave)
#define F 21.82676*pow(2,octave)
#define G 24.49971*pow(2,octave)
#define A 27.5*pow(2,octave)
#define B 30.86771*pow(2,octave)
#define high_C 32.70320*pow(2,octave)
#define rest 0

// servo rotations based on notes
#define Cserv 0
#define Dserv 37
#define Eserv 73
#define Fserv 110
#define Gserv 146
#define Aserv 183
#define Bserv 219
#define high_Cserv 255

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



bool startSong = 0;
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
double notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2, 1, 2, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1, 1, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 5, 1};

//int songLength = 28;
//int notes[] = {C, C, C, D, E, E, D, E, F, G, high_C, high_C, high_C, G, G, G, E, E, E, C, C, C, G, F, E, D, C, rest};
//int beats[] = {3,3,2,1,3,2,1,2,1,6,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,5,1};
int song_tempo = 250;

int servoNotes[] = {Cserv, Cserv, Cserv, Cserv, Cserv, Cserv, Dserv, Dserv, Eserv, Eserv, Eserv, Eserv, Dserv, Dserv, Eserv, Eserv, Fserv, Fserv, Gserv, Gserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, high_Cserv, Gserv, Gserv, Gserv, Gserv, Gserv, Gserv, Eserv, Eserv, Eserv, Eserv, Eserv, Eserv, Cserv, Cserv, Cserv, Cserv, Cserv, Cserv, Gserv, Gserv, Fserv, Fserv, Eserv, Eserv, Dserv, Dserv, Cserv, Cserv};
int ledIndexes[] = {Cled, Cled, Cled, Cled, Cled, Cled, Dled, Dled, Eled, Eled, Eled, Eled, Dled, Dled, Eled, Eled, Fled, Fled, Gled, Gled, high_Cled, high_Cled, high_Cled, high_Cled, high_Cled, high_Cled, Gled, Gled, Gled, Gled, Gled, Gled, Eled, Eled, Eled, Eled, Eled, Eled, Cled, Cled, Cled, Cled, Cled, Cled, Gled, Gled, Fled, Fled, Eled, Eled, Dled, Dled, Cled, Cled};
uint32_t ledColors[] = {Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Dcolor, Dcolor, Ecolor, Ecolor, Ecolor, Ecolor, Dcolor, Dcolor, Ecolor, Ecolor, Fcolor, Fcolor, Gcolor, Gcolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, high_Ccolor, Gcolor, Gcolor, Gcolor, Gcolor, Gcolor, Gcolor, Ecolor, Ecolor, Ecolor, Ecolor, Ecolor, Ecolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Ccolor, Gcolor, Gcolor, Fcolor, Fcolor, Ecolor, Ecolor, Dcolor, Dcolor, Ccolor, Ccolor};

class ProtoThread : public VariableTimedAction {

  int duration = floor(TEST_TEMPO*1000);
  int currBeat = 0;
  int beatSum = 0;
  int note_ind = 0;

  unsigned long run() {
    if (note_ind >= songLength) {
      currBeat = 0;
      note_ind = 0;
      beatSum = 0;
    }
    while (currBeat >= beatSum + beats[note_ind]) {
      beatSum += beats[note_ind];
      note_ind += 1;
    }
    tone(TEST_INPUT, notes[note_ind], duration);

    currBeat += 1;
    return 0;
  }
};

ProtoThread testAudioInput;

void setup() {
  pinMode(octavePin, INPUT);
  pinMode(tempoPin, INPUT);
  pinMode(audioInput, INPUT);
  pinMode(START_BUTTON, INPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(TEST_INPUT, OUTPUT);

  pitchServo.attach(3);  // attaches the servo on pin 3 to the servo object
  metronomeServo.attach(10);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  //debug only
  Serial.begin(9600);
  Serial.print("Setup Complete\n");
}

void loop() {

  int tempoRaw;
  int sampleRate = 2500; // Limits highest input octave to 5, by nyquist sampling of high_C
  int motor_speed;

  ///----------------------------------
  ///TEST PROCEDURE FOR LEDS AND SERVOS
  //------------------------------------
  solidColor(strip.ColorHSV(32000,   255,   255)); // Red
  delay(200);
  solidColor(strip.ColorHSV(56000, 255,   255)); // Green
  delay(200);
  solidColor(strip.ColorHSV(10000,   255, 255)); // Blue
  delay(200);

  ///----------------------------------
  ///TEMPO DETECTION & DELAY SYNC
  //------------------------------------


  testAudioInput.start(TEST_TEMPO*1000, true);
  
  tempo = getTempo(1.0/sampleRate);
//  testAudioInput.updateActions();
  int DFTsize = tempo >= 0.15 ? 128 : 96; //Hopefully multiple of two helps speed
  int* FFTindices = DFTind(DFTsize, 1.0/sampleRate);
  int windowLength = DFTsize;
  int window[windowLength];
  float v[8][3];
  float fftValues[8];
  int strongFreq[6] = {0, 0, 0, 0, 0, 0}; // We want [E, *, D, *, C, C]; 
  
  bool detected = false;
  unsigned long currTime; 
  int cnt = 0;

  int state = 0; //E is 3, D is 2, C is 1
  int input; // Input to state machine

  while (!detected) {
    for (int i = 0; i < windowLength; i++) {
      currTime = micros();
//      testAudioInput.updateActions();
      window[i] = analogRead(audioInput); // collect window
//      for (int* k = FFTindices; k < FFTindices + 4; k++) { // TODO: note how calculating v here is too slow
//        int f = k - FFTindices;
//        if (i == 0) {
//            v[f][2] = window[0];
//          } else if (i == 1) {
//            v[f][1] = v[f][2];
//            v[f][2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[f][1] + window[1];
//          } else {
//            v[f][0] = v[f][1];
//            v[f][1] = v[f][2];
//            v[f][2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[f][1] - v[f][0] + window[i];
//          }
//      }
//      if (micros() - currTime > long(1000000.0/sampleRate)) {
//        Serial.println(micros() - currTime);
//      }
      while (micros() - currTime < long(1000000.0/sampleRate)) {} // delay by sample period
    }

     unsigned long currTime2 = millis(); // Print total calculation time per window
    // Goertzel calculation of FFT values
    for (int* k = FFTindices; k < FFTindices + 4; k++) {
      int f = k - FFTindices; //index based on frequency
      for (int n = 0; n < DFTsize; n++) {
//        testAudioInput.updateActions();
        if (n == 0) {
          v[f][2] = window[0];
        } else if (n == 1) {
          v[f][1] = v[f][2];
          v[f][2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[f][1] + window[1];
        } else {
          v[f][0] = v[f][1];
          v[f][1] = v[f][2];
          v[f][2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[f][1] - v[f][0] + window[n];
        }
      }
      fftValues[f] = pow(v[f][2], 2) + pow(v[f][1], 2) - 2*cos(-2.0*3.14*(*k)/DFTsize)*v[f][2]*v[f][1];
    }
//     if (cnt % 6 == 0) {
//       Serial.println(millis() - currTime2);
//     }

    // Associate the most recent window w/ a note given frequency values
    for (int i = 1; i < sizeof(strongFreq)/sizeof(int); i++) { //shifting previous values back
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

    //Print Values every six loops
    strongFreq[sizeof(strongFreq)/sizeof(int) - 1] = maxInd + 1; //just some notation b/c initial array is all zeroes
    if (cnt % 6 == 0) {
      for (int i = 0; i < sizeof(strongFreq)/sizeof(int); i++) {
        Serial.print(strongFreq[i]); Serial.print(' ');
          if (i == sizeof(strongFreq)/sizeof(int) - 1) {
            Serial.println("");
          }
      }
    }
    cnt++;

    // State Machine Implementation for Pattern Detection
    input = strongFreq[sizeof(strongFreq)/sizeof(int) - 1]; //most recent value
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
      } else { state = 0;} //reset
    } else if (state == 3) { // D
      if (input == 1) {
        state = 4;
      } else if (input == 2) {
        // stay at D
      } else { state = 0;} //reset
    } else if (state == 4) { // Second Rest
      detected = true;
    }
      
  }

  Serial.println("Waiting for C to finish");
  while (analogRead(tempoPin) > threshold) { testAudioInput.updateActions(); }; //let C finish
  Serial.println("Waiting for rest to finish");
  currTime = millis();
  while (millis() - currTime < long(tempo*1000)) { };
//  while (analogRead(tempoPin) < threshold) { testAudioInput.updateActions(); } //let rest finish
  Serial.println("Final rest finished, now PLAY!!");
//  while (true) {
//    testAudioInput.updateActions();
//    Serial.println(analogRead(audioInput));
//  }
  //------------------------------------
  //playing THE SONG
  //------------------------------------

  startSong = 1;
  playOutput();
  while (true) /// TODO make variable that activates when its time to play
  {
    if (digitalRead(START_BUTTON)) {
      buttonISR();
    }
    if (!startSong && testOutput) {
      playOutput();
      testOutput = 0;
    }
    while (startSong) {
      playOutput();
    }
  }

}

void buttonISR() { // Not really an ISR
  long currTime = millis();
  int riseCount = 0;
  int currLevel = 0;
  int pastLevel = digitalRead(START_BUTTON);
  while (millis() - currTime < 20) { // detect # of rising edges over 20 ms
    currLevel = digitalRead(START_BUTTON);
    if (currLevel && !pastLevel) {
      riseCount++;
    }
    pastLevel = currLevel;
  }
  switch (riseCount) { //TODO take another look at the redundancy by startSong and testOutput
  case 0: // Play song in loop
    music = 1;
    lights = 1;
    motors = 1;
    startSong = 1;
    testOutput = 0;
    break;
  case 1: // All test
    music = 1;
    lights = 1;
    motors = 1;
    startSong = 0;
    testOutput = 1;
    break;
  case 2:
    music = 1;
    lights = 0;
    motors = 0;
    startSong = 0;
    testOutput = 1;
    break;
  case 3:
    music = 0;
    lights = 1;
    motors = 0;
    startSong = 0;
    testOutput = 1;
    break;
  case 4:
    music = 0;
    lights = 0;
    motors = 1;
    startSong = 0;
    testOutput = 1;
    break;
  }
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
  long currTime1;
  long currTime2;

  while (i_note_index < songLength) {
    currTime1 = millis();
    if (digitalRead(START_BUTTON)) {
//      buttonISR();
    }
    if (!startSong) {
      break;
    }

//    Serial.println(duration);

    mic = analogRead(audioInput);

    //play the song
    duration = beats[i_note_index]*tempo*1000; // in ms
    if (music) {
      tone(SPEAKER_PIN, notes[i_note_index]/2.0, duration);
    }
    
    // metronome servo positioning
    if (motors) {
      //TODO: Include stepper motor (clock hand) positioning
      metronomePos = metronomePos + beats[i_note_index];
      if (metronomePos >= 36) {
        metronomePosMapped = map(metronomePos, 36, 72, 128, 64);
      }
      else {
        metronomePosMapped = map(metronomePos, 0, 36, 64, 128);
      }
  //    Serial.print("metronome position: ");
  //    Serial.println(metronomePosMapped); 
      metronomeServo.write(metronomePosMapped);
    }
    //led strip output
    if (lights) {
      strip.clear();
      strip.setPixelColor(ledIndexes[i_note_index], ledColors[i_note_index]);         //  Set pixel's color (in RAM)
      strip.show();
  //    Serial.print("Note playing: ");
  //    Serial.println(notes[i_note_index]);
    }
    while (millis() - currTime1 < duration) {};

    currTime2 = millis();
    
    while (millis() - currTime2 < tempo*1000) {
      if (!digitalRead(START_BUTTON)) {
//        buttonISR();
        break;
      }
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


  samplePeriod = 1/5000.0;

//  while (true) { // view raw envelope
//    currTime = micros();
//    Serial.println(analogRead(tempoPin));
//    while (micros() - currTime < long(samplePeriod*1000000)) {};
//    }
//  while (true) { // view smoothed envelope
//    currTime = micros();
//    newData = analogRead(tempoPin);
//    if (i < windowSize) {
//      window[i] = newData;
//      windowSum += window[i];
//      i++;
//    } else {
//      windowSum -= window[0];
//      for (int j = 1; j < windowSize; j++) {
//        window[j - 1] = window[j];
//      }
//      window[windowSize - 1] = newData;
//      windowSum = windowSum + window[windowSize -1];
//      Serial.println(windowSum/(float)windowSize);
//    }
//    while (micros() - currTime < long(samplePeriod*1000000)) {};
//  }
  
  currTime = micros(); // Determine threshold by noise
  while (micros() - currTime < long(10000000*3*samplePeriod)) {
    data = analogRead(tempoPin);
    if (data > maxi) {
      maxi = data;
    }
    if (data < mini) {
      mini = data;
    }
  }
  thresh = mini + 1.5*(maxi - mini);
  Serial.println(thresh);
  delay(1500);
  while (true) {
    numRests = 0;
    outlierRegion = 30;
    while (numRests < 20) {  
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
      newData = windowSum/(2*(float)windowSize);
  
  //    if (cnt % round(1/samplePeriod) == 0) { // once per second
  //      Serial.println(numRests);
  //    }
      cnt++;
  //    Serial.println(newData);

      // Decision block based on level of averaged input
      if (newData <= thresh) { //if (newData < threshold)
        if (startCount) {
          restCount++;
          if (restCount == 1) {
  //          Serial.println(150);
          }
        }
      } else if (restCount) { // rising edge in envelope // else if (restCount) // && (newData - oldData >= 15)
        if (restCount > outlierRegion) {
          rests[numRests] = restCount;
          numRests++;
        }
        restCount = 0;
  //      Serial.println(650);
      } else {
        startCount = true;
      }
      
      while (micros() - currTime < long(samplePeriod*1000000)) {} // delay statement to make sure we're sampling as expected
    }
  
    // Find max of collected rest periods, and eliminate outliers determined by some percentage of the max
    outlierRegion = rests[0];
    for (int i = 1; i < 20; i++) {
      if (rests[i] > outlierRegion) {
        outlierRegion = rests[i];
      }
    }
    outlierRegion = ceil(outlierRegion*0.3);
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
 
  double tempo = samplePeriod*(restSum)/(float)trueTotal; //avg num of rest_samples * sample_period = tempo in sec
  Serial.print("Tempo: ");
  Serial.println(tempo, 5);
  delay(1000);
  return tempo;
}

int* DFTind(int DFTsize, float samplePeriod) {
  float freq[8] = {C, D, E, F, G, A, B, high_C}; //TODO
  float f = 1.0/(DFTsize*samplePeriod);
  float bestF[8] = {f, f, f, f, f, f, f, f};
  static int bestInd[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Make sure the indices aren't deallocated
  
  for (int k = 1; k <= DFTsize; k++) {
    f = k/(DFTsize*samplePeriod);
    for (int i = 0; i < 8; i++) {
      if (abs(freq[i] - f) < abs(freq[i] - bestF[i])) {
        bestF[i] = f;
        bestInd[i] = k;
      }
    }
  }
  for (int i = 0; i < 8; i++) {
    Serial.print(bestInd[i]); Serial.print(" ");
  }
  Serial.println("");
  return &bestInd[0];
}
