
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

//below are the metal sensors that are plugged into the analog ports
#define X_LEFT_SENSOR A4
#define X_RIGHT_SENSOR A0
#define Y_FRONT_SENSOR A3
#define Y_BACK_SENSOR A1
#define Z_BOTTOM_SENSOR A2

#define SENSOR_TRIPPED LOW

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
float tempX = 0, tempY = 0, tempZ = 0;

bool calibrated = false;

int stopped = false;

struct COORD{
  float x;
  float y;
  float z;
}currentPos;

const COORD maxCoord = {114,101,101};

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

  pinMode(X_LEFT_SENSOR, INPUT);
  pinMode(X_RIGHT_SENSOR, INPUT);
  pinMode(Y_FRONT_SENSOR, INPUT);
  pinMode(Y_BACK_SENSOR, INPUT);
  pinMode(Z_BOTTOM_SENSOR, INPUT);

  pinMode(13,OUTPUT);
  
  //Internal pullup resistor will invert signal
  pinMode(STOP_PIN, INPUT_PULLUP);

  calibrate();
}

void loop()
{
  if(!stopped){
    if (Serial.available()) {
      //Allow for transmission to finish
      delay(100);
      //Puts data in buffer lineRead
      readSerial();
      if(strcmp(lineRead,"vacon")==0){
        vacuumOn();
      }else if(strcmp(lineRead,"vacoff")==0){
        vacuumOff();
      }else if(strcmp(lineRead,"cal")==0){
        calibrate();
      }else{
        char *token, *string;
    
        string = lineRead;
        
        int iteration = 0;
        tempX = 0; 
        tempY = 0; 
        tempZ = 0;
        //pickup or dropoff
        char *type;
        char *modifier;
        
        //The following code is a particularly bad way of processing a string
        while ((token = strsep(&string, ",")) != NULL)
        {
          if(iteration==X){
            tempX = atof(token);
          }else if(iteration==Y){
            tempY = atof(token);
          }else if(iteration==2){
            type = token;
          }else if(iteration==3){
            modifier = token;
          }
          iteration++;
        }
        /*if (strcmp(type,"pickup")==0){
            pickup(tempX, tempY);
        }else if (strcmp(type,"dropoff")==0){
            dropoff(tempX, tempY);
        }else{*/
            tempZ = atof(type);
            if(strcmp(modifier,"abs")==0){
              absMove(tempX,tempY,tempZ);
            }else{
              movePos(tempX,tempY,tempZ);
            }
        //}
      }
      if(!stopped)
        Serial.print("\n");  //indicates that command has finished executing
    }
  }
}

//TODO rename
void moveDistance(float distance, int axis){
  rotate(abs(distance/DISTANCE_PER_TURN), axis, (distance<0)?CLOCKWISE:COUNTERCLOCKWISE);
}

void rotate(float angle, int axis, int dir){
  if(axis==Y){
    if(dir == COUNTERCLOCKWISE){
      digitalWrite(DIR_Y_PIN, HIGH);
    }else{
      digitalWrite(DIR_Y_PIN, LOW);
    }
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
    if(dir == COUNTERCLOCKWISE){
      digitalWrite(DIR_X_PIN, HIGH);
    }else{
      digitalWrite(DIR_X_PIN, LOW);
    }
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
    if(dir == CLOCKWISE){
      digitalWrite(DIR_Z_PIN, HIGH);
    }else{
      digitalWrite(DIR_Z_PIN, LOW);
    }
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
    if((byteRead = Serial.read()) != '\n'){
      lineRead[i] = byteRead;
    }else{
      break;
    }
  }
}

void vacuumOn()
{
  digitalWrite(VACUUM_PIN, LOW);  
  digitalWrite(13, HIGH);
}

void vacuumOff()
{
  digitalWrite(VACUUM_PIN, HIGH);
  digitalWrite(13,LOW);
}

/*char *pickup(float x, float y){
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
}*/
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

void movePos(float x, float y){
  movePos(x,y,0);
}

bool isColliding(bool dir, int axis){
  //variables:
  //dir -- either CLOCKWISE or COUNTERCLOCKWISE
  //axis -- X, Y, or Z
  
  //pretty bad code, improve if you want
  //basically, what this does is check to see if the 
  //sensor that's in the direction of motion
  //is detecting the presence of the platform.
  //if this function returns true, it means the
  //platform cannot move more or else it'll collide
  //with the boundaries.
  if(axis==X){
    if(dir==CLOCKWISE && digitalRead(X_RIGHT_SENSOR)==SENSOR_TRIPPED){
      return true;
    }
    if(dir==COUNTERCLOCKWISE && digitalRead(X_LEFT_SENSOR)==SENSOR_TRIPPED){
      currentPos.x=0;
      return true;
    }
  }else if(axis==Y){
    if(dir==CLOCKWISE && digitalRead(Y_BACK_SENSOR)==SENSOR_TRIPPED){
      return true;
    }
    if(dir==COUNTERCLOCKWISE && digitalRead(Y_FRONT_SENSOR)==SENSOR_TRIPPED){
      currentPos.y=0;
      return true;
    }
  }else if(axis==Z){
    if(dir==CLOCKWISE && digitalRead(Z_BOTTOM_SENSOR)==SENSOR_TRIPPED){
      currentPos.z=0;
      return true;
    }
  }
  return false;
}

void calibrate(){ 
  //this function is basically a clone of the movePos function,
  //but with each axis moving all the way in the negative direction.
  //look there if more documentation is needed.
  movePos(0,0,10);
  //trust me: replacing the following with movePos(INT_MIN,INT_MIN,INT_MIN) does not work.
  
  bool moveX = true;
  bool moveY = true;
  bool moveZ = true;
  bool xDir = COUNTERCLOCKWISE;
  bool yDir = COUNTERCLOCKWISE;
  bool zDir = CLOCKWISE;
  digitalWrite(DIR_X_PIN, LOW);
  digitalWrite(DIR_Y_PIN, LOW);
  digitalWrite(DIR_Z_PIN, HIGH);
  
  while(moveX || moveY){
     if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      if(isColliding(yDir,Y)){
        moveY = false;
      }
      if(isColliding(xDir,X)){
        moveX = false;
      }
      if(moveX){
        digitalWrite(CP_X_PIN, HIGH);
        digitalWrite(CP_X_PIN, LOW);
      }
      if(moveY){
        digitalWrite(CP_Y_PIN, HIGH);
        digitalWrite(CP_Y_PIN, LOW);
      }
  }

  while(moveZ){
      if(isStopped()){
          stopped = true;
          break;
      }
      delayMicroseconds(DELAY);
      if(isColliding(zDir,Z)){
        moveZ = false;
      }
      digitalWrite(CP_Z_PIN, HIGH);
      digitalWrite(CP_Z_PIN, LOW);
  }
  currentPos.x=0;
  currentPos.y=0;
  currentPos.z=0;
  calibrated = true;
  digitalWrite(13, HIGH);
}

void absMove(float x, float y, float z){
  if(!calibrated){
    calibrate();
  }
  movePos(x-currentPos.x,y-currentPos.y,z-currentPos.z);
}

void movePos(float x, float y, float z){
  if(calibrated){
    if(currentPos.x+x>maxCoord.x){
      return;
    }
    if(currentPos.y+y>maxCoord.y){
      return;
    }
    if(currentPos.z+z>maxCoord.z){
      return;
    }
    currentPos.x+=x;
    currentPos.y+=y;
    currentPos.z+=z;
  }
  
  int xTicks = abs(x*PULSES_PER_CIRCLE/DISTANCE_PER_TURN); //ticks are the number of pulses sent out to the motor drivers
  int yTicks = abs(y*PULSES_PER_CIRCLE/DISTANCE_PER_TURN);
  int zTicks = abs(z*PULSES_PER_CIRCLE/DISTANCE_PER_TURN)*2;
  int maxTicks = (xTicks>yTicks? (xTicks>zTicks? xTicks: zTicks): (yTicks>zTicks? yTicks:zTicks)); //nested ternary statements are not bad form, ok?
  
  bool xDir = (x<0)?COUNTERCLOCKWISE:CLOCKWISE; //counterclockwise is left
  //negative is left
  bool yDir = (y<0)?COUNTERCLOCKWISE:CLOCKWISE; //counterclockwise is backwards
  //negative is backwards
  bool zDir = (z<0)?CLOCKWISE:COUNTERCLOCKWISE; //for some reason, the z-axis goes backwards; counterclockwise is up
  //negative is down

  //setting motor direction
  if(xDir == CLOCKWISE){
      digitalWrite(DIR_X_PIN, HIGH);
  }else{
      digitalWrite(DIR_X_PIN, LOW);
  }
  if(yDir == CLOCKWISE){
      digitalWrite(DIR_Y_PIN, HIGH);
  }else{
      digitalWrite(DIR_Y_PIN, LOW);
  }
  if(zDir == CLOCKWISE){
      digitalWrite(DIR_Z_PIN, HIGH);
  }else{
      digitalWrite(DIR_Z_PIN, LOW);
  }

  //You don't need to understand this, but basically,
  //what's happening is the arduino is sending out a pulse signal
  //at a set frequency. Modifying DELAY makes the motor go faster
  //or slower depending on the pulse frequency.
  for(int i=0;i<maxTicks;i++){
     if(isStopped()){
        stopped = true;
        break;
      }
      delayMicroseconds(DELAY);
      if(isColliding(zDir,Z) && zTicks!=0){
        zTicks = 0;
        int maxTicks = (xTicks>yTicks? (xTicks>zTicks? xTicks: zTicks): (yTicks>zTicks? yTicks:zTicks)); //nested ternary statements are not bad form, ok?
      }
      if(isColliding(yDir,Y) && yTicks!=0){
        yTicks = 0;
        int maxTicks = (xTicks>yTicks? (xTicks>zTicks? xTicks: zTicks): (yTicks>zTicks? yTicks:zTicks)); //nested ternary statements are not bad form, ok?
      }
      if(isColliding(xDir,X) && xTicks!=0){
        xTicks = 0;
        int maxTicks = (xTicks>yTicks? (xTicks>zTicks? xTicks: zTicks): (yTicks>zTicks? yTicks:zTicks)); //nested ternary statements are not bad form, ok?
      }
      if(i<xTicks){
        digitalWrite(CP_X_PIN, HIGH);
        digitalWrite(CP_X_PIN, LOW);
      }
      if(i<yTicks){
        digitalWrite(CP_Y_PIN, HIGH);
        digitalWrite(CP_Y_PIN, LOW);
      }
      if(i<zTicks){
        digitalWrite(CP_Z_PIN, HIGH);
        digitalWrite(CP_Z_PIN, LOW);
      }
  }
}
