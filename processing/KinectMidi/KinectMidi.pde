// Daniel Shiffman
// Tracking the average location beyond a given depth threshold
// Thanks to Dan O'Sullivan

// https://github.com/shiffman/OpenKinect-for-Processing
// http://shiffman.net/p5/kinect/

import org.openkinect.freenect.*;
import org.openkinect.processing.*;
import themidibus.*; //Import the library

MidiBus myBus; // The MidiBus

// The kinect stuff is happening in another class
KinectTracker tracker;
Kinect kinect;

int screenWidth = 640;
int screenHeight = 520;

int maxNote = 127;
int maxVolume = 127;

void setup() {
  size(640, 520);
  kinect = new Kinect(this);
  tracker = new KinectTracker();
  myBus = new MidiBus(this, "IAC Bus 1", "IAC Bus 1"); // Create a new MidiBus with no input device and the default Java Sound Synthesizer as the output device.
}

void playNote(int pitch, int volume, int durationMs) {
  int channel = 0;
  int volumeCC = 7;
  int velocity = 127;
  
  //print("PlayNote:" );
  //print(pitch);
  //print(", ");
  //print(velocity);
  //println();
  
  //myBus.sendControllerChange(channel, volumeCC, 127);
  
  myBus.sendNoteOn(channel, pitch, velocity); // Send a Midi noteOn

  delay(durationMs);
  myBus.sendNoteOff(channel, pitch, velocity); // Send a Midi nodeOff
}

void delay(int time) {
  int current = millis();
  while (millis () < current+time) Thread.yield();
}

void draw() {
  background(255);

  // Run the tracking analysis
  tracker.track();
  // Show the image
  tracker.display();

  // Let's draw the raw location
  PVector v1 = tracker.getPos();
  fill(50, 100, 250, 200);
  noStroke();
  ellipse(v1.x, v1.y, 20, 20);
  
  int note = (int)map(v1.x, 0.0, screenWidth, 0, maxNote);
  int volume = (int)map(v1.y, 0.0, screenHeight, 0, maxVolume);
  
  playNote(note, volume, 500);

  // Let's draw the "lerped" location
  PVector v2 = tracker.getLerpedPos();
  fill(100, 250, 50, 200);
  noStroke();
  ellipse(v2.x, v2.y, 20, 20);

  // Display some info
  int t = tracker.getThreshold();
  fill(0);
  text("threshold: " + t + "    " +  "framerate: " + int(frameRate) + "    " + 
    "UP increase threshold, DOWN decrease threshold", 10, 500);
}

// Adjust the threshold with key presses
void keyPressed() {
  int t = tracker.getThreshold();
  if (key == CODED) {
    if (keyCode == UP) {
      t+=5;
      tracker.setThreshold(t);
    } else if (keyCode == DOWN) {
      t-=5;
      tracker.setThreshold(t);
    }
  }
}
