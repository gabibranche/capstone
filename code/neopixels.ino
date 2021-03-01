#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN    6

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 295

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


//Papa Bois Values:
unsigned long gunshotTime[] = {65000, 102000, 195000, 198000, 275000};
unsigned long gunshotEnd[] = {70000, 107000, 197000, 205000};


//Test Values:
// unsigned long gunshotTime[] = {1000, 3000, 5000, 7000, 9000};
// unsigned long gunshotEnd[] = {2000, 4000, 6000, 8000};

int Rcolor, Gcolor, Bcolor, treeBright;

const int ledPin =  8;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  //variables to manipulate neopixel colors
  Rcolor = 0;
  Gcolor = 150;
  Bcolor = 0;
  treeBright = 50;

  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(treeBright); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  // put your main code here, to run repeatedly:

  //checks the time to know if to change the lights
  CheckTime();

//  digitalWrite(ledPin, ledState);

  colorWipe(strip.Color(Rcolor,   Gcolor,   Bcolor), 1); // Red
}

void CheckTime() {
  unsigned long currentMillis = millis();

  //increases the array index for each of the gunshot times
  for (int i = 0; i < 4; i++) {
    if (currentMillis > gunshotTime[i] && currentMillis < gunshotEnd[i]) {
      Rcolor = 150;
      Gcolor = 0;
      Bcolor = 0;
      ledState = HIGH;
      treeBright = 50*(i+2);
    } else if (currentMillis > gunshotEnd[i] && currentMillis < gunshotTime[i + 1]) {
      Rcolor = 0;
      Gcolor = 150;
      Bcolor = 0;
      ledState = LOW;
      treeBright = 50;
    }
  }

//colorWipe(strip.Color(Rcolor,   Gcolor,   Bcolor), 1);
//
//  Serial.println(currentMillis);
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}



```
