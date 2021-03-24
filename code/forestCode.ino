#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 550


// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


//Values used to test, without loosing the calibrated values
//unsigned long gunshotTime[] = {63000, 98000, 193000, 275000};
//unsigned long gunshotEnd[] = {66000, 102000, 199000};

//Papa Bois Values: values needed to be calibrated taking into account that the pixels take roughly 5 secs to change
unsigned long gunshotTime[] = {25000, 41000, 80500, 130000};
unsigned long gunshotEnd[] = {29000, 43800, 85200};

//RGB Values of the NeoPixels
int Rcolor, Gcolor, Bcolor, treeBright;
int UltraRcolor, UltraGcolor, UltraBcolor, UltraTreeBright;

const int ledPin =  8;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

boolean gunRing = false;       // boolean used to determine whether to prioritise sound values or ultrasonic values

const int pingPin = 3; // Trigger Pin of Ultrasonic Sensor
const int echoPin = 2; // Echo Pin of Ultrasonic Sensor

unsigned long UltraPreviousMillis = 0;

const long UltraInterval = 100;




void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Rcolor = 0;
  Gcolor = 150;
  Bcolor = 0;
  treeBright = 50;

  UltraRcolor = 0;
  UltraGcolor = 150;
  UltraBcolor = 0;
  UltraTreeBright = 150;

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)

  // initiating with blue and giving a delay so as to set up all the trees before the music starts
    for (int i = 0; i < strip.numPixels(); i += 2) {
      strip.setPixelColor(i, strip.Color(0, 0, 150));
    }
  strip.show();
//  Turn OFF all pixels ASAP
  strip.setBrightness(treeBright); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(echoPin, INPUT);

  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:

  CheckTime();
  ultraSonicMeasure();
  DistPix(strip.Color(UltraRcolor,   UltraGcolor,   UltraBcolor)); // Red);
  //  SoundPix(strip.Color(Rcolor,   Gcolor,   Bcolor)); // Red

}


//Checks the time of millis and change color when the gun shot rings (coded to music)
void CheckTime() {
  unsigned long currentMillis = millis();

  for (int i = 0; i < 4; i++) {
    if (currentMillis > gunshotTime[i] && currentMillis < gunshotEnd[i]) {
      gunRing = true;
      UltraRcolor = 150;
      UltraGcolor = 0;
      UltraBcolor = 0;
      //      ledState = HIGH;
      treeBright = 50 * (i + 2);
    } else if (currentMillis > gunshotEnd[i] && currentMillis < gunshotTime[i + 1]) {
      gunRing = false;
      UltraRcolor = 0;
      UltraGcolor = 150;
      UltraBcolor = 0;
      //      ledState = LOW;
      treeBright = 50;

    }
  }
}

//Changes all the even lights based on CheckTime
//void SoundPix(uint32_t color) {
//  for (int i = 0; i < strip.numPixels(); i += 2) { // For each pixel in strip...
//    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//    //    strip.show();                          //  Update strip to match
//    //    delay(wait);                           //  Pause for a moment
//    //    strip.setBrightness(treeBright);
//  }
//  strip.show();
//}

//Measures how far someone is from the tree
void ultraSonicMeasure() {

  unsigned long UltraCurrentMillis = millis();

  if (UltraCurrentMillis - UltraPreviousMillis >= UltraInterval) {
    UltraPreviousMillis = UltraCurrentMillis;

    long duration, inches, cm;
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);

    if (gunRing == false) {

      for (int i = 1; i < strip.numPixels(); i += 2) { // For each pixel...

        if (cm <= 200 && gunRing == false) {
          // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
          // Here we're using a moderately bright green color:

          UltraRcolor = map(cm, 200, 0, 150, 255);
          UltraGcolor = map(cm, 200, 0, 150, 0);

        } else  {
          UltraRcolor = 0;
          UltraGcolor = 150;
        }

      }
      //    delay(DELAYVAL); // Pause before next pass through loop
    }
  }
}

//UPDATED: Shows all the pixels:
//Displays the odd pixels based on distance
void DistPix(uint32_t color) {
  for (int i = 0; i < strip.numPixels(); i ++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    //    strip.show();                          //  Update strip to match
    //    delay(wait);                           //  Pause for a moment
    //    strip.setBrightness(UltraTreeBright);
  }
  strip.show();
}

//turns microseconds to inches and centimeters
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
