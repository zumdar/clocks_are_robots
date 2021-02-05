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
#define LED_COUNT 12
#define BRIGHTNESS 255
#define SPEAKER_PIN 6

//INPUT SETUP

int octavePin = A0;
int tempoPin = A1;
int audioInput = A2;
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


int pitchPos = 0;    // variable to store the pitch servo position
int metronomePos = 0;    // variable to store the metronome penduluum servo position

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare NeoPixel strip object:
Servo pitchServo;  // create servo object to control a pitch servo
Servo metronomeServo;  // create servo object to control a metronom servo

//Row Row Row Your Boat
int songLength = 54;  
int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};

//int songLength = 28;  
//int notes[] = {C, C, C, D, E, E, D, E, F, G, high_C, high_C, high_C, G, G, G, E, E, E, C, C, C, G, F, E, D, C, rest};
//int beats[] = {3,3,2,1,3,2,1,2,1,6,1,1,1,1,1,1,1,1,1,1,1,1,2,1,2,1,5,1};
int song_tempo = 250;



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
  int tempo;
  int tempoRaw;
  int motor_speed;  
  
  ///----------------------------------
  ///TEST PROCEDURE FOR LEDS AND SERVOS
  //------------------------------------
  solidColor(strip.Color(255,   0,   0)); // Red
  delay(200);
  solidColor(strip.Color(  0, 255,   0)); // Green
  delay(200);
  solidColor(strip.Color(  0,   0, 255)); // Blue
  delay(200);
  for (pitchPos = 0; pitchPos <= 180; pitchPos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    pitchServo.write(pitchPos);              // tell servo to go to position in variable 'pos'
    metronomeServo.write(pitchPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pitchPos = 180; pitchPos >= 0; pitchPos -= 1) { // goes from 180 degrees to 0 degrees
    pitchServo.write(pitchPos);              // tell servo to go to position in variable 'pos'
    metronomeServo.write(pitchPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
    //------------------------------------
    //playing THE SONG
    //------------------------------------
    
    int i_note_index = 0;
    while (true) /// TODO make variable that activates when its time to play
	{	
      //read the tempo pot
      tempoRaw = analogRead(tempoPin);
      tempo = song_tempo*float(tempoRaw)/TempoCal;	//constrain the tempo value into something useful
      Serial.println(tempo);
      
      // set servo rotation for pitch
      
      // set servo rotations for metronome
      
      //play the song
      duration = beats[i_note_index] * tempo;
      tone(SPEAKER_PIN, notes[i_note_index], duration);
      Serial.print("Note playing: ");
      Serial.println(notes[i_note_index]);
      delay(duration);
        
      //increment the note counter
      ++i_note_index;
      if(i_note_index >= songLength) 
      {
        i_note_index = 0;
      }
	} 
  
}


void solidColor(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show(); // must call this everytime you want the LEDs to update
}
