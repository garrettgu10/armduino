#define CP_X_PIN 2 //red
#define DIR_X_PIN 3 //yellow
#define EN_X_PIN 4 //green

#define CP_Y_PIN 5
#define DIR_Y_PIN 6
#define EN_Y_PIN 7

//pins out of order because of soldering mistake the pins are intentionally switched around
#define CP_Z_PIN 8
#define DIR_Z_PIN 10
#define EN_Z_PIN 9

#define VACUUM_PIN 11  //LOW signal maps to vacuum on

#define STOP_PIN 12    //LOW signal input means stopped, this is because the internal pullup resistor inverts the signal

#define X 0
#define Y 1
#define Z 2

#define COUNTERCLOCKWISE 0
#define CLOCKWISE 1

#define PULSES_PER_CIRCLE 1600
//7.22 cm per turn at 19V DC
#define DISTANCE_PER_TURN 7.22
//Delay is in microseconds
#define DELAY 1000

int byteRead = 0;
#define LINE_SIZE 64

//Distance to go up or down in pickup/dropoff subroutines (cm as always)
#define UPDOWN 3

//buffer for serial data
char lineRead[LINE_SIZE];

//Location of head
int posX = 0, posY = 0, posZ = 0;
int tempX = 0, tempY = 0, tempZ = 0;

int stopped = false;

void setup()
{
  Serial.begin(9600);
  pinMode(CP_Y_PIN, OUTPUT);
  pinMode(EN_Y_PIN, OUTPUT);
  pinMode(DIR_Y_PIN, OUTPUT);
  digitalWrite(CP_Y_PIN, LOW);
  digitalWrite(EN_Y_PIN, LOW);
  digitalWrite(DIR_Y_PIN, LOW);

  pinMode(CP_X_PIN, OUTPUT);
  pinMode(EN_X_PIN, OUTPUT);
  pinMode(DIR_X_PIN, OUTPUT);
  digitalWrite(CP_X_PIN, LOW);
  digitalWrite(EN_X_PIN, LOW);
  digitalWrite(DIR_X_PIN, LOW);

  pinMode(CP_Z_PIN, OUTPUT);
  pinMode(EN_Z_PIN, OUTPUT);
  pinMode(DIR_Z_PIN, OUTPUT);
  digitalWrite(CP_Z_PIN, LOW);
  digitalWrite(EN_Z_PIN, LOW);
  digitalWrite(DIR_Z_PIN, LOW);
  
  pinMode(VACUUM_PIN, OUTPUT);
  //high signal disables vacuum
  digitalWrite(VACUUM_PIN, HIGH);
  
  //Internal pullup resistor will invert signal
  pinMode(STOP_PIN, INPUT_PULLUP);
}

void loop()
{
  if(!stopped){
    if (Serial.available()) {
      //Allow for transmission to finish
      delay(100);
      //Puts data in buffer lineRead
      readSerial();
      if(strcmp(lineRead,"vacon")==0)
        vacuumOn();
      else if(strcmp(lineRead,"vacoff")==0)
        vacuumOff();
      else{
        char *token, *string;
    
        string = lineRead;
        int iteration = 0;
        tempX = 0; 
        tempY = 0; 
        tempZ = 0;
        //pickup or dropoff
        char *type;
        //The following code is a particularly bad way of processing a string
        while ((token = strsep(&string, ",")) != NULL)
        {
          if(iteration==X)
            tempX = atof(token);
          else if(iteration==Y)
            tempY = atof(token);
          else if(iteration==2)
            type = token;
          iteration++;
        }
        if (strcmp(type,"pickup")==0)
            pickup(tempX, tempY);
          else if (strcmp(type,"dropoff")==0)
            dropoff(tempX, tempY);
          else{
            tempZ = atof(type);
            absoluteMove(tempX,tempY,tempZ);
          }
      }
      if(!stopped)
        Serial.println();  //indicates that command has finished executing
    }
  }
}

//TODO rename
void moveDistance(float distance, int axis){
  rotate(abs(distance/DISTANCE_PER_TURN), axis, (distance<0)?CLOCKWISE:COUNTERCLOCKWISE);
}

void rotate(float angle, int axis, int dir){
  if(axis==Y){
    if(dir == COUNTERCLOCKWISE)
      digitalWrite(DIR_Y_PIN, HIGH);
    else
      digitalWrite(DIR_Y_PIN, LOW);
    for(int i=0;i<PULSES_PER_CIRCLE*angle;i++){
      if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      digitalWrite(CP_Y_PIN, HIGH);
      digitalWrite(CP_Y_PIN, LOW);
    }
  }
  //In practice the code for moving X and Y is located in the moveXY function
  //The code immediately below should never get called
  else if(axis==X){
    if(dir == COUNTERCLOCKWISE)
      digitalWrite(DIR_X_PIN, HIGH);
    else
      digitalWrite(DIR_X_PIN, LOW);
    for(int i=0;i<PULSES_PER_CIRCLE*angle;i++){
      if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      digitalWrite(CP_X_PIN, HIGH);
      digitalWrite(CP_X_PIN, LOW);
    }
  }
  else if(axis==Z){
    //intentional
    if(dir == CLOCKWISE)
      digitalWrite(DIR_Z_PIN, HIGH);
    else
      digitalWrite(DIR_Z_PIN, LOW);
    for(int i=0;i<PULSES_PER_CIRCLE*angle;i++){
      if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      digitalWrite(CP_Z_PIN, HIGH);
      digitalWrite(CP_Z_PIN, LOW);
    }
  }
}

void readSerial(){
  for(int i=0;i<LINE_SIZE;i++){
    lineRead[i] = 0;
  }
  for(int i=0;(i<LINE_SIZE && Serial.available());i++){
    /*terminates read on 64 characters, end of data, or newline
    **Newline should be used to terminate input
    */
    if((byteRead = Serial.read()) != '\n')
      lineRead[i] = byteRead;
    else
      break;
  }
}

char *relativeMove(float x, float y, float z){
  //If Z is negative move last
  //If Z is positive move first
  if(z>0)
    moveDistance(z, Z);
//  moveDistance(x, X);
//  moveDistance(y, Y);
    moveXY(x,y);
  if(z<0)
    moveDistance(z, Z);
    
  //Update stored postitions
  posX += x;
  posY += y;
  posZ += z;
  char location[64];
  snprintf(location, 64, "%f %f %f", posX, posY, posZ);
  return (location);
}

char *absoluteMove(float x, float y, float z){
  return(relativeMove(x-posX,y-posY,z-posZ)); 
}

void vacuumOn()
{
  digitalWrite(VACUUM_PIN, LOW);  
}

void vacuumOff()
{
  digitalWrite(VACUUM_PIN, HIGH);
}

char *pickup(float x, float y){
  relativeMove(0,0,UPDOWN);
  absoluteMove(x,y, posZ);
  relativeMove(0,0,-UPDOWN);
  vacuumOn();
  return "done";
}

char *dropoff(float x, float y){
  relativeMove(0,0,UPDOWN);
  absoluteMove(x,y, posZ);
  relativeMove(0,0,-UPDOWN);
  vacuumOff();
  return "done";
}
//time in ms
void disableMotors(int time){
  digitalWrite(EN_X_PIN,HIGH);
  digitalWrite(EN_Y_PIN,HIGH);
  //digitalWrite(EN_Z_PIN,HIGH);
  delay(time);
  digitalWrite(EN_X_PIN,LOW);
  digitalWrite(EN_Y_PIN,LOW);
  //digitalWrite(EN_Z_PIN,LOW);
}

boolean isStopped(){
  //Code could go here to indicate program failure
  //Serial.println("error"); or something like this
  //Inverted signal
  return (digitalRead(STOP_PIN)==HIGH)?false:true;
}

void moveXY(float x, float y){
  int xTicks = abs(x/DISTANCE_PER_TURN*PULSES_PER_CIRCLE);
  int yTicks = abs(y/DISTANCE_PER_TURN*PULSES_PER_CIRCLE);
  int maxTicks = (xTicks>yTicks)?xTicks:yTicks;
  int xDir = (x<0)?COUNTERCLOCKWISE:CLOCKWISE;
  int yDir = (y<0)?COUNTERCLOCKWISE:CLOCKWISE;
  if(xDir == CLOCKWISE)
      digitalWrite(DIR_X_PIN, HIGH);
  else
      digitalWrite(DIR_X_PIN, LOW);
  if(yDir == CLOCKWISE)
      digitalWrite(DIR_Y_PIN, HIGH);
  else
      digitalWrite(DIR_Y_PIN, LOW);
  for(int i=0;i<maxTicks;i++){
     if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      if(i<xTicks){
        digitalWrite(CP_X_PIN, HIGH);
        digitalWrite(CP_X_PIN, LOW);
      }
      if(i<yTicks){
        digitalWrite(CP_Y_PIN, HIGH);
        digitalWrite(CP_Y_PIN, LOW);
      }
  }
}
