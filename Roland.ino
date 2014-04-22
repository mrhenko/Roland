/***************************************
 *                                    *
 *                                    *
 * Roland, the Headless MIDI Drummer  *
 *                                    *
 *                                    *
 **************************************/

// Include the motor shield
#include <Wire.h>
#include <MIDI.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Initiate Motorshields.
Adafruit_MotorShield AFMS0 = Adafruit_MotorShield( 0x60 );
Adafruit_MotorShield AFMS1 = Adafruit_MotorShield( 0x61 );

// Declare pointers to motors.
Adafruit_DCMotor *kickMotor    = AFMS0.getMotor( 1 );
Adafruit_DCMotor *snareMotor   = AFMS0.getMotor( 2 );
Adafruit_DCMotor *tom1Motor    = AFMS0.getMotor( 4 );
Adafruit_DCMotor *tom2Motor    = AFMS0.getMotor( 3 );

Adafruit_DCMotor *crashMotor   = AFMS1.getMotor( 1 );
Adafruit_DCMotor *rideMotor    = AFMS1.getMotor( 2 );
Adafruit_DCMotor *hihatMotor   = AFMS1.getMotor( 3 );
Adafruit_DCMotor *cowbellMotor = AFMS1.getMotor( 4 );

Adafruit_DCMotor *currentMotor;

// Timestamps for note ON.
int kickHitTime    = -1;
int snareHitTime   = -1;
int hihatHitTime   = -1;
int tom1HitTime    = -1;
int tom2HitTime    = -1;
int crashHitTime   = -1;
int rideHitTime    = -1;
int cowbellHitTime = -1;

const int modulusValue = 10000;
const int motorLimit = 100;

// Use variables for NOTE ON and OFF to simplify the usage of channels.
int midiChannel = 10; // Channel 10

// This is used as a "scaling value" to scale
// the speed to a proper motor speed value.
int speedFloor = 150;

void setup() {
  // Initialize motor shields.
  AFMS0.begin();
  AFMS1.begin();
  
  // Setup serial interface for MIDI.
  MIDI.begin( midiChannel );
  MIDI.setHandleNoteOn( handleNoteOn );
  MIDI.setHandleNoteOff( handleNoteOff );
}

void loop() {
  // Check and release motors that been running longer than limit.
  kickHitTime    = timeLimit( kickHitTime,    kickMotor );
  snareHitTime   = timeLimit( snareHitTime,   snareMotor );
  hihatHitTime   = timeLimit( hihatHitTime,   hihatMotor );
  tom1HitTime    = timeLimit( tom1HitTime,    tom1Motor );
  tom2HitTime    = timeLimit( tom2HitTime,    tom2Motor );
  crashHitTime   = timeLimit( crashHitTime,   crashMotor );
  rideHitTime    = timeLimit( rideHitTime,    rideMotor );
  cowbellHitTime = timeLimit( cowbellHitTime, cowbellMotor );
  
  // Check for MIDI messages.
  MIDI.read();
}

void handleNoteOn( byte channel, byte pitch, byte velocity ) {
  if ( getMotor( pitch, true ) ) {
    drum( velocity );
  }
}

void handleNoteOff( byte channel, byte pitch, byte velocity ) {
  if ( getMotor( pitch, false ) ) {
    drum( 0 );
  }
}

void drum( byte velocity ) {
  if ( velocity > 0 ) { // HIT
    int motorSpeed = map( (int) velocity, 0, 127, speedFloor, 255 );
    
    currentMotor->setSpeed( motorSpeed );
    currentMotor->run( FORWARD );
  } else { // RELEASE
    currentMotor->run( RELEASE );
  }
}

boolean getMotor( byte pitch, boolean noteOn ) {
  switch ( pitch ) {
    case 0x24: // C1, Kick
      currentMotor = kickMotor;
      kickHitTime = getTime( noteOn );
      break;
      
    case 0x26: // D1, Snare
      currentMotor = snareMotor;
      snareHitTime = getTime( noteOn );
      break;
    
    case 0x29: // B1/C2/D2, Tom 2
    case 0x2B:
    case 0x2D:
      currentMotor = tom1Motor;
      tom1HitTime = getTime( noteOn );
      break;
    
    case 0x2F: // F1/G1/A1, Tom 1
    case 0x30:
    case 0x32:
      currentMotor = tom2Motor;
      tom2HitTime = getTime( noteOn );
      break;
    
    case 0x2A: // F#1, HiHat
      currentMotor = hihatMotor;
      hihatHitTime = getTime( noteOn );
      break;
    
    case 0x2E: // A#1, Open HiHat
      currentMotor = hihatMotor;
      hihatHitTime = getTime( noteOn );
      break;
    
    case 0x31: // C#2, Crash
      currentMotor = crashMotor;
      crashHitTime = getTime( noteOn );
      break;
    
    case 0x33: // D#2, Ride
      currentMotor = rideMotor;
      rideHitTime = getTime( noteOn );
      break;
    
    default: // Other, don't trigger any motors.
      return false;
  }
  
  // Set motor should be triggered.
  return true;
}

int timeLimit( int hitTime, Adafruit_DCMotor *myMotor ) {
  // Check if drum has been triggered.
  if ( hitTime > -1 ) {
    int time = getTime( true );
    int runTime = time - hitTime;
    
    // Timestamp is less than current, set time to difference.
    if ( time < hitTime ) {
      runTime = time + modulusValue - hitTime;
    }
    
    // Has motor run longer than limit?
    if ( runTime > motorLimit ) {
      // Turn off the drum.
      myMotor->run( RELEASE );
      return -1;
    }
  }
  
  return hitTime;
}

int getTime( boolean isOn ) {
  if ( isOn ) {
    return millis() % modulusValue;
  } else {
    return -1;
  }
}
