
void setup(){
  //Serial.begin(9600);
  pinMode(13, OUTPUT); 

}

void loop(){
  delay(1000);
  //read the pushbutton value into a variable
  int sensorVal = 0;
  //OR all the input together
  for(int i=8;i<=12;i++){
    if(digitalRead(i))
      Serial.println("h");
    else{
      sensorVal = HIGH;
      Serial.println("l");
    }
  }
  //Inverted logic
  if (sensorVal == HIGH) {
    digitalWrite(13,LOW);
    Serial.println("lowout");
  } 
  else {
    digitalWrite(13,HIGH);
    Serial.println("highout");
  }
}



