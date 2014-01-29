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
byte noteByte = 0;
byte velocityByte = 0;

boolean noteOnOrOff = false;

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
  if ( Serial.available() ) { // Do we have any serial data?
  
    if ( ( noteOnOrOff == true ) && ( noteByte != 0 ) ) { // We have a note ON or OFF and a note value
      velocityByte = Serial.read();
      
      int currentDrum = whichDrum( noteByte );
      
      if ( commandByte == 0x90 ) { // Note ON
        drum( currentDrum, true );
      } else if ( commandByte == 0x80 ) { // Note OFF
        drum( currentDrum, false );
      }
      
    }
  
    if ( noteOnOrOff == true ) { // Last byte was a note ON or OFF
      noteByte = Serial.read();
    } else {
      commandByte = Serial.read();
    }
    
    if ( ( commandByte == 0x90 ) || ( commandByte == 0x80 ) ) { // Note ON or OFF
      noteOnOrOff = true;
    }
  }
}

void drum( int myDrum, boolean hitOrRelease ) {
 
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
  
}

int whichDrum( byte nB ) {
  return drumMotors[ nB ];
}
