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
int drumMotors[ 4 ];

// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();



void setup() {
  // Initialize addresses to drums.
  drumMotors[ 0x24 ] = 1;
  drumMotors[ 0x26 ] = 2;
  drumMotors[ 0x2A ] = 3;
  drumMotors[ 0x31 ] = 4;
  
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
      int drumMotor = 0;
      getBytes( 2, values );
      drumMotor = getMotor( values[ 0 ] );
      
      if ( drumMotor > 0 ) {
        drum( drumMotor, values[ 1 ] );
      }
    }
  }
}

void drum( int myDrum, byte velocity ) {
  Adafruit_DCMotor *drumMotor = AFMS.getMotor( myDrum );
  
  if ( velocity > 0 ) { // HIT
    int motorSpeed = (byte)velocity + 128; // 255;
    
    drumMotor->setSpeed( motorSpeed );
    drumMotor->run( FORWARD );
  } else { // RELEASE
    drumMotor->run( RELEASE );
  }
}

int getMotor( byte nB ) {
  return drumMotors[ nB ];
}

void getBytes( int expectedBytes, byte savedBytes[] ) {
  int receivedBytes = 0;
  
  do {
    if ( Serial.available() ) {
      savedBytes[ receivedBytes ] = Serial.read();
      receivedBytes++;
    }
  } while( receivedBytes < expectedBytes );
}
