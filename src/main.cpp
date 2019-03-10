#include "MIDIUSB.h"
#include "PitchToNote.h"

const byte notePitches[] = {
    D3,
    D3,
    D3,
    G3,
    D4,
    C4,
    B3,
    A3,
    G4,
    D4,
    C4,
    B3,
    A3,
    G4,
    D4,
    C4,
    B3,
    C4,
    A3};
// Promethean scale
// const byte notePitches[numButtons] = {F3, G3b, B3b, E3, A3, D3};
int numButtons = 0;
uint8_t noteIndex = 0;
int laserTripped[3];

#define SENSOR_PIN_1 0
#define SENSOR_PIN_2 1
#define SENSOR_PIN_3 2
#define THRESHOLD 700

// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity)
{
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void checkLaser(int pin)
{
  int level = analogRead(pin);
  Serial.print("Level: ");
  Serial.print(level);
  Serial.print(" on pin: ");
  Serial.println(pin);

  if (level < THRESHOLD)
  {
    // Beam is broken
    if (!laserTripped[pin])
    {
      laserTripped[pin] = true;
      Serial.print("Laser noteOn - pin ");
      Serial.println(pin);
      noteOn(pin, notePitches[noteIndex], 127);
    }
    else
    {
      // Already on
    }
  }
  else
  {
    if (laserTripped[pin])
    {
      Serial.print("Laser noteOff - pin ");
      Serial.println(pin);
      noteOff(pin, notePitches[noteIndex], 0);
      noteIndex = (noteIndex + 1) % numButtons;
      laserTripped[pin] = false;
    }
    else
    {
      // Already off
    }
  }
  MidiUSB.flush();
}
void setup()
{
  pinMode(SENSOR_PIN_1, INPUT);
  pinMode(SENSOR_PIN_2, INPUT);
  pinMode(SENSOR_PIN_3, INPUT);

  laserTripped[SENSOR_PIN_1] = false;
  laserTripped[SENSOR_PIN_2] = false;
  laserTripped[SENSOR_PIN_3] = false;

  Serial.begin(115200);
  numButtons = sizeof(notePitches);
}

void loop()
{
  checkLaser(SENSOR_PIN_1);
  checkLaser(SENSOR_PIN_2);
  checkLaser(SENSOR_PIN_3);
}