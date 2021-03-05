//-----------------------------------------------------
// Engineer: William Kennedy, Alex Liu, Erin-Grace Walden, Solomon Martin
// Overview:
//		When this device is turned on it listens to the conductor play
//		plays a song with the conductor. It also displays
// 		what pitches it is playing with a servo and leds
//		and displays the timing with a metronome like penduluum
// Design Name:   clocks are robots too
// File Name:     roboclock.ino
//
// Inputs:
//		Start Switch: switch to start playing the song
//		Stop switch: stops the song
//		Volume: controls the volume
//		Tempo: controls the tempo of the song
//		Octave Select: control the octave of the song
// Outputs:
//		Speaker: Used to play the song
//
//
// History:       4 February 2021 - William - File created
//                1 March 2021 - Solomon - Tempo and Windowing Functionality Added
//
//-----------------------------------------------------

// TODO: create array for servo positions based on note (similar to the notes array)
//create lighting array
// create pendluluum rotation based on tempo
// put in code for buttons
// put in code for octave switching

#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <stdio.h>
#include <math.h>



// OUTPUT SETUP (servos, leds, speaker)
#define LED_PIN    2
#define LED_COUNT 7
#define BRIGHTNESS 255
#define SPEAKER_PIN 6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare NeoPixel strip object:
Servo pitchServo;  // create servo object to control a pitch servo
Servo metronomeServo;  // create servo object to control a metronom servo

//INPUT SETUP

int octavePin = A0;
int tempoPin = A1; // envelope or comparator input
int audioInput = A2;
#define threshold 340 // Conventions are bad, this is only temporary. Replace w/ threshold input
#define START_BUTTON 8
#define STOP_BUTTON 9

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





int pitchPos = 0;    // variable to store the pitch servo position
int metronomePos = 0;    // variable to store the metronome penduluum servo position
int metronomePosMapped;

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



void setup() {
  pinMode(octavePin, INPUT);
  pinMode(tempoPin, INPUT);
  pinMode(START_BUTTON, INPUT);
  pinMode(STOP_BUTTON, INPUT);

  pinMode(SPEAKER_PIN, OUTPUT);

  pitchServo.attach(3);  // attaches the servo on pin 3 to the servo object
  metronomeServo.attach(5);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);

  //debug only
  Serial.begin(9600);
  Serial.print("Setup Complete\n");
}

void loop() {

  int duration;
  double tempo;
  int tempoRaw;
  long sampleRate = 5000L;
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
  
  tempo = getTempo();
  Serial.println((int)(tempo*1000));
  int DFTsize = 500;
  int* FFTindices = DFTind(DFTsize, 1.0/sampleRate);
  Serial.println(FFTindices[0]);
//  long windowLength = ceil(tempo*sampleRate); // window is the size of a single beat
  long windowLength = 2195L;
  Serial.println(windowLength, 4);
  int window[windowLength];
  float v[3];
  float fftValues[8];
  int strongFreq[6] = {0, 0, 0, 0, 0, 0}; // We want [E, *, D, *, C, C]; 
  
  bool detected = false;
  long currTime; 
  while (!detected) {
    for (int i = 0; i < windowLength; i++) {
      currTime = millis();
      window[i] = analogRead(audioInput); // collect window
      while (currTime - millis() < long(1000.0/sampleRate)) {} // delay by sample period
    }

    // Goertzel calculation of FFT values
    for (int* k = FFTindices; k < FFTindices + 8; k++) {
      for (int n = 0; n < DFTsize; n++) {
        if (n == 0) {
          v[2] = window[0];
        } else if (n == 1) {
          v[1] = v[2];
          v[2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[1] + window[1];
        } else {
          v[0] = v[1];
          v[1] = v[2];
          v[2] = 2*cos(2.0*3.14*(*k)/DFTsize)*v[1] - v[0] + window[n];
        }
      }
      fftValues[k - FFTindices] = pow(v[2], 2) + pow(v[1], 2) - 2*cos(-2.0*3.14*(*k)/DFTsize)*v[2]*v[1];
    }

    // Associate the most recent window w/ a note given frequency values
    for (int i = 1; i < sizeof(strongFreq)/sizeof(int); i++) { //shifting previous values back
      strongFreq[i - 1] = strongFreq[i];
    }
    
    float maximum;
    int maxInd = 0;
    for (int i = 0; i < 8; i++) {
      if (i == 0) {
        maximum = fftValues[0];
      } else {
        if (fftValues[i] > maximum) {
          maximum = fftValues[i];
          maxInd = i;
        }
      }
    } 
    strongFreq[sizeof(strongFreq)/sizeof(int) - 1] = maxInd + 1; //just some notation b/c initial array is all zeroes
    for (int i = 0; i < sizeof(strongFreq)/sizeof(int); i++) {
//      Serial.print(strongFreq[i]); Serial.print(' ');
//        if (i == sizeof(strongFreq)/sizeof(int) - 1) {
//          Serial.println("");
//        }
    }
 
    if (strongFreq[0] == 3 && strongFreq[2] == 2 && strongFreq[4] == 1 && strongFreq[5] == 1) {
      detected == true;
      while(true) {};
    }
  } 
  
//  long currTime = millis(); // now that we've seen the second beat of C, wait 4 more beats before playing;
//  while (millis() - currTime < long(4*1000.0/sampleRate)) {}
  // now that we've seen the second beat of C, wait until after final rest to play
  while (analogRead(audioInput) > threshold) {} //let C finish
  while (analogRead(audioInput) < threshold) {} //let rest finish
  
  //------------------------------------
  //playing THE SONG
  //------------------------------------

  int i_note_index = 0;
  //
//      sample_rate = 5000;
//      window_s = 1;
//      window = ceil(sample_rate*window_s);
//      synced = false;
//      while (!synced) {
//  
//      }
  unsigned int mic = 0;

  while (true) /// TODO make variable that activates when its time to play
  {


    //read the tempo pot
    //      tempoRaw = analogRead(tempoPin);
//    tempoRaw = 512;
//    tempo = song_tempo * float(tempoRaw) / TempoCal;	//constrain the tempo value into something useful
    Serial.println(tempo);

    mic = analogRead(audioInput);
    Serial.println(mic);

    //play the song
    duration = beats[i_note_index] * tempo*1000; // in ms
    tone(SPEAKER_PIN, notes[i_note_index], duration);
    // set servo rotation for pitch
    pitchServo.write(servoNotes[i_note_index]);
    // metronome servo positioning
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
    //led strip output
    strip.clear();
    strip.setPixelColor(ledIndexes[i_note_index], ledColors[i_note_index]);         //  Set pixel's color (in RAM)
    strip.show();
//    Serial.print("Note playing: ");
//    Serial.println(notes[i_note_index]);

    delay(duration);

    //increment the note counter
    i_note_index += 2; //as delay covers for the rests, we can skip them here
    if (i_note_index >= songLength)
    {
      i_note_index = 0;
      metronomePos = 0;
    }
  }

}


void solidColor(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show(); // must call this everytime you want the LEDs to update
}

double getTempo() {
  float samplePeriod = 1.0/5000.0;
  int restCount = 0; //The number of samples so far that count as a rest
  int numRests = 0; // The number of entire rests we have recorded
  int rests[20];
  int newData;
  bool startCount = false;
  long cnt = 0;

  while (numRests < 20) {
    long currTime = millis();
    newData = analogRead(audioInput);
    if (cnt % 5000 == 0) {
      Serial.println(newData);
    }
//    cnt++;
    if (newData < threshold) {
      if (startCount) {
        restCount++;
      }
    } else if (restCount) { // rising edge in envelope
      rests[numRests] = restCount;
      numRests++;
      restCount = 0;
    } else {
      startCount = true;
    }
    while (micros() - currTime < long(samplePeriod*1000000)) {} // delay statement to make sure we're sampling as expected
  }

  int restSum = 0;
  for (int i = 0; i < 20; i++) {
    Serial.print(rests[i]); Serial.print(' ');
    restSum += rests[i];
  }
  Serial.println(restSum);
 
//  Serial.println((rests[0] + rests[1] + rests[2])/20);
  double tempo = samplePeriod*(restSum)/20.0; //avg num of rest_samples * sample_period = tempo in sec
  Serial.println(tempo, 5);
  return tempo;
}

int* DFTind(int DFTsize, float samplePeriod) {
  float freq[8] = {C, D, E, F, G, A, B, high_C};
  float f = 1.0/(DFTsize*samplePeriod);
  float bestF[8] = {f, f, f, f, f, f, f, f};
  int bestInd[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  
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
//  while(true) {}
  return &bestInd[0];
}
