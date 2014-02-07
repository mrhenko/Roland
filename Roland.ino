/***************************************
 *                                    *
 *                                    *
 * Roland, the Headless MIDI Drummer  *
 *                                    *
 *                                    *
 **************************************/

// Include the motor shield
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

byte commandByte = 0;

// Initiate Motorshields.
Adafruit_MotorShield AFMS0 = Adafruit_MotorShield( 0x60 );
Adafruit_MotorShield AFMS1 = Adafruit_MotorShield( 0x61 );

// Declare pointers to motors.
Adafruit_DCMotor *kickMotor       = AFMS0.getMotor( 1 );
Adafruit_DCMotor *snareMotor      = AFMS0.getMotor( 2 );
Adafruit_DCMotor *hihatMotor      = AFMS0.getMotor( 3 );
Adafruit_DCMotor *hihatPedalMotor = AFMS0.getMotor( 4 );
Adafruit_DCMotor *tom1Motor       = AFMS1.getMotor( 1 );
Adafruit_DCMotor *tom2Motor       = AFMS1.getMotor( 2 );
Adafruit_DCMotor *crashMotor      = AFMS1.getMotor( 3 );
Adafruit_DCMotor *rideMotor       = AFMS1.getMotor( 4 );
Adafruit_DCMotor *currentMotor;

// Timestamps for note ON.
int kickHitTime  = -1;
int snareHitTime = -1;
int hihatHitTime = -1;
int tom1HitTime  = -1;
int tom2HitTime  = -1;
int crashHitTime = -1;
int rideHitTime  = -1;

const int modulusValue = 10000;
const int motorLimit = 2000;

// Use variables for NOTE ON and OFF to simplify the usage of channels.
int midiChannel = 10; // Channel 10
int noteOn      = 144 + midiChannel - 1;   // Minus 1 since MIDI Channels
int noteOff     = 128 + midiChannel - 1;   // aren't zero indexed but the
                                           // actual MIDI data is.

void setup() {
  // Setup serial interface for MIDI.
  Serial.begin( 31250 );

  // Initialize motor shields.
  AFMS0.begin();
  AFMS1.begin();
}

void loop() {
  // Check and release motors that been running longer than limit.
  kickHitTime  = timeLimit( kickHitTime,  kickMotor );
  snareHitTime = timeLimit( snareHitTime, snareMotor );
  hihatHitTime = timeLimit( hihatHitTime, hihatMotor );
  tom1HitTime  = timeLimit( tom1HitTime,  tom1Motor );
  tom2HitTime  = timeLimit( tom2HitTime,  tom2Motor );
  crashHitTime = timeLimit( crashHitTime, crashMotor );
  rideHitTime  = timeLimit( rideHitTime,  rideMotor );
  
  // Check for MIDI messages.
  checkMIDI();
}

int timeLimit( int hitTime, Adafruit_DCMotor *myMotor ) {
  // Check if drum has been triggered.
  if ( hitTime > -1 ) {
    int time = getTime( true );
    
    // Timestamp is less than current, set time to difference.
    if ( time < hitTime ) {
      time += modulusValue - hitTime;
    }
    
    // Has motor run longer than limit?
    if ( time - hitTime > motorLimit ) {
      // Turn off the drum.
      myMotor->run( RELEASE );
      return -1;
    }
  }
  
  return hitTime;
}

void checkMIDI() {
  // Check if we have any serial data.
  if ( Serial.available() ) {
    // Read first byte (command).
    commandByte = Serial.read();

    // Just listen for note on and off.
    if ( commandByte == noteOn || commandByte == noteOff ) {
      byte values[ 2 ];
      getBytes( 2, values );
      
      // Send MIDI note value and boolean wether it's a on or not.
      if ( getMotor( values[ 0 ], commandByte == noteOn ) ) {
        // Trigger drum if it should be on.
        drum( values[ 1 ] );
      }
    }
  }
}

void drum( byte velocity ) {
  if ( velocity > 0 ) { // HIT
    int motorSpeed = ( byte ) velocity + 128;
    
    currentMotor->setSpeed( motorSpeed );
    currentMotor->run( FORWARD );
  } else { // RELEASE
    currentMotor->run( RELEASE );
  }
}

boolean getMotor( byte nB, boolean isOn ) {
  switch ( nB ) {
    case 0x24: // C1, Kick
      currentMotor = kickMotor;
      kickHitTime = getTime( isOn );
      break;    
      
    case 0x26: // D1, Snare
      currentMotor = snareMotor;
      snareHitTime = getTime( isOn );
      break;
    
    case 0x29: // B1/C2/D2, Tom 2
    case 0x2B:
    case 0x2D:
      currentMotor = tom2Motor;
      tom2HitTime = getTime( isOn );
      break;
    
    case 0x2F: // F1/G1/A1, Tom 1
    case 0x30:
    case 0x32:
      currentMotor = tom1Motor;
      tom1HitTime = getTime( isOn );
      break;
    
    case 0x2A: // F#1, HiHat
      currentMotor = hihatMotor;
      hihatHitTime = getTime( isOn );
      break;
    
    case 0x2E: // A#1, Open HiHat
      currentMotor = hihatMotor;
      hihatHitTime = getTime( isOn );
      break;
    
    case 0x31: // C#2, Crash
      currentMotor = crashMotor;
      crashHitTime = getTime( isOn );
      break;
    
    case 0x33: // D#2, Ride
      currentMotor = rideMotor;
      rideHitTime = getTime( isOn );
      break;
    
    default: // Other, don't trigger any motors.
      return false;
  }
  
  // Set motor should be triggered.
  return true;
}

int getTime( boolean isOn ) {
  if ( isOn ) {
    return millis() % modulusValue;
  } else {
    return -1;
  }
}

void getBytes( int expectedBytes, byte savedBytes[] ) {
  int receivedBytes = 0;
  
  // Wait and read until received expected number of values.
  do {
    if ( Serial.available() ) {
      // Save values in supplied array.
      savedBytes[ receivedBytes ] = Serial.read();
      receivedBytes++;
    }
  } 
  while( receivedBytes < expectedBytes );
}

