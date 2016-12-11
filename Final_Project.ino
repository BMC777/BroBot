#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeMCore.h>

MeUltrasonicSensor ultr( PORT_3 );
MeDCMotor MotorL( M1 );
MeDCMotor MotorR( M2 );
MePort port( PORT_4 );
Servo sonicServo;

int16_t servopin = port.pin1();
int16_t moveSpeed = 0;

float targetX = 0;
float targetY = 0;
float posX = 0;
float posY = 0;
float timeDelta = 0;
float prevTime = 0;
float currentTime = 0;
float distanceToTargetX = 0;
float distanceToTargetY = 0;

float timer = 0;

enum
{
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD,
  ON_TARGET,
};

uint8_t moveState = FORWARD;
uint8_t relativeMoveState = FORWARD;
uint8_t servoState = FORWARD;

void Forward()
{
  MotorL.run(-moveSpeed * 1.05);
  MotorR.run(moveSpeed);
}
void Backward()
{
  MotorL.run(moveSpeed); 
  MotorR.run(-moveSpeed);
}
void TurnLeft()
{
  MotorL.run(-moveSpeed/2.2);
  MotorR.run(moveSpeed);
}
void TurnRight()
{
  MotorL.run(-moveSpeed);
  MotorR.run(moveSpeed/5);
}
void BackwardAndTurnLeft()
{
  MotorL.run(moveSpeed/8 ); 
  MotorR.run(-moveSpeed);
}

void BackwardAndTurnRight()
{
  MotorL.run(moveSpeed); 
  MotorR.run(-moveSpeed/8);
}
void Stop()
{
  //rgb.setColor(0,0,0);
  //rgb.show();
  MotorL.run(0);
  MotorR.run(0);
}

void lookLeft()
{
  sonicServo.write( 180 );
  servoState = LEFT;
}

void lookRight()
{
  sonicServo.write( 0 );
  servoState = RIGHT;
}

void lookForward()
{
  sonicServo.write( 75 );
  servoState = FORWARD;
}

bool checkClearPath()
{
  uint8_t d = ultr.distanceCm(70);
  static long time = millis();
  
  Serial.print("Distance:");
  Serial.print( ultr.distanceCm() );
  Serial.println(" cm");

  if (d > 40 || d == 0 )
  {
    return true;
  }

  return false;
}

bool checkFarObject()
{
  uint8_t d = ultr.distanceCm(70);
  static long time = millis();
  
  Serial.print("Distance:");
  Serial.print( ultr.distanceCm() );
  Serial.println(" cm");

  if ( (d > 10) && (d < 50) )
  {
    return true;
  }

  return false;
}

void moveToTarget()
{
  randomSeed(analogRead(6));
  uint8_t randNumber = random(2);

  //readSerial();
  Serial.print("Distance:");
  Serial.print( ultr.distanceCm() );
  Serial.println(" cm");
  Serial.println();
  Serial.print(" PositionX: " );
  Serial.print( posX );
  Serial.print( " PositionY: ");
  Serial.println( posY );
  Serial.println();
  //delay(1000);

  if ( distanceToTargetX < 1 && abs(posY) < 1 )
  {
    Stop();
    moveState == ON_TARGET;
  }
  else if ( moveState == FORWARD )
  {
    if ( posY < 0.2 && posY > -0.2 )
    {
      lookForward();
      delay(300);
      if ( checkClearPath() )
      {
         Forward();
      }
    }
    else if ( posY < -0.2 )
    {
      lookForward();
      delay(300);
      if ( checkClearPath() )
      {
        Forward();
        delay(500);
      }
      
      Stop();
      lookLeft();
      delay(500);
      
      if ( checkClearPath() )
      { 
        lookForward();
        TurnLeft();
        moveState = LEFT;
        delay(700);
        return;
      }
    }
    else if ( posY > 0.2 )
    {
      if ( checkClearPath() )
      {
        Forward();
        delay(500);
      }
      
      Stop();
      lookRight();
      delay(500);
      
      if ( checkClearPath() )
      {
        lookForward();
        TurnRight();
        moveState = RIGHT;
        delay(700);
        return;
      }
    }

    lookForward();
    delay(300);
    if ( checkClearPath() )
    {
      Forward();
    }
    else if ( checkFarObject() ) 
    {
      Stop();
      lookLeft();
      delay(500);
      
      if ( checkClearPath() )
      { 
        lookForward();
        TurnLeft();
        moveState = LEFT;
        delay(700);
      }
      else
      {
        lookRight();
        delay(500);
        if ( checkClearPath() )
        {
          lookForward();
          TurnRight();
          moveState = RIGHT;
          delay(700);
        }
      }
    }
    else
    {
      Stop();
      /*lookLeft();
      delay(500);
      if ( checkClearPath() )
      {  
        BackwardAndTurnLeft();
        moveState = LEFT;
        delay(1000);
      }
      else
      {
        Stop();
        lookRight();
        delay(500);
        if ( checkClearPath() )
        {
          BackwardAndTurnRight();
          moveState = RIGHT;
          delay(1000);
        }
      }*/
    }
  }
  else if ( moveState == LEFT )
  {
    lookForward();
      Forward();
      delay(300);
      
    if ( posY < 0.3 )
    {
      if( checkClearPath() )
      {
        Forward();
        delay(700);
        return;
      }
    }

    delay(200);
    Stop();
    lookRight();
    delay(500);
    if ( checkClearPath() )
    {
      lookForward();
      TurnRight();
      moveState = FORWARD;
      
    }
    else
    {
      lookForward();
      delay(500);
      if ( checkClearPath() )
      {
        Forward();
      }
    }
  }
  else if ( moveState == RIGHT )
  {
    lookForward();
    Forward();
    delay(300);
    
    if ( posY > -0.3 )
    {
      if ( checkClearPath() )
      {
        Forward();
        delay(700);
        return;
      }
    }

    delay(200);
    Stop();
    lookLeft();
    delay(500);
    if ( checkClearPath() )
    {
      lookForward();
      TurnLeft();
      moveState = FORWARD;
      
    }
    else
    {
      lookForward();
      delay(300);
      if ( checkClearPath() )
      {
        Forward();
      }
    }
  }
  
  delay(700);
}



void wander()
{
  uint8_t d = ultr.distanceCm(70);
  static long time = millis();
  randomSeed(analogRead(6));
  uint8_t randNumber = random(2);

  //readSerial();
  Serial.print("Distance:");
  Serial.print( ultr.distanceCm() );
  Serial.println(" cm");
  //delay(1000);
  
  if (d > 40 || d == 0)
  {
    Forward();
  }
  else if ((d > 15) && (d < 40)) 
  {
    //buzzer.tone(NTD2, 300);
    switch (randNumber)
    {
      case 0:
        TurnLeft();
        delay(300);
        break;
      case 1:
        TurnRight();
        delay(300);
        break;
    }
  }
  else
  {
    switch (randNumber)
    {
      case 0:
        BackwardAndTurnLeft();
        delay(800);
        break;
      case 1:
        BackwardAndTurnRight();
        delay(800);
        break;
    }
  }
  delay(100);
}

void setTarget( float x, float y )
{
  targetX = x;
  targetY = y;
}

void updatePos( float timeDelta )
{
  switch ( moveState )
  {
    case FORWARD:
      posX += timeDelta * 0.005;
      break;
    case BACKWARD:
      posX -= timeDelta * 0.005;
      break;
    case LEFT:
      posY += timeDelta * 0.005;
      break;
    case RIGHT:
      posY -= timeDelta * 0.005;
      break;
  }

  distanceToTargetX = targetX - posX;
}

void setup() 
{
  sonicServo.attach(servopin);
  Serial.begin( 115200 );
  moveSpeed = 120;
  // put your setup code here, to run once:
  sonicServo.write(75);

  setTarget( 15, 0 );


  posX = 0;
  posY = 0;
  timeDelta = 0;
  prevTime = 0;
  currentTime = 0;
  distanceToTargetX = 0;
  distanceToTargetY = 0;
  timer = 0;
}

void loop() 
{
  currentTime = millis() / 100;

  if (moveState != ON_TARGET )
  {
    updatePos( currentTime - prevTime );
    moveToTarget();
  }
  
  // wander();
  // put your main code here, to run repeatedly:
}
