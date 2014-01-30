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
      getBytes( 2, values );
    }
  }
}

/*void drum( int myDrum, boolean hitOrRelease ) {
 
  Adafruit_DCMotor *myDrumMotor = AFMS.getMotor( myDrum );
  
  if ( hitOrRelease == true ) { // HIT
  
    int motorSpeed = 0; // 255;
    
    if ( velocityByte > 0 ) {
      motorSpeed = (byte)velocityByte + 128;
      //motorSpeed = 20;
    }
  
    myDrumMotor->setSpeed( motorSpeed );
    myDrumMotor->run( FORWARD );
  } else { // RELEASE
    myDrumMotor->run( RELEASE );
  }
  
  noteOnOrOff = false;
  noteByte = 0;
  velocityByte = 0;
  
}*/

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
