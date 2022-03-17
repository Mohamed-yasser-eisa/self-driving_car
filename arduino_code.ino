int leftMotorForward = 6;    
int rightMotorForward = 4;   
int leftMotorBackward = 7;   
int rightMotorBackward = 5; 

int rightMotorENB = 3; 
int leftMotorENB = 2;  

int pin3 = A2;
int pin1 = A0;
int pin2 = A1;

int data1;
int data2;
int data3;

void setup() {
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT); 
  pinMode(leftMotorBackward, OUTPUT);  
  pinMode(rightMotorBackward, OUTPUT);
  pinMode(pin1,INPUT);
  pinMode(pin2,INPUT);
  pinMode(pin3,INPUT);
  pinMode(leftMotorENB, OUTPUT); 
  pinMode(rightMotorENB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
   // analogWrite(pin3,LOW);
    data1 = analogRead(pin1);
    data2 = analogRead(pin2);
    data3 = analogRead(pin3);
    Serial.println(data1);
    Serial.println(data2);
    Serial.println(data3);
    //delay(3000);
    if (data1 >500 && data2 > 500 && data3 > 500)
    { MotorForward();
      delay(300);
      digitalWrite(leftMotorENB,LOW);
      digitalWrite(rightMotorENB,LOW);
    }
    /* If the incoming data is "backward", run the "MotorBackward" function */
    else if (data1 < 500 && data2 > 500 && data3 > 500)
    {
      MotorBackward();
      delay(300);
      digitalWrite(leftMotorENB,LOW);
      digitalWrite(rightMotorENB,LOW);
      }
    /* If the incoming data is "left", run the "TurnLeft" function */
    else if (data1 > 500 && data2 < 500 && data3 > 500)
    {
      TurnLeft();
      delay(300);
      digitalWrite(leftMotorENB,LOW);
      digitalWrite(rightMotorENB,LOW);
    }
    /* If the incoming data ins "right", run the "TurnRight" function */
    else if (data1 < 500 && data2 < 500 && data3 > 500) 
    {
      TurnRight();
      delay(300);
      digitalWrite(leftMotorENB,LOW);
      digitalWrite(rightMotorENB,LOW);
    }
    /* If the incoming data is "stop", run the "MotorStop" function */
    else if (data1 < 500 && data2 < 500 && data3 < 500) 
    {
      MotorStop();
    }
}

/********************************************* FORWARD *****************************************************/
void MotorBackward(void)   
{
  analogWrite(leftMotorENB,150);
  analogWrite(rightMotorENB,250);
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  delay(100);
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  delay(100);
}

/********************************************* BACKWARD *****************************************************/
void MotorForward(void)   
{
  analogWrite(leftMotorENB,130);
  analogWrite(rightMotorENB,250);
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
  delay(100);
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  delay(50);

}

/********************************************* TURN LEFT *****************************************************/
void TurnRight(void)   
{
  analogWrite(leftMotorENB,150);
  analogWrite(rightMotorENB,0); 
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorBackward,HIGH); 
  delay(100);
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  delay(50);
}


/********************************************* TURN RIGHT *****************************************************/
void TurnLeft(void)   
{
  analogWrite(leftMotorENB,0);
  analogWrite(rightMotorENB,250);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  delay(100);
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  delay(50);

}

/********************************************* STOP *****************************************************/
void MotorStop(void)   
{
  digitalWrite(leftMotorENB,LOW);
  digitalWrite(rightMotorENB,LOW);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  Serial.println("stop");
}
