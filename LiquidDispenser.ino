// Screen Libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Screen Parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//////////

// Cooldown and Mixer Time in Milliseconds
#define COOLDOWN 5000
#define MIXER_MAX 7500
#define SHOT_UNIT 1500

// Motor Pins
#define MOTOR_SHOT 4      // Shot Output
#define MOTOR_MIXER 5     // Mixer Output

// Button Pins //
#define BUTTON_CHOOSE 2    // Choose Button
#define BUTTON_START 3     // Start Button

bool mode_reg = 0;
bool buttonChoose_prev = 0;
bool buttonChoose = 0;

bool buttonStart_prev = 0;
bool buttonStart = 0;
//////////

// Potentiometer Readings
int potPin = A7;          // Pot Input
int pot_readout = 0;

// Initial Percentage and Shot Count
int percentage = 0;
int shotCount = 0;

int shotTime = 0;
int mixerTime = 0;

// Clock Tied ints
unsigned int clock = 0;
unsigned int cooldownEnd = 0;

void setup() {
  Serial.begin(115200);

  // Display Setup //
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //////////

  // Button and Motor Pins //
  pinMode(BUTTON_CHOOSE, INPUT);
  pinMode(BUTTON_START, INPUT);

  pinMode(MOTOR_SHOT, OUTPUT);
  pinMode(MOTOR_MIXER, OUTPUT);
}

void loop() {
  // Clock update
  clock = millis();

  // T FLipFlop
  buttonChoose = digitalRead(BUTTON_CHOOSE);
  if (buttonChoose == 1 && buttonChoose_prev == 0) {
    buttonChoose_prev = 1;
    mode_reg = !mode_reg;
  }

  else if (buttonChoose == 0 && buttonChoose_prev == 1) {
    buttonChoose_prev = 0;
  }
  //////////

  //////////////////////////////
  // Selection Screen //
  //////////////////////////////
  display.clearDisplay();
  display.setCursor(0, 5);
  display.print("Dick and Balls");
  display.setCursor(0, 25);
  display.print("Selected: #");
  display.print(shotCount);
  display.print(" || ");
  display.print(percentage);
  display.print("%");
  display.setCursor(0, 15);

  // // Cooldown Shower
  // if (buttonStart_prev) {
  //   display.setCursor(0, 35);
  //   display.print("Cooldown: ");
  //   display.print((cooldownEnd - clock) / 1000);
  //   display.print("s");
  // }

  // Read Pot
  pot_readout = analogRead(potPin);

  // Mode Switcher
  switch(mode_reg) {
    // Mode #1: Num of Shots
    case 1: 
      shotCount = (pot_readout * 5.0) / 1023;
      display.print("Number of Shots: #");
      display.print(shotCount);
      display.display();
      break;

    // Mode #2: Percent Mixer
    case 0: 
      percentage = (pot_readout * 100.0) / 1023;
      display.print("Mixer Percent: ");
      display.print(percentage);
      display.print("%");
      display.display();
      break;

    default:	
      display.clearDisplay();
      display.setCursor(0, 20);
      break;
  }

  //////////////////////////////
  // Pouring Sequence //
  //////////////////////////////
  buttonStart = digitalRead(BUTTON_START);

  if (buttonStart == 1 && buttonStart_prev == 0) {
    buttonStart_prev = 1;

    // Motor Timings
    shotTime = shotCount * SHOT_UNIT;
    mixerTime = percentage * (MIXER_MAX / 100);
    Serial.print(shotTime);
    Serial.print(" ");
    Serial.println(mixerTime);

    // Cooldown Setter (To prevent Overlap)
    cooldownEnd = clock + mixerTime + shotTime + COOLDOWN;

    display.clearDisplay();
    display.setCursor(0, 5);
    display.print("Dick and Balls");
    display.setCursor(0, 15);
    display.print("Selected: #");
    display.print(shotCount);
    display.print(" || ");
    display.print(percentage);
    display.print("%");
    display.setCursor(0, 25);
    display.print("Pouring...");
    display.display();

    digitalWrite(MOTOR_SHOT, HIGH);
    delay(shotTime);
    digitalWrite(MOTOR_SHOT, LOW);
    digitalWrite(MOTOR_MIXER, HIGH);
    delay(mixerTime);
    digitalWrite(MOTOR_MIXER, LOW);

    // Reset Mixer to 0
    percentage = 0;
  }

  else if (buttonStart == 0 && buttonStart_prev == 1 && (clock > cooldownEnd)) {
    buttonStart_prev = 0;
  }

  delay(100);
}

// // Maximum Number finder
// int max_num(int raw_input, int max_input) {
//   int input_max = max_input;
//   if (raw_input > max_input) {
//     input_max = raw_input;
//   }
//   return input_max;
// }
