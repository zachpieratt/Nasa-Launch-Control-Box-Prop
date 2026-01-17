

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>


#define SHIELD_RESET -1
#define SHIELD_CS 7
#define SHIELD_DCS 6

#define CARDCS 4
#define DREQ 3

Adafruit_VS1053_FilePlayer musicPlayer =

  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Simple Test");

  if (!musicPlayer.begin()) {  // start music player
    Serial.println(F("Couldn't find VS1053."));
    while (1)
      ;
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1)
      ;
  }

  // list files on sd card
  printDirectory(SD.open("/"), 0);

  // Set volume for left,right channels
  musicPlayer.setVolume(0, 0);

  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);

  Serial.println(F("Playing countdown track"));
  musicPlayer.playFullFile("/countdo.mp3");
}

void loop() {
  if (musicPlayer.stopped()) {
    Serial.println("No sounds effects playing");
    while (1) {
      delay(10);
    }
  }
  if (Serial.available()) {
    char c = Serial.read();

    if (c == 's') {
      musicPlayer.stopPlaying();
    }

    if (c == 'p') {
      if (!musicPlayer.paused()) {
        Serial.println("Paused");
        musicPlayer.pausePlaying(true);
      } else {
        Serial.println("Resumed");
        musicPlayer.pausePlaying(false);
      }
    }
  }

  delay(100);
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
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
