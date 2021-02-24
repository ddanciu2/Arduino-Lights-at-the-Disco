/* CS 362 Group Project 
 * Lights at the Disco! (LED Music Visualizer)
 * Members: Kyle Trufin, Darian Danciu
 * Written for Ardiuno Uno
 */

#include <SoftwareSerial.h>
#include <FastLED.h>

//LED Strip setup
#define LED_PIN     7
#define BRIGHTNESS  20
#define NUM_LEDS    20 
#define NUM_STRIPS  6
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

// 2D Array for assigning colors to the LEDs
CRGB leds[NUM_STRIPS][NUM_LEDS];

//Spectrum Shield Pins
#define STROBE  4
#define RESET  5
#define AUDIO_1  A0
#define AUDIO_2  A1

//audio input variables
int left_freq[7];
int right_freq[7];
int audio_input = 0;

int decay = 0; // Number of microseconds before one light decay
int decay_check = 0;
long litLEDs = 0; // Number of LEDs that are LIT

//other external devices
int photoResistorPIN = A0;
int photoResistorValue = 0;

int button10PIN = 10;
int button12PIN = 12;

int button10Status;
int button12Status;
int lastButton10Status = LOW;
int lastButton12Status = LOW;

int motorPIN = 9;

//pattern variables
int stylePattern = 1;
int colorPattern = 1;

int runTime = millis();

//sets up the bluetooth signal
SoftwareSerial ble(0,1);

void setup()
{
  //Spectrum Shield setup
  pinMode(STROBE, OUTPUT);
  pinMode(RESET, OUTPUT);
  pinMode(AUDIO_1, INPUT);
  pinMode(AUDIO_2, INPUT);

  digitalWrite(STROBE, HIGH);
  digitalWrite(RESET, LOW);

  //Other pins
  pinMode(motorPIN, OUTPUT);
  pinMode(photoResistorPIN, INPUT);
  pinMode(button10PIN, INPUT);
  pinMode(button12PIN, INPUT);

  //start the bluetooth connection
  ble.begin(9600);
  
  // LED Strip setup
  delay( 3000 ); //safety delay
  //adds the LEDs by row and applies color correction
  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds[0], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds[1], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(leds[2], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(leds[3], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(leds[4], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(leds[5], NUM_LEDS).setCorrection( TypicalLEDStrip ); 
  
  FastLED.setBrightness(BRIGHTNESS);

  //Turns all the LEDs off
  for (int i = 0; i < NUM_STRIPS; i++)
    for (int j = 0; j < NUM_LEDS; j++) {
      leds[i][j] = CRGB(0, 0, 0);
    }
    
  FastLED.show();

  Serial.begin(9600);
}

//pattern where all rows have the same length
void setPattern1() {
  
  int innerStrips = litLEDs * .75;
  int outerStrips = litLEDs * .5;
  
  for (int i = 0; i < NUM_LEDS; i++) {
      if (i < litLEDs) {
        leds[0][i] = CRGB(1, 1, 1);
        leds[1][i] = CRGB(1, 1, 1);
        leds[2][i] = CRGB(1, 1, 1);
        leds[3][i] = CRGB(1, 1, 1);
        leds[4][i] = CRGB(1, 1, 1);
        leds[5][i] = CRGB(1, 1, 1);
      } 
      else {
        leds[0][i] = CRGB(0, 0, 0);
        leds[1][i] = CRGB(0, 0, 0);
        leds[2][i] = CRGB(0, 0, 0);
        leds[3][i] = CRGB(0, 0, 0);
        leds[4][i] = CRGB(0, 0, 0);
        leds[5][i] = CRGB(0, 0, 0);
      }
    }
}

//pattern where the amount of lights decreases on the outer rows
void setPattern2() {
  
  int innerStrips = litLEDs * .75;
  int outerStrips = litLEDs * .5;
  
  for (int i = 0; i < NUM_LEDS; i++) {
      if (i < outerStrips) {
        leds[0][i] = CRGB(1, 1, 1);
        leds[1][i] = CRGB(1, 1, 1);
        leds[2][i] = CRGB(1, 1, 1);
        leds[3][i] = CRGB(1, 1, 1);
        leds[4][i] = CRGB(1, 1, 1);
        leds[5][i] = CRGB(1, 1, 1);
      } 
      else if (i < innerStrips) {
        leds[0][i] = CRGB(0, 0, 0);
        leds[1][i] = CRGB(1, 1, 1);
        leds[2][i] = CRGB(1, 1, 1);
        leds[3][i] = CRGB(1, 1, 1);
        leds[4][i] = CRGB(1, 1, 1);
        leds[5][i] = CRGB(0, 0, 0);
      }
      else if (i < litLEDs) {
        leds[0][i] = CRGB(0, 0, 0);
        leds[1][i] = CRGB(0, 0, 0);
        leds[2][i] = CRGB(1, 1, 1);
        leds[3][i] = CRGB(1, 1, 1);
        leds[4][i] = CRGB(0, 0, 0);
        leds[5][i] = CRGB(0, 0, 0);
      } 
      else {
        leds[0][i] = CRGB(0, 0, 0);
        leds[1][i] = CRGB(0, 0, 0);
        leds[2][i] = CRGB(0, 0, 0);
        leds[3][i] = CRGB(0, 0, 0);
        leds[4][i] = CRGB(0, 0, 0);
        leds[5][i] = CRGB(0, 0, 0);
      }
    }
}

//sets the colors to Red, Blue, and White
void setColor1() {
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
      if (leds[i][j] != CRGB(0, 0, 0)){
        if (j < 7) {
          leds[i][j] = CRGB(255, 0, 0); 
        }
        else if (j < 14) {
          leds[i][j] = CRGB(0, 0, 255); 
        }
        else {
          leds[i][j] = CRGB(255, 255, 255); 
        }
      }
    }
  }
}

//sets the colors to Rainbow
void setColor2() {
  
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
 
      if (leds[i][j] != CRGB(0, 0, 0)){
        if (i == 0) {
          leds[i][j] = CRGB(255, 0, 0); 
        }
        else if (i == 1) {
          leds[i][j] = CRGB(255, 128, 0); 
        }
        else if (i == 2) {
          leds[i][j] = CRGB(255, 255, 0); 
        }
        else if (i == 3) {
          leds[i][j] = CRGB(0, 255, 0); 
        }
        else if (i == 4) {
          leds[i][j] = CRGB(0, 0, 255); 
        }
        else if (i == 5) {
          leds[i][j] = CRGB(127, 0, 255); 
        }
      } 
    }
  }
}

//sets the colors to White and Red stripes
void setColor3() {
  
  for (int i = 0; i < NUM_STRIPS; i++) {
    for (int j = 0; j < NUM_LEDS; j++) {
 
      if (leds[i][j] != CRGB(0, 0, 0)){
        if (j % 2 == 0) {
          leds[i][j] = CRGB(255, 0, 0); 
        }
        else {
          leds[i][j] = CRGB(255, 255, 255); 
        }
      } 
    }
  }
}

//applies the pattern depending on button selection
void setPattern()
{ 
  
  if (stylePattern == 1) {
    setPattern1();
  }
  else if (stylePattern == 2) {
    setPattern2();
  }
  
  if (colorPattern == 1) {
    setColor1();
  }
  else if (colorPattern == 2) {
    setColor2();
  }
  else if (colorPattern == 3 ) {
    setColor3();
  }
  
  FastLED.show(); 
}

//reads in the frequencies picked up by the Spectrum Shield
void read_freq()
{
  digitalWrite(RESET, HIGH);
  delayMicroseconds(200);
  digitalWrite(RESET, LOW);
  delayMicroseconds(200);

  //reads the frequencies for each of the 7 bands
  for(int i = 0; i < 7; i++)
  {
    digitalWrite(STROBE, HIGH); //Goes to the next band
    delayMicroseconds(50); 
    digitalWrite(STROBE, LOW);
    delayMicroseconds(50);  
    
    left_freq[i] = analogRead(AUDIO_1); //stores the left frequencies
    right_freq[i] = analogRead(AUDIO_2); // and right frequencies
    
  }
}

//converts the frequencies to the amount of LEDs we want to light up
void convert_freq()
{
  if (left_freq[0] > right_freq[0])
    audio_input = left_freq[0];
  else
    audio_input = right_freq[0];

  if (audio_input > 80)
  { 
      litLEDs = ((long)NUM_LEDS * (long)audio_input) / 105; 
      
  }
}

void loop()
{  
  //read button input
  button10Status = digitalRead(button10PIN);
  button12Status = digitalRead(button12PIN);

  //read photoresistor input
  photoResistorValue = analogRead(photoResistorPIN);
  analogWrite(motorPIN, 50);

  //read frequencies from audio input
  read_freq();
  convert_freq();

  //See if button status has changed since last loop read
  if(button10Status != lastButton10Status){

    //check if button is pressed
    if(button10Status == HIGH){
       stylePattern++;
       if (stylePattern == 3) {
        stylePattern = 1;
       }
    }
   
    //Store newly read button status into another variable
    lastButton10Status = button10Status;
  }
  
  //See if button status has changed since last loop read
  if(button12Status != lastButton12Status){

    //check if button is pressed
    if(button12Status == HIGH){
       colorPattern++;
       if (colorPattern == 4) {
        colorPattern = 1;
       }
    }
   
    //Store newly read button status into another variable
    lastButton12Status = button12Status;
  }

  //apply pattern and color
  setPattern(); 

  //turn off LEDs
  if (litLEDs > 0)
    litLEDs--;

  //send frequency and photoresistor values to smartphone
  Serial.println("Freq: " + (String)audio_input +" Res: " + (String)photoResistorValue);
  
  

}
