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
int laserTripped = false;

#define SENSOR_PIN 0

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

void checkLaser()
{
  int level = analogRead(SENSOR_PIN);
  Serial.println(level);

  if (level < 900)
  {
    // Beam is broken
    if (!laserTripped)
    {
      laserTripped = true;
      Serial.println("Laser noteOn");
      noteOn(0, notePitches[noteIndex], 127);
    }
    else
    {
      // Already on
    }
  }
  else if (level > 950)
  {
    if (laserTripped)
    {
      Serial.println("Laser noteOff");
      noteOff(0, notePitches[noteIndex], 0);
      noteIndex = (noteIndex + 1) % numButtons;
      laserTripped = false;
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
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(115200);
  numButtons = sizeof(notePitches);
}

void loop()
{
  checkLaser();
}