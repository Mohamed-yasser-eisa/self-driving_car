/* include libraries */
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 16
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.


WiFiClient client;
WiFiServer server(80);

const char* ssid = "adel";
const char* password = "vwnh60511";

//to store the action in string
String  data =""; 

int pin3 = 14;
int pin1 = 12;
int pin2 = 13;

int trigPin = 5;            
int echoPin = 16;            
float duration, dist;


void setup()
{
  //define 3 pins as output to control arduino to control the car
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  //ultrasound pin
  pinMode(trigPin, OUTPUT); // define trigger pin as output
 
  
  Serial.begin(115200);
  connectWiFi();
  //start server
  server.begin();
  // Initiate  SPI bus
  SPI.begin();     
  // Initiate MFRC522 
  mfrc522.PCD_Init();   
  Serial.println("Approximate your card to the reader...");
  Serial.println();
}

void loop()
{
  //check rfid tag
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  /* If the server available, run the "checkClient" function */  
  client = server.available();
  //send the tag to client
  client.print(content.substring(1)); 
  
    
  if (!client) return; 
  data = checkClient ();
    
    
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // set the trigger pin HIGH for 10μs
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);  // measure the echo time (μs)
  dist = (duration/2.0)*0.0343;   // convert echo time to distance (cm)
 
  
  /************************ Run function according to incoming data from application *************************/

    
    //if ((data == "straight" || data == "turnright" ||data == "turnleft") && dist <= 30) //for auto mode
    if (data == "?pin=11")
    {
      MotorForward();
      //for auto mode
      /* TurnRight(); 
      delay(1000);
      MotorForward();*/
    }

    //else if (data == "straight" && dist >= 31) MotorForward(); //for auto mode
    else if (data == "?pin=13") MotorBackward(); 
    
    else if (data == "?pin=14") TurnLeft();
   
    else if (data == "?pin=12") TurnRight();
    
    else if (data == "?pin=15") MotorStop();
} 

/********************************************* FORWARD *****************************************************/
void MotorForward(void)   
{
  
  digitalWrite(pin1,HIGH);
  digitalWrite(pin2,HIGH);
  digitalWrite(pin3,HIGH);
  
}

/********************************************* BACKWARD *****************************************************/
void MotorBackward(void)   
{
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,HIGH);
  digitalWrite(pin3,HIGH);
}

/********************************************* TURN LEFT *****************************************************/
void TurnLeft(void)   
{
  digitalWrite(pin1,HIGH);
  digitalWrite(pin2,LOW);
  digitalWrite(pin3,HIGH);
}

/********************************************* TURN RIGHT *****************************************************/
void TurnRight(void)   
{
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
  digitalWrite(pin3,HIGH);
}

/********************************************* STOP *****************************************************/
void MotorStop(void)   
{
  digitalWrite(pin1,LOW);
  digitalWrite(pin2,LOW);
  digitalWrite(pin3,LOW);
  Serial.println(data);
}

/********************************** RECEIVE DATA FROM the APP ******************************************/
String checkClient (void)
{
  while(!client.available()) delay(1); 
  String request = client.readStringUntil('\r');
  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Access-Control-Allow-Origin: *");
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}
void connectWiFi()
{
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("..");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("NodeMCU Local IP is : ");
  Serial.print((WiFi.localIP()));
}
