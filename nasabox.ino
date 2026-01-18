#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define SHIELD_RESET  -1
#define SHIELD_CS     7
#define SHIELD_DCS    6
#define CARDCS        4
#define DREQ          3

const int RED_PIN   = 9;
const int GREEN_PIN = 10;
const int BLUE_PIN  = 5;
const int ROCKER    = 12;

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void printDirectory(File dir, int numTabs);

void setup() {
  Serial.begin(9600);

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(ROCKER, INPUT_PULLUP);

  Serial.println(F("Adafruit VS1053 Simple Test"));

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

  Serial.println(F("Starting countdown track"));
  
  musicPlayer.startPlayingFile("/countdo.mp3");
}

void loop() {
  int buttonState = digitalRead(ROCKER);

  
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);

  if (buttonState == LOW) { /
    Serial.println(F("State 1"));
  } else {
    Serial.println(F("State 2"));
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
