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

byte commandByte;
byte noteByte = 0;
byte velocityByte;

boolean noteOnOrOff = false;

/*int redLed = 2;
int greenLed = 3;
int yellowLed = 4;
int whiteLed = 5;
int currentLed = 0;*/

int kickMotor = 1;
int snareMotor = 2;
int hihatMotor = 3;
int crashMotor = 4;


// Motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

//Adafruit_DCMotor *snareMotor = AFMS.getMotor(1);


void setup() {
  Serial.begin( 31250 );
  
  /*pinMode( redLed, OUTPUT );
  pinMode( greenLed, OUTPUT );
  pinMode( yellowLed, OUTPUT );
  pinMode( whiteLed, OUTPUT );*/
  
  AFMS.begin();
}

void checkMIDI() {
  
  if ( Serial.available() ) { // Do we have any serial data?
  
    if ( noteOnOrOff == true ) { // Last byte was a note ON or OFF
      noteByte = Serial.read();
    } else {
      commandByte = Serial.read();
    }
    
    if ( ( commandByte == 0x90 ) || ( commandByte == 0x80 ) ) { // Note ON or OFF
      noteOnOrOff = true;
    }
    
    if ( ( noteOnOrOff == true ) && ( noteByte != 0 ) ) { // We have a note ON or OFF and a note value
      velocityByte = Serial.read();
      
      int currentDrum = whichDrum( noteByte );
      
      if ( commandByte == 0x90 ) { // Note ON
        drum( currentDrum, true );
      } else if ( commandByte == 0x80 ) { // Note OFF
        drum( currentDrum, false );
      }
      
    }
    
  }
}

void drum( int myDrum, boolean hitOrRelease ) {
 
  Adafruit_DCMotor *myDrumMotor = AFMS.getMotor( myDrum );
  
  if ( hitOrRelease == true ) { // HIT
    myDrumMotor->setSpeed( 255 );
    myDrumMotor->run( FORWARD );
  } else { // RELEASE
    myDrumMotor->run( RELEASE );
  }
  
  noteOnOrOff = false;
  noteByte = 0;
  
}
/*    
      noteByte = Serial.read();
      velocityByte = Serial.read();
      currentLed = whichLed ( noteByte );
      
      if ( commandByte == 0x90 ) { // Note ON
        digitalWrite ( currentLed, HIGH );

        snareMotor->setSpeed ( velocityByte + 128 );
        snareMotor->run( FORWARD );
      } else { // Note OFF
        digitalWrite ( currentLed, LOW );
        snareMotor->run( RELEASE );
      }
    }
     
      

*/ 

void loop() {
  checkMIDI();
}

/*void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}*/

/*int whichLed ( byte nB ) {
  switch ( noteByte ) {
    case 0x24: // C1, Kick
      return redLed;
      break;    
      
    case 0x26: // D1, Snare
      return greenLed;
      hitSnare();
      break;
    
    case 0x2A: // F#1, HiHat
      return yellowLed;
      break;
      
    }
}*/

int whichDrum ( byte nB ) {
  
  switch ( nB ) {
    case 0x24: // C1, Kick
      return kickMotor;
      break;    
      
    case 0x26: // D1, Snare
      return snareMotor;
      // hitSnare();
      break;
    
    case 0x2A: // F#1, HiHat
      return hihatMotor;
      break;
      
    case 0x31: // Crash
      return crashMotor;
      break;
    
    default: // Other
      return 0;
    }

}

/*void hitSnare() {
  snareMotor->setSpeed( 255 );
  snareMotor->run( FORWARD );
}*/
