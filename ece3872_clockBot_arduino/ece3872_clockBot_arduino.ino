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
#define LED_COUNT 7
#define BRIGHTNESS 255
#define SPEAKER_PIN 6

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // Declare NeoPixel strip object:
Servo pitchServo;  // create servo object to control a pitch servo
Servo metronomeServo;  // create servo object to control a metronom servo

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
int notes[] = {C, rest, C, rest, C, rest, D, rest, E, rest, E, rest, D, rest, E, rest, F, rest, G, rest, high_C, rest, high_C, rest, high_C, rest, G, rest, G, rest, G, rest, E, rest, E, rest, E, rest, C, rest, C, rest, C, rest, G, rest, F, rest, E, rest, D, rest, C, rest};
int beats[] = {2,1,2,1,2,1,1,1,2,1,2,1,1,1,2,1,1,1,6,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,2,1,1,1,5,1};

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
  int tempo;
  int tempoRaw;
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
  
    //------------------------------------
    //playing THE SONG
    //------------------------------------
    
    int i_note_index = 0;
    while (true) /// TODO make variable that activates when its time to play
	{	
      //read the tempo pot
      tempoRaw = analogRead(tempoPin);
      tempo = song_tempo*float(tempoRaw)/TempoCal;	//constrain the tempo value into something useful
      tempo = 125;
      Serial.println(tempo);
      
      
      //play the song
      duration = beats[i_note_index] * tempo;
      tone(SPEAKER_PIN, notes[i_note_index], duration);
       // set servo rotation for pitch
      pitchServo.write(servoNotes[i_note_index]);
      // metronome servo positioning
      Serial.print("i_note_index: ");
      Serial.println(i_note_index);
      metronomePos = i_note_index;
      if (metronomePos >= 27){
      	metronomePosMapped = map(metronomePos, 28, 53, 100, 0);
      }
      else {
        metronomePosMapped = map(metronomePos, 0, 27, 0, 100);
      }
      Serial.print("metronome position: ");
      Serial.println(metronomePosMapped);
      metronomeServo.write(metronomePosMapped);
//      metronomeServo.write(50); // to calibrate the middle position
      //led strip output
      strip.clear();
      strip.setPixelColor(ledIndexes[i_note_index], ledColors[i_note_index]);         //  Set pixel's color (in RAM)
      strip.show();
      Serial.print("Note playing: ");
      Serial.println(notes[i_note_index]);
      
      delay(duration);
        
      //increment the note counter
      ++i_note_index;
      if(i_note_index >= songLength) 
      {
        i_note_index = 0;
        metronomePos = 0;
      }
	} 
  
}


void solidColor(uint32_t color) {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
  }
  strip.show(); // must call this everytime you want the LEDs to update
}
