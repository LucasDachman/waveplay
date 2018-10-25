
/* Touch Sensing */
#include <CapacitiveSensor.h>

const int THRESHOLD = 1000;
const int TIMEOUT = 20;
//const int CAL_TIMEOUT = 5000;
const unsigned long CAL_TIMEOUT = 0xFFFFFFFF;
const int RESOLUTION = 20;
const int NUM_KEYS = 12;
const int SEND_PIN = 0;

bool isAnyKeyPressed = false;

// on or off states
bool keyStates[12];
bool lastKeyStates[12];

// sensor objects
CapacitiveSensor keys[] = {CapacitiveSensor(SEND_PIN, 1), // 0
                           CapacitiveSensor(SEND_PIN, 4), // 1
                           CapacitiveSensor(SEND_PIN, 5), // 2
                           CapacitiveSensor(SEND_PIN, 6), // 3
                           CapacitiveSensor(SEND_PIN, 7), // 4
                           CapacitiveSensor(SEND_PIN, 8), // 5
                           CapacitiveSensor(SEND_PIN, 9), // 6
                           CapacitiveSensor(SEND_PIN, 10), // 7
                           CapacitiveSensor(SEND_PIN, 11), // 8
                           CapacitiveSensor(SEND_PIN, 12), // 9
                           CapacitiveSensor(SEND_PIN, 13), // 10
                           CapacitiveSensor(SEND_PIN, 14) // 11
                           };
const int blacklist[] = {1, 8, 9, 10};
const int blacklistSize = 4;

bool isBlacklist(int val) {
  for (int i = 0; i < blacklistSize; i++) {
    if (blacklist[i] == val) {
      return true;
    }
  }
  return false;
}

void setUpKeys()
{
  for (int i = 0; i < NUM_KEYS; i++)
  {
    keys[i].set_CS_AutocaL_Millis(CAL_TIMEOUT);
    keys[i].set_CS_Timeout_Millis(TIMEOUT);
    keyStates[i] = false;
    lastKeyStates[i] = false;
  }
}

void readKeys() {
  isAnyKeyPressed = false;
    for (int i = 0; i < NUM_KEYS; i++) {
      long val = keys[i].capacitiveSensor(RESOLUTION);
      if (!isBlacklist(i) && (val > THRESHOLD || val < 0)) {
        // note on
        isAnyKeyPressed = true;
        keyStates[i] = true;
        Serial.print(i);
        Serial.print(": ");
        Serial.print(val);
        Serial.print("\t\t");
      }
      else {
        keyStates[i] = false;
        Serial.print("\t\t");
      }
    }
    Serial.println();
}

/* Audio */
#include <SD.h>
#include <ArduinoSound.h>

const int sampleRate = 44100;
const int amplitude = 10;
const char *fileNames[] = {
  "S1-A.wav",
  "S1-A#.wav",
  "S1-B.wav",
  "S1-C.wav",
  "S1-C#.wav",
  "S1-D.wav",
  "S1-D#.wav",
  "S1-E.wav",
  "S1-F.wav",
  "S1-F#.wav",
  "S1-G.wav",
  "S1-G#.wav"
};

// from SD lib
SDWaveFile waveFiles[NUM_KEYS];

void setUpAudio() {
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while(1);
  }

  // read in wave files
  for (int i = 0; i < NUM_KEYS; i++) {
    waveFiles[i] = SDWaveFile(fileNames[i]);

    if (!waveFiles[i]) {
      Serial.print("invalid wave: ");
      Serial.println(i);
      while(1);
    }
    if (!AudioOutI2S.canPlay(waveFiles[i]))
    {
      Serial.print("unable to play wave file using I2S!");
      Serial.println(i);
      while (1);
    }
  }

  AudioOutI2S.volume(50.0);
}

/* Arduino lifecycle */

void setup()
{
  Serial.begin(9600);
  setUpKeys();
  setUpAudio();
}

void loop()
{
  readKeys();
  for (int i = 0; i < NUM_KEYS; i++) {
    if (keyStates[i] == true && lastKeyStates[i] == false) {
      // play audio clip
      Serial.println("pre play");
      AudioOutI2S.play(waveFiles[i]);
      Serial.println("post play");
      lastKeyStates[i] = true;
    }
    else if (keyStates[i] == false) {
      lastKeyStates[i] = false;
    }
  }
  if (!isAnyKeyPressed)
    Serial.println("Audio stop");
    AudioOutI2S.stop();
}
