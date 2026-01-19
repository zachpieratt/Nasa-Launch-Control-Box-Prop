#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

enum Mode : uint8_t {
  STARTUP,
  POWERUP,
  LAUNCH
};

Mode currentMode = STARTUP;

#define SHIELD_RESET  -1
#define SHIELD_CS     7
#define SHIELD_DCS    6
#define CARDCS        4
#define DREQ          3

const int GREEN_PIN   = 9;
const int RED_PIN = 10;
const int BLUE_PIN  = 5;
const int ROCKER    = A0;

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void printDirectory(File dir, int numTabs);

int lastState = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(ROCKER, INPUT_PULLUP);

  lastState = digitalRead(ROCKER);


  if (!musicPlayer.begin()) {
    Serial.println(F("Couldn't find VS1053."));
    while (1) { delay(10); }
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1) { delay(10); }
  }

  // list files on sd card
  printDirectory(SD.open("/"), 0);

  
  musicPlayer.setVolume(0, 0);

  
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  
  musicPlayer.playFullFile("/track002.mp3");

  //rgb led should start off
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);

  currentMode = POWERUP;
}

void loop() {
  int reading = digitalRead(ROCKER);
if (reading != lastState) {
  delay(20);                    
  int confirm = digitalRead(ROCKER);
  if (confirm != lastState) {   
    lastState = confirm;

    if (currentMode == POWERUP) {
      powerUp();
    } else if (currentMode == LAUNCH) {
      launch();
    }
  }
}

  
  if (!musicPlayer.playingMusic) {
  }

  if (Serial.available()) {
    char c = Serial.read();

    if (c == 's') {
      Serial.println(F("Stop"));
      musicPlayer.stopPlaying();
    } else if (c == 'p') {
      if (!musicPlayer.paused()) {
        Serial.println(F("Paused"));
        musicPlayer.pausePlaying(true);
      } else {
        Serial.println(F("Resumed"));
        musicPlayer.pausePlaying(false);
      }
    }
  }

  delay(50);
}

void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;

    for (uint8_t i = 0; i < numTabs; i++) Serial.print('\t');
    Serial.print(entry.name());

    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void powerUp() {
  musicPlayer.playFullFile("/track005.mp3");
  delay(2000);

  digitalWrite(RED_PIN, HIGH);
  musicPlayer.playFullFile("/track006.mp3");
  delay(7000);

  musicPlayer.playFullFile("/track008.mp3");
  delay(2000);
  digitalWrite(GREEN_PIN, HIGH);
  musicPlayer.playFullFile("/track007.mp3");
  
  currentMode = LAUNCH;
  lastState = digitalRead(ROCKER);
}

void launch() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
  musicPlayer.playFullFile("/track001.mp3");
  delay(3000);
  musicPlayer.playFullFile("/track004.mp3");
  delay(7000);
  musicPlayer.playFullFile("/track003.mp3");
}
