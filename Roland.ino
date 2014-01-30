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

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *kickMotor = AFMS.getMotor( 1 );
Adafruit_DCMotor *snareMotor = AFMS.getMotor( 2 );
Adafruit_DCMotor *hihatMotor = AFMS.getMotor( 3 );
Adafruit_DCMotor *crashMotor = AFMS.getMotor( 4 );
Adafruit_DCMotor *currentMotor;

void setup() {
  // Setup serial interface for MIDI.
  Serial.begin( 31250 );

  // Initialize motor shield.
  AFMS.begin();
}

void loop() {
  checkMIDI();
}

void checkMIDI() {
  // Check if we have any serial data.
  if ( Serial.available() ) {
    // Read first byte (command).
    commandByte = Serial.read();

    // Just listen for note on and off.
    if ( commandByte == 0x90 || commandByte == 0x80 ) {
      byte values[ 2 ];
      getBytes( 2, values );
      
      if ( getMotor( values[ 0 ] ) ) {
        drum( values[ 1 ] );
      }
    }
  }
}

void drum( byte velocity ) {
  if ( velocity > 0 ) { // HIT
    int motorSpeed = ( byte ) velocity + 128; // 255;

    currentMotor->setSpeed( motorSpeed );
    currentMotor->run( FORWARD );
  } 
  else { // RELEASE
    currentMotor->run( RELEASE );
  }
}

boolean getMotor( byte nB ) {
  switch ( nB ) {
    case 0x24: // C1, Kick
      currentMotor = kickMotor;
      break;    
      
    case 0x26: // D1, Snare
      currentMotor = snareMotor;
      break;
    
    case 0x2A: // F#1, HiHat
      currentMotor = hihatMotor;
      break;
      
    case 0x31: // Crash
      currentMotor = crashMotor;
      break;
    
    default: // Other
      return false;
  }
  
  return true;
}

void getBytes( int expectedBytes, byte savedBytes[] ) {
  int receivedBytes = 0;

  do {
    if ( Serial.available() ) {
      savedBytes[ receivedBytes ] = Serial.read();
      receivedBytes++;
    }
  } 
  while( receivedBytes < expectedBytes );
}

